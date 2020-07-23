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

#ifndef DDS_BUCKET_STORE_H
#define DDS_BUCKET_STORE_H

#include <stdbool.h>

struct dds_store;

typedef void (*dds_store_insert_func)(struct dds_store*, int, long);
typedef void (*dds_store_remove_func)(struct dds_store*, int, long);
typedef bool (*dds_store_is_empty_func)(struct dds_store*);
typedef long (*dds_store_get_total_counts_func)(struct dds_store*);
typedef int (*dds_store_get_min_bid_func)(struct dds_store*);
typedef int (*dds_store_get_max_bid_func)(struct dds_store*);
typedef int (*dds_store_get_prev_bid_func)(struct dds_store*, int);
typedef int (*dds_store_get_next_bid_func)(struct dds_store*, int);
typedef long (*dds_store_get_bucket_count_func)(struct dds_store*, int);
typedef int (*dds_store_get_num_collapses_func)(struct dds_store*);
typedef int (*dds_store_get_num_buckets_func)(struct dds_store*);


enum store_type {
    UNBOUNDED_MAPSTORE = 0,
    COLLAPSINGHIGH_MAPSTORE = 1,
    COLLAPSINGLOW_MAPSTORE = 2,
    COLLAPSINGALL_MAPSTORE = 3
};

struct dds_store_interface {
    dds_store_insert_func insert;
    dds_store_remove_func remove;
    dds_store_is_empty_func is_empty;
    dds_store_get_total_counts_func get_total_counts;
    dds_store_get_min_bid_func get_min_bid;
    dds_store_get_max_bid_func get_max_bid;
    dds_store_get_prev_bid_func get_prev_bid;
    dds_store_get_next_bid_func get_next_bid;
    dds_store_get_bucket_count_func get_bucket_count;
    dds_store_get_num_collapses_func get_num_collapses;
    dds_store_get_num_buckets_func get_num_buckets;
};

struct dds_store {
    struct dds_store_interface *vtable;
};
#ifdef __cplusplus
extern "C" {
#endif

void dds_store_insert(struct dds_store *store, int bid, long count);
void dds_store_remove(struct dds_store *store, int bid, long count);
bool dds_store_is_empty(struct dds_store *store);
long dds_store_get_total_counts(struct dds_store *store);
int dds_store_get_min_bid(struct dds_store* store);
int dds_store_get_max_bid(struct dds_store* store);
int dds_store_get_prev_bid(struct dds_store*, int bid);
int dds_store_get_next_bid(struct dds_store*, int bid);
long dds_store_get_bucket_count(struct dds_store*, int bid);
int dds_store_get_num_collapses(struct dds_store*);
int dds_store_get_num_buckets(struct dds_store*);


#ifdef __cplusplus
}
#endif

#endif //DDS_BUCKET_STORE_H
