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

#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "dds_bucket_id_mapping.h"
#include "dds_mapstore.h"

static int *new_bid(int key) {
    int *nkey = malloc(sizeof(int));
    if (!nkey) {
        fprintf(stderr, "new_key func: out of memory\n");
        abort();
    }
    *nkey = key;
    return nkey;
}

static long *new_count(long count) {
    long *ncount = malloc(sizeof(long));
    if (!ncount) {
        fprintf(stderr, "new_count func: out of memory\n");
        abort();
    }
    *ncount = count;
    return ncount;
}

static void add_item (dict *dict, int bid, long count) {
    int *bid_ptr = new_bid(bid);
    dict_insert_result result = dict_insert(dict, bid_ptr);
    if (result.inserted) {
        *result.datum_ptr = new_count(count);
    } else {
        **(long**)result.datum_ptr += count;
        free(bid_ptr);
    }
}

static void key_val_free(void *key, void *datum)
{
    free(key);
    free(datum);
}

static void collapse_impl (struct dds_mapstore *store) {
    dict_itor *itor = dict_itor_new(store->buckets);
    switch (store->store_type) {
        case UNBOUNDED_MAPSTORE:
            return;
        case COLLAPSINGLOW_MAPSTORE: {
            dict_itor_first(itor);
            long **first_datum_ptr = dict_itor_datum(itor);
            int *first_key_ptr = dict_itor_key(itor);
            dict_itor_next(itor);
            if (!dict_itor_valid(itor)) return;
            long **second_datum_ptr = dict_itor_datum(itor);
            long new_count = **(long**)first_datum_ptr + **(long**)second_datum_ptr;
            **(long**)second_datum_ptr = new_count;
            dict_remove_result result = dict_remove(store->buckets, first_key_ptr);

            if (result.key) {
                free(result.key);
                result.key = NULL;
            }

            if (result.datum) {
                free(result.datum);
                result.datum = NULL;
            }
        }
            break;
        case COLLAPSINGHIGH_MAPSTORE: {
            dict_itor_last(itor);
            long **last_datum_ptr = dict_itor_datum(itor);
            int *last_key_ptr = dict_itor_key(itor);

            dict_itor_prev(itor);
            if (!dict_itor_valid(itor)) return;
            long **second_last_datum_ptr = dict_itor_datum(itor);

            long new_count = **(long**)second_last_datum_ptr + **(long**)last_datum_ptr;
            **(long**)second_last_datum_ptr = new_count;
            dict_remove_result result = dict_remove(store->buckets, last_key_ptr);
            if (result.key) {
                    free(result.key);
                    result.key = NULL;
                }

            if (result.datum) {
                free(result.datum);
                result.datum = NULL;
            }
        }
            break;
        case COLLAPSINGALL_MAPSTORE: {
            dict *new_dict = hb_dict_new(dict_int_cmp);
            dict_itor_first(itor);
            while(dict_itor_valid(itor)) {
                int bid = *(int*)dict_itor_key(itor);
                long value = **(long**)dict_itor_datum(itor);
                if (store->id_mapping->type == LOG_UPPER) {
                    bid = (int)ceil((double)bid / 2.0);
                } else {
                    bid = (int)floor((double)bid / 2.0);
                }
                add_item(new_dict, bid, value);
                dict_itor_next(itor);
            }
            dict_free(store->buckets, key_val_free);
            store->buckets = new_dict;
        }
            break;
        default:
            return;
    }
    dict_itor_free(itor);
    store->is_collapsed = true;
    store->num_collapses++;
}

static void base_insert_impl(struct dds_mapstore *store, int bid, long count) {
    if (count <= 0) {
        return;
    }

    dict_itor *itor = dict_itor_new(store->buckets);
    dict_itor_search(itor,&bid);
    if (dict_itor_valid(itor)) {
        long *value_ptr = *(long **)dict_itor_datum(itor);
        *value_ptr = *value_ptr + count;
    } else {
        if (store->is_collapsed) {
            if (store->store_type == COLLAPSINGHIGH_MAPSTORE ) {
                dict_itor_last(itor);
                if (bid > *(int*)dict_itor_key(itor)) {
                    **(long**)dict_itor_datum(itor) += count;
                } else {
                    add_item(store->buckets, bid, count);
                }
            } else if (store->store_type == COLLAPSINGLOW_MAPSTORE) {
                dict_itor_first(itor);
                if (bid < *(int*)dict_itor_key(itor)) {
                    **(long**)dict_itor_datum(itor) += count;
                } else {
                    add_item(store->buckets, bid, count);
                }
            } else {
                add_item(store->buckets, bid, count);
            }
        } else {
            add_item(store->buckets, bid, count);
        }
        if (!store->resizable) {
            if (store->store_type != UNBOUNDED_MAPSTORE) {
                int num_bucks = dict_count(store->buckets);
                while (num_bucks > store->max_num_buckets) {
                    dds_mapstore_collapse(store);
                    if (store->store_type == COLLAPSINGALL_MAPSTORE)
                        dds_bucket_id_mapping_update(store->id_mapping, pow(store->id_mapping->gamma_value,2.0));
                    num_bucks = dict_count(store->buckets);
                }
            }
        }
    }
    dict_itor_free(itor);
    store->total_counts += count;
}

