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

#include "dds_sketch.h"


void dds_sketch_update(struct dds_sketch *sketch, double value, long count){
    sketch->vtable->update(sketch, value, count);
}

bool dds_sketch_is_addressable(struct dds_sketch *sketch, double  value){
    return sketch->vtable->is_addressable(sketch, value);
}

long dds_sketch_get_total_count(struct dds_sketch *sketch){
    return sketch->vtable->get_total_count(sketch);
}

//double dds_sketch_get_rank(struct dds_sketch *sketch, long rank){
//    return sketch->vtable->get_rank(sketch, rank);
//}

double dds_sketch_get_quantile(struct dds_sketch *sketch, double quantile, bool *is_accurate){
    return sketch->vtable->get_quantile(sketch, quantile, is_accurate);
}

bool dds_sketch_is_empty(struct dds_sketch *sketch){
    return sketch->vtable->is_empty(sketch);
}

struct dds_bucket_id_mapping *dds_sketch_get_id_mapping(struct dds_sketch *sketch) {
    return sketch->vtable->get_id_mapping(sketch);
}

int dds_sketch_get_num_collapses(struct dds_sketch *sketch) {
    return sketch->vtable->get_num_collapses(sketch);
}

int dds_sketch_get_size(struct dds_sketch *sketch) {
    return sketch->vtable->get_size(sketch);
}