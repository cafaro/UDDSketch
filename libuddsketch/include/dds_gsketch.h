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

#ifndef DDS_GSKETCH_H
#define DDS_GSKETCH_H

#include "dds_sketch.h"

struct dds_gsketch {
    struct dds_sketch super;

    struct dds_store *nestore;
    struct dds_store *postore;
    struct dds_bucket_id_mapping *id_map;
    long zero_bucket;
    int max_sketch_size;
    double min_addressable_absvalue;
    double max_addressable_absvalue;
    enum store_type store_type;
};
#ifdef __cplusplus
extern "C" {
#endif
struct dds_gsketch* dds_gsketch_init(enum store_type store_type,
                                     double alpha,
                                     double min_addressable_value,
                                     int max_store_size);

void dds_gsketch_destroy(struct dds_gsketch* sketch);
#ifdef __cplusplus
}
#endif

#endif //DDS_GSKETCH_H
