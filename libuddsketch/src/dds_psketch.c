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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dds_psketch.h>
#include "dds_sketch.h"
#include "dds_mapstore.h"

static void base_update_impl(struct dds_psketch *sketch, double value, long count){
    if (dds_sketch_is_addressable(sketch, value)) {
        if (value < sketch->min_addressable_value) {
            if (count > 0) {
                sketch->zero_bucket += count;
            } else if (count < 0 && (sketch->zero_bucket + count) >= 0) {
                sketch->zero_bucket += count;
            }
        } else {
            int bid = dds_get_bucket_id(sketch->id_map, value);
            if (count > 0) {
                dds_store_insert(sketch->store, bid, count);
            } else if (count < 0) {
                dds_store_remove(sketch->store, bid, count);
            }
        }
    }
}

static bool base_is_addressable_impl(struct dds_psketch *sketch, double value){
    if (value < 0 || value > sketch->max_addressable_value)
        return false;
    else
        return true;
}

static bool base_is_empty_impl(struct dds_psketch *sketch){
    return (sketch->zero_bucket == 0) && dds_store_is_empty(sketch->store);
}

static long base_get_total_count_impl(struct dds_psketch *sketch){
    return sketch->zero_bucket + dds_store_get_total_counts(sketch->store);
}

//static double get_rank_impl(struct dds_sketch* sketch, long rank) {
//    if (rank < 0) {
//        fprintf(stderr, "func get_rank_value_impl: bad rank value.\n");
//        return -1.0;
//    }
//
//    if (rank < sketch->zero_bucket) {
//        return 0;
//    }
//
//    int last_bid = dds_store_get_max_bid(sketch->store);
//    int first_bid = dds_store_get_min_bid(sketch->store);
//    long counts = 0;
//    int bid;
//    double quantile = ((double) rank/(double)(dds_sketch_get_total_count(sketch) - 1));
//    if (quantile <= 0.5) {
//        bid = first_bid;
//        counts = sketch->zero_bucket + dds_store_get_bucket_count(sketch->store, first_bid);
//        while (counts <= rank && bid < last_bid) {
//            bid = dds_store_get_next_bid(sketch->store, bid);
//            counts += dds_store_get_bucket_count(sketch->store, bid);
//        }
//    } else {
//        bid = last_bid;
//        counts = dds_store_get_total_counts(sketch->store) - dds_store_get_bucket_count(sketch->store, last_bid);
//        while (counts > rank && bid > first_bid) {
//            bid = dds_store_get_prev_bid(sketch->store, bid);
//            counts -= dds_store_get_bucket_count(sketch->store, bid);
//        }
//    }
//
//    return dds_get_bucket_value(sketch->id_map, bid);
//}

static double base_get_quantile_impl(struct dds_psketch* sketch, double quantile, bool *is_accurate){
    if (quantile < 0 || quantile > 1) {
        fprintf(stderr, "func get_quantile_impl: quantile must be in [0,1]\n");
        return -1.0;
    }
    *is_accurate = true;

    long rank = floor(quantile * (dds_sketch_get_total_count(sketch) - 1));
    //fprintf(stderr, "rank: %d, tc: %d, sc: %d\n", rank, dds_sketch_get_total_count(sketch), dds_store_get_total_counts(sketch->store));
    if (rank < sketch->zero_bucket) {
        return 0;
    }

    int last_bid = dds_store_get_max_bid(sketch->store);
    int first_bid = dds_store_get_min_bid(sketch->store);
    long counts = 0;
    int bid;
    // if (quantile <= 0.5) {
        bid = first_bid;
        counts = sketch->zero_bucket + dds_store_get_bucket_count(sketch->store, first_bid);
        while (counts <= rank && bid < last_bid) {
            bid = dds_store_get_next_bid(sketch->store, bid);
            counts += dds_store_get_bucket_count(sketch->store, bid);
        }
    // } 
    // else {
    //      bid = last_bid;
    //      counts = dds_store_get_total_counts(sketch->store) - dds_store_get_bucket_count(sketch->store, last_bid);
    //      while (counts > rank && bid > first_bid) {
    //          bid = dds_store_get_prev_bid(sketch->store, bid);
    //          counts -= dds_store_get_bucket_count(sketch->store, bid);
    //     }
    //     bid = dds_store_get_next_bid(sketch->store, bid);
    // }

    if (dds_sketch_get_num_collapses(sketch) > 0) {
        if ((sketch->store_type == COLLAPSINGLOW_MAPSTORE && bid == first_bid) ||
            (sketch->store_type == COLLAPSINGHIGH_MAPSTORE && bid == last_bid)) {
            *is_accurate = false;
        }
    }

    return dds_get_bucket_value(sketch->id_map, bid);
}

struct dds_bucket_id_mapping *base_get_id_mapping_impl (struct dds_psketch *sketch) {
    return sketch->id_map;
}

static int base_get_num_collapses_impl(struct dds_psketch *sketch) {
    return dds_store_get_num_collapses(sketch->store);
}

static int base_get_size_impl(struct dds_psketch *sketch) {
    return dds_store_get_num_buckets(sketch->store);
}

static struct dds_sketch_interface  base_vtable = {
        (dds_sketch_update_func)base_update_impl,
        (dds_sketch_is_addressable_func)base_is_addressable_impl,
        (dds_sketch_get_total_count_func)base_get_total_count_impl,
        (dds_sketch_get_quantile_func)base_get_quantile_impl,
        (dds_sketch_is_empty_func)base_is_empty_impl,
        (dds_sketch_get_id_mapping_func)base_get_id_mapping_impl,
        (dds_sketch_get_num_collpases_func)base_get_num_collapses_impl,
        (dds_sketch_get_size_func)base_get_size_impl
};

struct dds_psketch* dds_psketch_init(enum store_type store_type,
                                     double alpha,
                                     double min_addressable_value,
                                     int max_store_size)
{
    struct dds_psketch *sketch = (struct dds_psketch*)malloc(sizeof(struct dds_psketch));
    sketch->super.vtable = &base_vtable;
    sketch->id_map = dds_bucket_id_mapping_init(LOG_UPPER, alpha);

    sketch->store = (struct dds_store*)dds_mapstore_init(store_type, false, max_store_size, sketch->id_map);

    sketch->store_type = store_type;
    sketch->zero_bucket = 0;
    double min_mappable_value = dds_get_min_addressable_value(sketch->id_map);
    sketch->min_addressable_value = min_addressable_value < min_mappable_value? min_mappable_value : min_addressable_value;
    sketch->max_addressable_value = dds_get_max_addressable_value(sketch->id_map);

    return sketch;
}

void dds_psketch_destroy(struct dds_psketch* sketch)
{
    dds_mapstore_destroy(sketch->store);
    dds_bucket_id_mapping_destroy(sketch->id_map);
    free(sketch);
    sketch = NULL;
}