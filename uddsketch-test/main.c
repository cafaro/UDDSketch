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
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <time.h>
#include <math.h>
#include "dds_sketch.h"
#include "textcolor_macros.h"

#define UNIFORM_WITH_INITIAL_ALPHA 4
#define BACKWARD_COLLAPSES 11

char *store_type_to_str(enum store_type stype){
    switch (stype) {
        case UNBOUNDED_MAPSTORE:
            return "UNBOUNDED MAPSTORE";
        case COLLAPSINGHIGH_MAPSTORE:
            return "HIGH COLLAPSING MAPSTORE";
        case COLLAPSINGLOW_MAPSTORE:
            return "LOW COLLAPSING MAPSTORE";
        case COLLAPSINGALL_MAPSTORE:
            return "UNIFORM COLLAPSING MAPSTORE";
        case UNIFORM_WITH_INITIAL_ALPHA:
            return "UNIFORM COLLAPSING WITH INITIAL ALPHA";
        default:
            return NULL;
    }
}

void print_statistics(double alpha, double *sketch_quantiles, double *exact_quantiles, int step){
    printf(FG_LIGHTYELLOW("\n%-5s|\t%-21s\t|\t%-21s\t|\t%-21s\n\n"), " q", " Computed quantiles", "   Exact quantiles", "   Relative Error");
    double avg_re = 0.0;
    for (int p = 1; p < 100; p++) {
        double re = 0.0;
        if (fabs(exact_quantiles[p]) > 0.0) {
            re = fabs((sketch_quantiles[p] - exact_quantiles[p]))/fabs(exact_quantiles[p]);
            avg_re += re;
        }
        if (!(p % step)) {
            printf(FG_LIGHTGREEN("%3.2f | %21.9f\t|\t%21.9f\t|\t"), p/100.0, sketch_quantiles[p], exact_quantiles[p]);
            if ((re - alpha) < 0.000001)
                printf(FG_LIGHTGREEN("%21.9f"), re);
            else
                printf(FG_LIGHTRED("%21.9f"), re);
            printf("\n");
        }
    }
    printf(FG_LIGHTYELLOW("\nAverage relative error over all of the percentiles: %.9f\n"), avg_re/101.0);
}

void print_statistics_csv(double *sketch_quantiles, double *exact_quantiles, int step){
    double avg_re = 0.0;
    for (int p = 0; p < 101; p++) {
        double re = 0.0;
        if (fabs(exact_quantiles[p]) > 0) {
            re = fabs((sketch_quantiles[p] - exact_quantiles[p])) / fabs(exact_quantiles[p]);
            avg_re += re;
        }
        if (!(p % step)) {
            printf("%.10f,", re);
        }
    }
    printf("%.10f,", avg_re/101.0);
}

void print_usage(char *prog_name) {
    printf("\nUsage: %s -f <input file name>\n"
           "    Options\n"
           "      -n: needed if input contains negative items\n"
           "      -a <alpha value>: alpha value\n"
           "      -s <store type>: type of store\n"
           "      -b <max sketch size>: max sketch size in case of bounded store\n"
           "      -o: for csv output\n\n", prog_name);
}

