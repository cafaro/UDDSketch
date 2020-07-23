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

#ifndef DDS_MAPSTORE_H
#define DDS_MAPSTORE_H

#include <stdbool.h>
#include "dds_store.h"
#include "dict/dict.h"

struct dds_mapstore;

typedef int (*dds_mapstore_resize_func)(struct dds_mapstore *, int);
typedef void (*dds_mapstore_collapse_func)(struct dds_mapstore *);

struct dds_mapstore_interface {
    dds_mapstore_resize_func resize;
    dds_mapstore_collapse_func collapse;
};

struct dds_mapstore {
    struct dds_store super;
    struct dds_mapstore_interface *vtable;

    dict *buckets;
    long total_counts;
    int max_num_buckets;
    bool resizable;
    bool is_collapsed;
    int num_collapses;
    enum store_type store_type;
    struct dds_bucket_id_mapping *id_mapping;
};

struct dds_mapstore *dds_mapstore_init(enum store_type store_type, bool resizable, int max_num_buckets, struct dds_bucket_id_mapping *id_map);
struct dds_mapstore *dds_mapstore_default_init(struct dds_bucket_id_mapping *id_map);
void dds_mapstore_destroy(struct dds_mapstore *store);
bool dds_mapstore_is_collapsed(struct dds_mapstore *store);
int dds_mapstore_get_num_buckets(struct dds_mapstore *store);
int dds_mapstore_resize(struct dds_mapstore *store, int new_length);
void dds_mapstore_collapse(struct dds_mapstore *store);

#endif //DDS_MAPSTORE_H
