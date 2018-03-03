/*
Benjamin A. Slack
benjamin.a.slack@wmich.edu
CS6260
Assignment #2
Solving linear systems with
Back Substitution.
03.02.2018

Description:
Implements a parallel solution to 
solving systems of equations using
a triangular matrix and back sub-
stitution.
*/

#define _XOPEN_SOURCE 1

#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

#define kDEBUG false

// globals
int n = 0;
int t = 0;

#define ACCESS2D(A, i, j, n) (A)[(i) * (n) + (j)]

void dump(double A[], double b[], double x[], int n) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fprintf(stderr, "%0.3f ", ACCESS2D(A, i, j, n));
        }
        fprintf(stderr, " - %0.3f", x[i]);
        fprintf(stderr, " - %0.3f\n", b[i]);
    }
    fprintf(stderr, "\n");
}

void triangulate(double A[], double b[], double x[], int n) {
    // for each row
    int i = 0;
    int j = 0;
    int k = 0;
    double multiple = 0.0;
    {
        for (i = 0; i < n; i++) {
            // for each subsequent row
            for (j = i + 1; j < n; j++) {
                // calculate the multiple required
                multiple = ACCESS2D(A, j, i, n) / ACCESS2D(A, i, i, n);
                b[j] = b[j] - b[i] * multiple;
                // eliminate the component for the col
#pragma omp parallel for num_threads(t)\
    schedule(runtime)\
    default(none)\
    shared(A, b, x, n, i, j, multiple)\
    private(k)
                for (k = i; k < n; k++) {
                    ACCESS2D(A, j, k, n) = ACCESS2D(A, j, k, n) - ACCESS2D(A, i, k, n) * multiple;
                }

            }
        }
    }
} // end triangulate

void backsub(double A[], double b[], double x[], int n){
    // starting from the last row in A
    // calculate x of that row from A and B
    int i = 0;
    int j = 0;
    double temp;

    for (i = (n-1); i >= 0; i--){
        temp = 0.0;
#pragma omp parallel for num_threads(t)\
        schedule(runtime)\
        default(none)\
        shared(A, b, x, n, i, stderr)\
        private(j)\
        reduction(+:temp)
        for ( j = (n-1); j >= i; j-- ){
            temp += (i == (n-1)) ? 0 : x[j+1] * ACCESS2D(A, i, j+1, n);
        }
        x[i] = (b[i]-temp)/ACCESS2D(A, i, i, n);
    }
}

void Init(double A[], double b[], double x[], int n) {
    int i, j;
    for (i = 0; i < n; i++) x[i] = 1.0;
    // srandom(1);
    char *state_buf = NULL;
    size_t state_len = BUFSIZ >> 3;
    state_buf = (char *) calloc(state_len, sizeof(char));
    struct random_data *dat = NULL;
    dat = (struct random_data *) calloc(1, sizeof(struct random_data));
    int32_t result = 0;
    unsigned int seed = 1;
    initstate_r(seed, state_buf, state_len, dat);

#pragma omp parallel num_threads(t)\
    default(none)\
    shared(A, n, x, b, stderr, state_buf, state_len, dat, seed)\
    private(i, j, result)
    for (i = 0; i < n; i++) {
#pragma omp for schedule(runtime)
        for (j = 0; j < n; j++) {
            if (i != j) {
                random_r(dat, &result);
                A[i * n + j] = result / ((double) RAND_MAX);
            } else {
                A[i * n + i] = n / 10.0;
            }
            if (kDEBUG) {
                fprintf(stderr, "thread: %d, A[%d,%d] = %0.6f\n", omp_get_thread_num(), i, j, A[i * n + j]);
            }
        } // iterated parallel for ends
    } //first for ends, first parallel block ends


    double temp;
    for (i = 0; i < n; i++) {
        temp = 0.0;
#pragma omp parallel for num_threads(t)\
        schedule(runtime)\
        default(none)\
        shared(A,b,x,n,i)\
        private(j)\
        reduction(+:temp)
        for (j = 0; j < n; j++) {
            temp += (ACCESS2D(A, i, j, n) * x[j]);
        }
        b[i] = temp;
    }

    free(state_buf);
    free(dat);
    memset(x, 0, n * sizeof(double));
}

int main(int argc, char **argv, char **environ) {
    int op;
    bool bVerbose = false;

    while ((op = getopt(argc, argv, ":vd:g:s:n:t:")) != -1) {
        switch ((char) op) {
            case 'v':
                bVerbose = true;
                break;
            case 'n':
                n = atoi(optarg);
                if (n < 1) {
                    fprintf(stderr, "size of n invalid = %d\n", n);
                }
                break;
            case 't':
                t = atoi(optarg);
                if (t < 1) {
                    fprintf(stderr, "number of threads invalid = %d\n", t);
                }
                break;
            case 'd':
                omp_set_schedule(omp_sched_dynamic, atoi(optarg));
                break;
            case 'g':
                omp_set_schedule(omp_sched_guided, atoi(optarg));
                break;
            case 's':
                omp_set_schedule(omp_sched_static, atoi(optarg));
                break;
            case '?':
                fprintf(stderr, "unknown option \n");
                break;
            case ':':
                fprintf(stderr, "missing argument \n");
                break;
            default:
                fprintf(stderr, "unknown %c\n", (char) op);
                break;
        }
    }
    
    double *A = (double *) calloc(n * n, sizeof(double));
    double *b = (double *) calloc(n, sizeof(double));
    double *x = (double *) calloc(n, sizeof(double));

    Init(A, b, x, n);

    if (bVerbose){
        fprintf(stdout, "Initial Matrix\n");
        dump(A, b, x, n);
    }

    struct timeval s_tv;
    struct timeval e_tv;

    gettimeofday(&s_tv, NULL);
    triangulate(A, b, x, n);
    gettimeofday(&e_tv, NULL);

    struct timeval t_tv;
    timersub(&e_tv, &s_tv, &t_tv);

    if (bVerbose){
        fprintf(stdout, "Triangular Matrix\n");
        dump(A, b, x, n);
    }

    gettimeofday(&s_tv, NULL);
    backsub(A, b, x, n);
    gettimeofday(&e_tv, NULL);

    struct timeval b_tv;
    timersub(&e_tv, &s_tv, &b_tv);

    if (bVerbose){
        fprintf(stdout, "Backwards Substitution\n");
        dump(A, b, x, n);
    }

    fprintf(stdout, "triangulate(sec): %d.%06d, ",\
        (unsigned int)(t_tv.tv_sec),\
        (unsigned int)(t_tv.tv_usec));

    fprintf(stdout, "backsub(sec): %d.%06d\n",\
        (unsigned int)(b_tv.tv_sec),\
        (unsigned int)(b_tv.tv_usec));

    free(A);
    free(b);
    free(x);

    exit(EXIT_SUCCESS);
}