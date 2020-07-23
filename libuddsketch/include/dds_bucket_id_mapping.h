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

#ifndef DDS_BUCKETID_MAPPING_H
#define DDS_BUCKETID_MAPPING_H

enum mapping_type {
    LOG_UPPER,
    LOG_LOWER
};

struct dds_bucket_id_mapping {
    double alpha;
    double gamma_value;
    double log_gamma;
    enum mapping_type type;
};

#ifdef __cplusplus
extern "C" {
#endif
struct dds_bucket_id_mapping *dds_bucket_id_mapping_init (enum mapping_type type, double alpha);
struct dds_bucket_id_mapping *dds_bucket_id_mapping_update (struct dds_bucket_id_mapping *bid_mapping, double gamma);
int dds_get_bucket_id(struct dds_bucket_id_mapping *bid_mapping, double value);
double dds_get_bucket_value(struct dds_bucket_id_mapping *bid_mapping, int bid);
double dds_get_max_addressable_value(struct dds_bucket_id_mapping *bid_mapping);
double dds_get_min_addressable_value(struct dds_bucket_id_mapping *bid_mapping);
void dds_bucket_id_mapping_destroy (struct dds_bucket_id_mapping * id_mapping);
#ifdef __cplusplus
}
#endif

#endif //DDS_BUCKETID_MAPPING_H
