#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"


void print_vector(ArcaVector *vec) {
    if (vec == NULL) {
        printf("Vec: {}\n");
        return;
    }

    printf("Vec: {");
    for (size_t i = 0; i < arca_vector_len(vec); ++i) {
        int value;
        if (arca_vector_get(vec, i, &value) == ARCA_SUCCESS) {
            printf("%d, ", value);
        } else {
            fprintf(stderr, "Failed to get element %zu\n", i);
        }
    }
    printf("\b\b}\n");
}

int cmp_int(const void *a, const void *b) {
    int int_a = *(const int *)a;
    int int_b = *(const int *)b;

    if (int_a > int_b) return 1;
    if (int_a < int_b) return -1;
    return 0;
}

int cmp_rev_int(const void *a, const void *b) {
    return -cmp_int(a, b);
}

int randi(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

bool eq_int(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b;
}


int main(void) {
    srand((unsigned)time(NULL));

    double arca_total = 0.0;
    double qsort_total = 0.0;

    for (int test = 0; test < 10; ++test) {

        size_t num_elements = randi(100000, 500000);

        int *original = malloc(num_elements * sizeof(int));
        int *qsort_arr = malloc(num_elements * sizeof(int));

        ArcaVector *vec = arca_vector_new(sizeof(int), num_elements);

        if (!original || !qsort_arr || !vec) {
            fprintf(stderr, "Allocation failed.\n");
            return 1;
        }

        /* Generate identical input */
        for (size_t i = 0; i < num_elements; ++i) {
            original[i] = rand();

            arca_vector_push(vec, &original[i]);
            qsort_arr[i] = original[i];
        }

        /* Benchmark Arca */
        clock_t start = clock();
        arca_vector_sort(vec, cmp_int);
        clock_t end = clock();

        double arca_ms =
            (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

        /* Benchmark qsort */
        start = clock();

        qsort(
            qsort_arr,
            num_elements,
            sizeof(int),
            cmp_int
        );

        end = clock();

        double qsort_ms =
            (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

        arca_total += arca_ms;
        qsort_total += qsort_ms;

        /* Verify correctness */
        for (size_t i = 0; i < num_elements; ++i) {
            int value;
            arca_vector_get(vec, i, &value);

            if (value != qsort_arr[i]) {
                printf("Mismatch at index %zu\n", i);
                exit(EXIT_FAILURE);
            }
        }

        printf(
            "Test %2d | Size: %-7zu | Arca: %8.3f ms | qsort: %8.3f ms | %.2fx\n",
            test + 1,
            num_elements,
            arca_ms,
            qsort_ms,
            qsort_ms / arca_ms
        );

        free(original);
        free(qsort_arr);
        arca_vector_free(vec);
    }

    printf("\n===============================\n");
    printf("Average Arca : %.3f ms\n", arca_total / 10.0);
    printf("Average qsort: %.3f ms\n", qsort_total / 10.0);
    printf("Speedup      : %.2fx\n",
           qsort_total / arca_total);

    return 0;
}