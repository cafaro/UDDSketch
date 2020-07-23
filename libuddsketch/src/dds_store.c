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

#include "dds_store.h"

void dds_store_insert(struct dds_store *store, int bid, long count) {
    store->vtable->insert(store, bid, count);
}

void dds_store_remove(struct dds_store *store, int bid, long count) {
    store->vtable->remove(store, bid, count);
}

bool dds_store_is_empty(struct dds_store *store) {
    return store->vtable->is_empty(store);
}

long dds_store_get_total_counts(struct dds_store *store){
    return store->vtable->get_total_counts(store);
}

int dds_store_get_min_bid(struct dds_store* store){
    return store->vtable->get_min_bid(store);
}

int dds_store_get_max_bid(struct dds_store* store){
    return store->vtable->get_max_bid(store);
}

int dds_store_get_prev_bid(struct dds_store *store, int bid){
    return store->vtable->get_prev_bid(store, bid);
}

int dds_store_get_next_bid(struct dds_store *store, int bid){
    return store->vtable->get_next_bid(store, bid);
}

long dds_store_get_bucket_count(struct dds_store *store, int bid){
    return store->vtable->get_bucket_count(store, bid);
}

int dds_store_get_num_collapses(struct dds_store *store) {
    return store->vtable->get_num_collapses(store);
}

int dds_store_get_num_buckets(struct dds_store *store) {
    return store->vtable->get_num_buckets(store);
}