int compare_doubles(const void* a, const void* b)
{
    double arg1 = *(const double*)a;
    double arg2 = *(const double*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

int main(int argc, char *argv[]) {

    char *input_filename = NULL;
    enum store_type store_type = UNBOUNDED_MAPSTORE;
    int sketch_size_bound = 256;
    double alpha = 0.01;
    double user_alpha = 0.0;
    bool neg_items = false;
    bool csv_output = false;

    opterr = 0;
    char c;
    while ((c = getopt (argc, argv, "hf:a:b:s:no")) != -1) {
        switch (c) {
            case 'f':
                input_filename = optarg;
                break;
            case 'b':
                sketch_size_bound = strtol(optarg, NULL, 10);
                break;
            case 'a':
                alpha = strtod(optarg, NULL);
                break;
            case 's':
                store_type = strtol(optarg, NULL, 10);
                if (store_type < 0 || store_type > 4) {
                    fprintf(stderr, "Option -s argument can be:\n"
                                    " 0: unbounded map store (no collapses, implements DDSketch)\n"
                                    " 1: collapsing last two buckets map store (note: the last two buckets are not necessarily adjacent, implements DDSketch)\n"
                                    " 2: collapsing first two buckets map store (note: the first two buckets are not necessarily adjacent, implements DDSketch\n"
                                    " 3: uniform collapse map store (implements UDDSketch)\n"
                                    " 4: uniform collapse map store with initial alpha setting (implements UDDSketch)\n");
                    exit(1);
                }
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'n':
                neg_items = true;
                break;
            case 'o':
                csv_output = true;
                break;
            case '?':
                if (optopt == 'f' || optopt == 'a' || optopt == 'b' || optopt == 's' || optopt == 'l' || optopt == 'g')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                exit(1);
            default:
                exit(1);
        }
    }

    setlocale(LC_CTYPE, "");

    if (input_filename == NULL) {
        fprintf(stderr, "Option -f is mandatory.\n");
        exit(1);
    }

    if (store_type == UNBOUNDED_MAPSTORE)
        sketch_size_bound = 0;

    user_alpha = alpha;

    FILE *ifile = fopen(input_filename, "rb");
    if (!ifile) {
        fprintf(stderr, FG_LIGHTRED("Input file name mispelled or file does not exist\n"));
        exit(EXIT_FAILURE);
    }

    fseek(ifile, 0, SEEK_END);
    long fdim = ftell(ifile);
    rewind(ifile);
    long slength = fdim / sizeof(double);
    double *istream = (double*)malloc(sizeof(double)*slength);
    fread(istream, sizeof(double), slength, ifile);
    fclose(ifile);


    if (!csv_output) {
        fprintf(stdout, FG_LIGHTYELLOW("\nDDSketch launched with the following parameters:\n"));
        fprintf(stdout, FG_LIGHTGREEN("- input filename = %s\n"), input_filename);
        fprintf(stdout, FG_LIGHTGREEN("- input size = %ld\n"), slength);
        fprintf(stdout, FG_LIGHTGREEN("- store type = %s\n"), store_type_to_str(store_type));
        if (store_type != UNBOUNDED_MAPSTORE) {
            fprintf(stdout, FG_LIGHTGREEN("- sketch size bound = %d\n"), sketch_size_bound);
        }
        fprintf(stdout, FG_LIGHTGREEN("- %ls = %f, %ls = %f\n"), L"\u03B1", user_alpha, L"\u03B3",
                (1 + alpha) / (1 - alpha));
    }

    struct dds_sketch* sketch = NULL;
    struct dds_sketch* sketch_low = NULL;
    struct dds_sketch* sketch_high = NULL;

    if (store_type == UNIFORM_WITH_INITIAL_ALPHA) {
        store_type = COLLAPSINGALL_MAPSTORE;
        alpha = tanh(atanh(alpha)/pow(2.0,BACKWARD_COLLAPSES-1));
    }
    if (neg_items) {
        sketch = (struct dds_sketch *) dds_gsketch_init(store_type,
                                                        alpha,
                                                        0.0,
                                                        sketch_size_bound);
    } else {
        sketch = (struct dds_sketch *) dds_psketch_init(store_type,
                                                        alpha,
                                                        0.0,
                                                        sketch_size_bound);
    }


    clock_t begin_time = clock();

    for (long i = 0; i < slength; i++) {
        dds_sketch_update(sketch, istream[i], 1);
    }

    clock_t end_time = clock();
    double time_spent = 1000 * (double)(end_time - begin_time) / CLOCKS_PER_SEC;
    double updates_per_ms = (double) slength / time_spent;

    qsort(istream, slength, sizeof(double), compare_doubles);

    double sketch_quantiles[101];
    double exact_quantiles[101];
    bool is_accurate = true;
    time_spent = 0.0;
    for (int p = 0; p <= 100; p++) {
        double q = (double) p / 100.0;
        begin_time = clock();
        sketch_quantiles[p] = dds_sketch_get_quantile(sketch, q, &is_accurate);
        end_time = clock();
        time_spent += 1000 * (double) (end_time - begin_time) / CLOCKS_PER_SEC;
        long r = q * (slength - 1);
        exact_quantiles[p] = istream[r];
    }


    double query_time = (double) time_spent / 100.0;
    if (!csv_output) {
        printf(FG_LIGHTYELLOW("\nTotal counts after sketch is filled = %ld\n"), dds_sketch_get_total_count(sketch));
        print_statistics(user_alpha, sketch_quantiles, exact_quantiles, 5);
        printf(FG_LIGHTYELLOW("\nInitial alpha = %.10f\n"), alpha);
        printf(FG_LIGHTYELLOW("\nFinal alpha = %.10f\n"), dds_sketch_get_id_mapping(sketch)->alpha);
        printf(FG_LIGHTYELLOW("Number of collapses = %d\n"), dds_sketch_get_num_collapses(sketch));
        printf(FG_LIGHTYELLOW("Number of buckets at the end of execution = %d\n"), dds_sketch_get_size(sketch));
        printf(FG_LIGHTYELLOW("Sketch update performance = %.2f/ms\n"), updates_per_ms);
        printf(FG_LIGHTYELLOW("Quantile query mean time = %.4fms\n"), query_time);
    }

    if (csv_output) {
        //filename, stream length, neg_items_bool, store_type, store size bound, store size, user alpha, initial alpha, final alpha, num_collapses,
        // rel_error[0], rel_error[1], ..., avg_rel_error, updates/ms, query time
        printf("%s,%ld,%d,%d,%d,%d,%.10f,%.10f,%.10f,%d,",
                input_filename,
                slength,
                neg_items,
                store_type,
                sketch_size_bound,
                dds_sketch_get_size(sketch),
                user_alpha,
                alpha,
                dds_sketch_get_id_mapping(sketch)->alpha,
                dds_sketch_get_num_collapses(sketch));

        print_statistics_csv(sketch_quantiles, exact_quantiles, 10);

        printf("%f,%f\n", updates_per_ms, query_time);
    }

    if (neg_items) dds_gsketch_destroy(sketch);
    else dds_psketch_destroy(sketch);
    free(istream);
    return 0;
}