static int resize_impl(struct dds_mapstore *store, int new_length) {
    if (!store->resizable || store->store_type == UNBOUNDED_MAPSTORE) {
        return dict_count(store->buckets);
    }
    int num_bucks = dict_count(store->buckets);
    while (num_bucks > new_length) {
        dds_mapstore_collapse(store);
        if (store->store_type == COLLAPSINGALL_MAPSTORE)
            dds_bucket_id_mapping_update(store->id_mapping, pow(store->id_mapping->gamma_value,2.0));
        num_bucks = dict_count(store->buckets);
    }
    return num_bucks;
}

static void base_remove_impl(struct dds_mapstore *store, int bid, long count) {
    if (count >= 0) {
        return;
    }
    count = -count;
    long *value_ptr = NULL;
    dict_itor *itor = dict_itor_new(store->buckets);
    dict_itor_last(itor);
    int max_bid = *(int*)dict_itor_key(itor);
    dict_itor_first(itor);
    int min_bid = *(int*)dict_itor_key(itor);
    if (store->is_collapsed) {
        if (store->store_type == COLLAPSINGHIGH_MAPSTORE && bid >= max_bid) {
            dict_itor_last(itor);
            value_ptr = *(long**)dict_itor_datum(itor);
            if (*value_ptr == count) {
                store->is_collapsed = false;
                store->num_collapses = 0;
                bid = max_bid;
            }
        } else if (store->store_type == COLLAPSINGLOW_MAPSTORE && bid <= min_bid) {
            dict_itor_first(itor);
            value_ptr = *(long**)dict_itor_datum(itor);
            if (*value_ptr == count) {
                store->is_collapsed = false;
                store->num_collapses = 0;
                bid = min_bid;
            }
        } else {
            dict_itor_search(itor, &bid);
            if (!dict_itor_valid(itor)) {
                //fprintf(stderr, "remove_impl: bucket not found\n");
                return;
            }
            value_ptr = *(long**)dict_itor_datum(itor);
        }
    } else {
        dict_itor_search(itor, &bid);
        if (!dict_itor_valid(itor)) {
            //fprintf(stderr, "remove_impl: bucket not found\n");
            return;
        }
        value_ptr = *(long**)dict_itor_datum(itor);
    }

    if (*value_ptr < count) {
        //fprintf(stderr, "remove_impl: bucket count is not enough\n");
        return;
    }
    *value_ptr = *value_ptr - count;
    if (*value_ptr == 0) {
        dict_remove_result result = dict_remove(store->buckets, &bid);
            if (result.key) {
                    free(result.key);
                    result.key = NULL;
                }

            if (result.datum) {
                free(result.datum);
                result.datum = NULL;
            }
        
    }
    dict_itor_free(itor);
    store->total_counts -= count;
}

static bool base_is_empty_impl(struct dds_mapstore* store) {
    return !dict_count(store->buckets);
}

static long base_get_total_counts_impl(struct dds_mapstore *store) {
//    dict_itor *itor = dict_itor_new(store->buckets);
//    dict_itor_first(itor);
//    long tc = 0;
//    while(dict_itor_valid(itor)) {
//        tc += **(long**)dict_itor_datum(itor);
//        dict_itor_next(itor);
//    }
//    return tc;
    return store->total_counts;
}

int base_get_min_bid_impl(struct dds_mapstore *store) {
    dict_itor *itor = dict_itor_new(store->buckets);

    dict_itor_first(itor);

    int bid = *(int*)dict_itor_key(itor);

    dict_itor_free(itor);
    return bid;
}

