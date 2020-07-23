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

#ifndef DDS_SKETCH_H
#define DDS_SKETCH_H

#include "dds_store.h"
#include "dds_bucket_id_mapping.h"

struct dds_sketch;

typedef void (*dds_sketch_update_func)(struct dds_sketch*, double, long);
typedef bool (*dds_sketch_is_addressable_func)(struct dds_sketch*, double);
typedef long (*dds_sketch_get_total_count_func)(struct dds_sketch*);
typedef double (*dds_sketch_get_quantile_func)(struct dds_sketch*, double, bool*);
typedef bool (*dds_sketch_is_empty_func)(struct dds_sketch*);
typedef struct dds_bucket_id_mapping* (*dds_sketch_get_id_mapping_func)(struct dds_sketch*);
typedef int (*dds_sketch_get_num_collpases_func)(struct dds_sketch*);
typedef int (*dds_sketch_get_size_func)(struct dds_sketch*);

struct dds_sketch_interface {
    dds_sketch_update_func update;
    dds_sketch_is_addressable_func is_addressable;
    dds_sketch_get_total_count_func get_total_count;
    dds_sketch_get_quantile_func get_quantile;
    dds_sketch_is_empty_func is_empty;
    dds_sketch_get_id_mapping_func get_id_mapping;
    dds_sketch_get_num_collpases_func get_num_collapses;
    dds_sketch_get_size_func get_size;
};

struct dds_sketch {
    struct dds_sketch_interface *vtable;
};


#ifdef __cplusplus
extern "C" {
#endif

void dds_sketch_update(struct dds_sketch *sketch, double value, long count);
bool dds_sketch_is_addressable(struct dds_sketch *sketch, double value);
long dds_sketch_get_total_count(struct dds_sketch *sketch);
double dds_sketch_get_quantile(struct dds_sketch *sketch, double quantile, bool *is_accurate);
bool dds_sketch_is_empty(struct dds_sketch *sketch);
struct dds_bucket_id_mapping *dds_sketch_get_id_mapping(struct dds_sketch *sketch);
int dds_sketch_get_num_collapses(struct dds_sketch *sketch);
int dds_sketch_get_size(struct dds_sketch *sketch);
//double dds_sketch_get_rank(struct dds_sketch *sketch, long rank);

#ifdef __cplusplus
}
#endif

#include "dds_psketch.h"
#include "dds_gsketch.h"

#endif //DDS_SKETCH_H
