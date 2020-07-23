/*
 * Copyright (c) 2019 Marco Pulimeno, University of Salento
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so,
 * provided that the above copyright notice(s) and this permission notice
 * appear in all copies of the Software and that both the above copyright
 * notice(s) and this permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
 * LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization of the
 * copyright holder.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "dds_mapstore.h"
#include "dds_gsketch.h"

static int get_num_pvalue_buckets (struct dds_gsketch *gsketch) {
    return dds_store_get_num_buckets(gsketch->postore);
}

static int get_num_nvalue_buckets (struct dds_gsketch *gsketch) {
    return dds_store_get_num_buckets(gsketch->nestore);
}

static void base_update_impl(struct dds_gsketch *gsketch, double value, long count){
    double absvalue = fabs(value);
    if (dds_sketch_is_addressable(gsketch, absvalue)) {
        if (absvalue < gsketch->min_addressable_absvalue) {
            if (count > 0) {
                gsketch->zero_bucket += count;
            } else if (count < 0 && gsketch->zero_bucket >= count) {
                gsketch->zero_bucket -= count;
            }
        } else {
            int bid = dds_get_bucket_id(gsketch->id_map, absvalue);
            if (count > 0) {
                if (value > 0) {
                    dds_store_insert(gsketch->postore, bid, count);
                } else {
                    dds_store_insert(gsketch->nestore, bid, count);
                }
            } else if (count < 0) {
                if (value > 0) {
                    dds_store_remove(gsketch->postore, bid, count);
                } else {
                    dds_store_remove(gsketch->nestore, bid, count);
                }
            }
        }
    }
    int num_pbuckets = get_num_pvalue_buckets(gsketch);
    int num_nbuckets = get_num_nvalue_buckets(gsketch);
    int num_buckets = num_pbuckets + num_nbuckets;
    if (num_buckets > gsketch->max_sketch_size) {
        switch(gsketch->store_type) {
            case COLLAPSINGHIGH_MAPSTORE:
                if (num_nbuckets < gsketch->max_sketch_size) {
                    dds_mapstore_resize((struct dds_mapstore*)gsketch->postore, gsketch->max_sketch_size - num_nbuckets);
                } else {
                    num_pbuckets = dds_mapstore_resize((struct dds_mapstore*)gsketch->postore, 1);
                    dds_mapstore_resize((struct dds_mapstore*)gsketch->nestore, gsketch->max_sketch_size - num_pbuckets);
                }
                break;
            case COLLAPSINGLOW_MAPSTORE:
                if (num_pbuckets < gsketch->max_sketch_size) {
                    dds_mapstore_resize((struct dds_mapstore*)gsketch->nestore, gsketch->max_sketch_size - num_pbuckets);
                } else {
                    num_nbuckets = dds_mapstore_resize((struct dds_mapstore*)gsketch->nestore, 1);
                    dds_mapstore_resize((struct dds_mapstore*)gsketch->postore, gsketch->max_sketch_size - num_nbuckets);
                }
                break;
            case COLLAPSINGALL_MAPSTORE:
                while (num_buckets > gsketch->max_sketch_size) { // we reached the max number of buckets
                    num_buckets = 0;
                    if (dds_store_get_num_buckets(gsketch->nestore) > 0) {
                        dds_mapstore_collapse((struct dds_mapstore *) gsketch->nestore);
                        num_buckets += dds_store_get_num_buckets(gsketch->nestore);
                    }
                    if (dds_store_get_num_buckets(gsketch->postore) > 0) {
                        dds_mapstore_collapse((struct dds_mapstore *) gsketch->postore);
                        num_buckets += dds_store_get_num_buckets(gsketch->postore);
                    }
                    dds_bucket_id_mapping_update(dds_sketch_get_id_mapping((struct dds_sketch*)gsketch), pow(dds_sketch_get_id_mapping((struct dds_sketch*)gsketch)->gamma_value,2.0));
                }
                break;
            default:
                return;
        }
    }
}

static bool base_is_addressable_impl(struct dds_gsketch *gsketch, double value){
    value = fabs(value);
    if (value < 0 || value > gsketch->max_addressable_absvalue)
        return false;
    else
        return true;
}

static long base_get_total_count_impl(struct dds_gsketch *gsketch){
    long tcount = gsketch->zero_bucket +
                  dds_store_get_total_counts(gsketch->postore) +
                  dds_store_get_total_counts(gsketch->nestore);

    return tcount;
}

static int rank_to_bid(struct dds_store *store, long rank) {
    int last_bid = dds_store_get_max_bid(store);
    int first_bid = dds_store_get_min_bid(store);
    long counts = 0;
    int bid;
    double pseudo_q = (double)rank/(double)dds_store_get_total_counts(store);
    if ( pseudo_q <= 0.5) {
        bid = first_bid;
        counts = dds_store_get_bucket_count(store, first_bid);
        while (counts <= rank && bid < last_bid) {
            bid = dds_store_get_next_bid(store, bid);
            counts += dds_store_get_bucket_count(store, bid);
        }
    } else {
        bid = last_bid;
        counts = dds_store_get_total_counts(store) - dds_store_get_bucket_count(store, last_bid);
        while (counts > rank && bid > first_bid) {
            bid = dds_store_get_prev_bid(store, bid);
            counts -= dds_store_get_bucket_count(store, bid);
        }
    }

    return bid;
}

static double base_get_quantile_impl(struct dds_gsketch *gsketch, double quantile, bool *is_accurate) {
    if (quantile < 0 || quantile > 1) {
        fprintf(stderr, "func get_quantile_impl: quantile must be in [0,1]\n");
        return -1.0;
    }

    *is_accurate = true;

    long rank = floor(quantile * (double)(dds_sketch_get_total_count((struct dds_sketch*)gsketch) - 1));
    long netotcount = dds_store_get_total_counts(gsketch->nestore);
    int bid = 0;
    if (rank < netotcount) {
        rank = netotcount - rank - 1;
        bid = rank_to_bid(gsketch->nestore, rank);
        int last_bid = dds_store_get_max_bid(gsketch->nestore);
        int first_bid = dds_store_get_min_bid(gsketch->nestore);
        if (dds_store_get_num_collapses(gsketch->nestore) > 0) {
            if ((gsketch->store_type == COLLAPSINGLOW_MAPSTORE && bid == last_bid) ||
                (gsketch->store_type == COLLAPSINGHIGH_MAPSTORE && bid == first_bid)) {
                *is_accurate = false;
            }
        }
        return -dds_get_bucket_value(gsketch->id_map, bid);
    } else if (rank >= netotcount && rank < netotcount+gsketch->zero_bucket) {
        return 0;
    } else {
        rank = rank - netotcount - gsketch->zero_bucket;
        bid = rank_to_bid(gsketch->postore, rank);
        int last_bid = dds_store_get_max_bid(gsketch->postore);
        int first_bid = dds_store_get_min_bid(gsketch->postore);
        if (dds_store_get_num_collapses(gsketch->postore) > 0) {
            if ((gsketch->store_type == COLLAPSINGLOW_MAPSTORE && bid == first_bid) ||
                (gsketch->store_type == COLLAPSINGHIGH_MAPSTORE && bid == last_bid)) {
                *is_accurate = false;
            }
        }
        return dds_get_bucket_value(gsketch->id_map, bid);
    }
}

static bool base_is_empty_impl(struct dds_gsketch *gsketch){
    bool is_empty = gsketch->zero_bucket == 0            &&
                    dds_store_is_empty(gsketch->postore) &&
                    dds_store_is_empty(gsketch->nestore);
    return is_empty;
}

static struct dds_bucket_id_mapping* base_get_id_mapping_impl(struct dds_gsketch* gsketch) {
    return gsketch->id_map;
}

static int base_get_num_collapses_impl(struct dds_gsketch* gsketch) {
    if (gsketch->store_type == COLLAPSINGALL_MAPSTORE) {
        return fmax(dds_store_get_num_collapses(gsketch->nestore), dds_store_get_num_collapses(gsketch->postore));
    } else {
        return dds_store_get_num_collapses(gsketch->nestore) + dds_store_get_num_collapses(gsketch->postore);
    }
}

static int base_get_size_impl(struct dds_gsketch* gsketch) {
    return dds_store_get_num_buckets(gsketch->nestore) + dds_store_get_num_buckets(gsketch->postore);
}

static struct dds_sketch_interface base_vtable = {
        (dds_sketch_update_func)base_update_impl,
        (dds_sketch_is_addressable_func)base_is_addressable_impl,
        (dds_sketch_get_total_count_func)base_get_total_count_impl,
        (dds_sketch_get_quantile_func)base_get_quantile_impl,
        (dds_sketch_is_empty_func)base_is_empty_impl,
        (dds_sketch_get_id_mapping_func)base_get_id_mapping_impl,
        (dds_sketch_get_num_collpases_func)base_get_num_collapses_impl,
        (dds_sketch_get_size_func)base_get_size_impl
};

struct dds_gsketch *dds_gsketch_init(enum store_type store_type,
                                           double alpha,
                                           double min_addressable_absvalue,
                                           int max_store_size) {
    struct dds_gsketch *sketch = (struct dds_gsketch*)calloc(sizeof(struct dds_gsketch), 1);
    sketch->super.vtable = &base_vtable;
    sketch->id_map = dds_bucket_id_mapping_init(LOG_UPPER, alpha);
    switch (store_type) {
        case UNBOUNDED_MAPSTORE:
            sketch->nestore = (struct dds_store*)dds_mapstore_init(UNBOUNDED_MAPSTORE, true, 0, sketch->id_map);
            sketch->postore = (struct dds_store*)dds_mapstore_init(UNBOUNDED_MAPSTORE, true, 0, sketch->id_map);
            break;
        case COLLAPSINGHIGH_MAPSTORE:
            sketch->postore = (struct dds_store*)dds_mapstore_init(COLLAPSINGHIGH_MAPSTORE, true, 0, sketch->id_map);
            sketch->nestore = (struct dds_store*)dds_mapstore_init(COLLAPSINGLOW_MAPSTORE, true, 0, sketch->id_map);
            break;
        case COLLAPSINGLOW_MAPSTORE:
            sketch->postore = (struct dds_store*)dds_mapstore_init(COLLAPSINGLOW_MAPSTORE, true, 0, sketch->id_map);
            sketch->nestore = (struct dds_store*)dds_mapstore_init(COLLAPSINGHIGH_MAPSTORE, true, 0, sketch->id_map);
            break;
        case COLLAPSINGALL_MAPSTORE:
            sketch->postore = (struct dds_store*)dds_mapstore_init(COLLAPSINGALL_MAPSTORE, true, 0, sketch->id_map);
            sketch->nestore = (struct dds_store*)dds_mapstore_init(COLLAPSINGALL_MAPSTORE, true, 0, sketch->id_map);
            break;
        default:
            fprintf(stderr, "func dds_array_sketch_init: choice not implemented\n");
            exit(EXIT_FAILURE);
    }

    sketch->store_type = store_type;
    sketch->zero_bucket = 0;
    double min_mappable_value = dds_get_min_addressable_value(sketch->id_map);
    sketch->min_addressable_absvalue = min_addressable_absvalue < min_mappable_value? min_mappable_value : min_addressable_absvalue;
    sketch->max_addressable_absvalue = dds_get_max_addressable_value(sketch->id_map);
    sketch->max_sketch_size = max_store_size;
    return sketch;
    

}

void dds_gsketch_destroy(struct dds_gsketch *gsketch){
    dds_mapstore_destroy(gsketch->nestore);
    dds_mapstore_destroy(gsketch->postore);
    dds_bucket_id_mapping_destroy(gsketch->id_map);
    free(gsketch);
    gsketch = NULL;
}