static int base_get_max_bid_impl(struct dds_mapstore *store) {
    dict_itor *itor = dict_itor_new(store->buckets);

    dict_itor_last(itor);

    int bid = *(int*)dict_itor_key(itor);

    dict_itor_free(itor);
    return bid;
}

static int base_get_prev_bid_impl(struct dds_mapstore *store, int bid) {
    dict_itor *itor = dict_itor_new(store->buckets);
    dict_itor_search(itor, &bid);

    if (!dict_itor_valid(itor)) {
        fprintf(stderr, "get_next_bid func: something went wrong.\n");
        return bid;
    }

    dict_itor_prev(itor);
    int nbid;

    if (dict_itor_valid(itor)) {
        nbid = *(int*)dict_itor_key(itor);
    } else {
        nbid = bid;
    }
    dict_itor_free(itor);

    return nbid;
}

static int base_get_next_bid_impl(struct dds_mapstore *store, int bid) {
    dict_itor *itor = dict_itor_new(store->buckets);
    dict_itor_search(itor, &bid);

    if (!dict_itor_valid(itor)) {
        fprintf(stderr, "get_next_bid func: something went wrong.\n");
        return bid;
    }

    dict_itor_next(itor);
    int nbid;

    if (dict_itor_valid(itor)) {
        nbid = *(int*)dict_itor_key(itor);
    } else {
        nbid = bid;
    }
    dict_itor_free(itor);

    return nbid;
}

static long base_get_bucket_count_impl(struct dds_mapstore *store, int bid) {
    void **count_ptr = dict_search(store->buckets, &bid);
    if (count_ptr == NULL) {
        fprintf(stderr, "get_bucket_count func: something went wrong.\n");
        return -1;
    }
    return **(long**)count_ptr;
}

static int base_get_num_collapses_impl(struct dds_mapstore *store) {
    return store->num_collapses;
}

static int base_get_num_buckets_impl(struct dds_mapstore *store) {
    return dict_count(store->buckets);
}

static struct dds_store_interface base_vtable = {
        (dds_store_insert_func)base_insert_impl,
        (dds_store_remove_func)base_remove_impl,
        (dds_store_is_empty_func)base_is_empty_impl,
        (dds_store_get_total_counts_func)base_get_total_counts_impl,
        (dds_store_get_min_bid_func)base_get_min_bid_impl,
        (dds_store_get_max_bid_func)base_get_max_bid_impl,
        (dds_store_get_prev_bid_func)base_get_prev_bid_impl,
        (dds_store_get_next_bid_func)base_get_next_bid_impl,
        (dds_store_get_bucket_count_func)base_get_bucket_count_impl,
        (dds_store_get_num_collapses_func) base_get_num_collapses_impl,
        (dds_store_get_num_buckets_func) base_get_num_buckets_impl
};

static struct dds_mapstore_interface vtable =  {
        resize_impl,
        collapse_impl
};

struct dds_mapstore* dds_mapstore_init(enum store_type store_type, bool resizable, int max_num_buckets, struct dds_bucket_id_mapping *id_map) {
    struct dds_mapstore *mapstore = (struct dds_mapstore*)malloc(sizeof(struct dds_mapstore));
    mapstore->super.vtable = &base_vtable;
    mapstore->vtable = &vtable;
    mapstore->buckets = wb_dict_new(dict_int_cmp);
    mapstore->total_counts = 0;
    mapstore->max_num_buckets = max_num_buckets;
    mapstore->id_mapping = id_map;
    mapstore->store_type = store_type;
    mapstore->is_collapsed = false;
    mapstore->resizable = resizable;
    mapstore->num_collapses = 0;

    return mapstore;
}

struct dds_mapstore* dds_mapstore_default_init(struct dds_bucket_id_mapping *id_map) {
    struct dds_mapstore *result;
    result = dds_mapstore_init(UNBOUNDED_MAPSTORE, false, 0, id_map);
    return result;
}


void dds_mapstore_destroy(struct dds_mapstore *store) {
    dict_free(store->buckets, key_val_free);
    free(store);
    store = NULL;
}

bool dds_mapstore_is_collapsed(struct dds_mapstore *store) {
    return store->is_collapsed;
}

int dds_mapstore_resize(struct dds_mapstore *store, int new_length){
    return store->vtable->resize(store, new_length);
}

void dds_mapstore_collapse(struct dds_mapstore *store) {
    store->vtable->collapse(store);
}
