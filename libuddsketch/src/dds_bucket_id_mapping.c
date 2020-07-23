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

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "dds_bucket_id_mapping.h"

struct dds_bucket_id_mapping *dds_bucket_id_mapping_init (enum mapping_type type, double alpha) {
    struct dds_bucket_id_mapping *bid_mapping = (struct dds_bucket_id_mapping *)
                                                 malloc(sizeof(struct dds_bucket_id_mapping));
    bid_mapping->alpha = alpha;
    bid_mapping->gamma_value = (1 + alpha) / (1 - alpha);
    bid_mapping->log_gamma = log(bid_mapping->gamma_value);
    bid_mapping->type = type;
    return bid_mapping;
}

struct dds_bucket_id_mapping *dds_bucket_id_mapping_update (struct dds_bucket_id_mapping *bid_mapping, double gamma) {
    bid_mapping->gamma_value = gamma;
    bid_mapping->log_gamma = log(bid_mapping->gamma_value);
    bid_mapping->alpha = (gamma - 1)/(gamma + 1);
    return bid_mapping;
}

int dds_get_bucket_id(struct dds_bucket_id_mapping *bid_mapping, double item_value){
    int bid;
    switch (bid_mapping->type) {
        case LOG_UPPER:
            bid = (int)ceil(log(item_value)/bid_mapping->log_gamma);
            break;
        case LOG_LOWER:
            bid = (int)floor(log(item_value)/bid_mapping->log_gamma);
            bid = bid >= 0? bid : bid - 1;
            break;
        default:
            bid = (int)ceil(log(item_value)/bid_mapping->log_gamma);
    }

    return bid;
}

double dds_get_bucket_value(struct dds_bucket_id_mapping *bid_mapping, int bid) {
    double value;
    switch (bid_mapping->type) {
        case LOG_UPPER:
            value = exp(bid * bid_mapping->log_gamma) * (1 - bid_mapping->alpha);
            break;
        case LOG_LOWER:
            value = exp(bid * bid_mapping->log_gamma) * (1 + bid_mapping->alpha);
            break;
        default:
            value = exp(bid * bid_mapping->log_gamma) * (1 - bid_mapping->alpha);
    }

    return value;
}

double dds_get_max_addressable_value(struct dds_bucket_id_mapping *bid_mapping) {
    double den;
    switch (bid_mapping->type) {
        case LOG_UPPER:
            den = (1 - bid_mapping->alpha);
            break;
        case LOG_LOWER:
            den = (1 + bid_mapping->alpha);
            break;
        default:
            den = (1 - bid_mapping->alpha);
    }
    double max_addressable_value = fmin(exp(INT_MAX * bid_mapping->log_gamma),DBL_MAX / den);
    return max_addressable_value;
}

double dds_get_min_addressable_value(struct dds_bucket_id_mapping *bid_mapping){
    double min_addressable_value;
    switch (bid_mapping->type) {
        case LOG_UPPER:
            min_addressable_value = fmax(exp((INT_MIN) * bid_mapping->log_gamma), DBL_MIN);
            break;
        case LOG_LOWER:
            min_addressable_value = fmax(exp((INT_MIN + 1) * bid_mapping->log_gamma),
                                         DBL_MIN * bid_mapping->gamma_value);
            break;
        default:
            min_addressable_value = fmax(exp((INT_MIN) * bid_mapping->log_gamma), DBL_MIN);
    }
    return min_addressable_value;
}

void dds_bucket_id_mapping_destroy (struct dds_bucket_id_mapping * id_mapping) {
    free(id_mapping);
    id_mapping = NULL;
}