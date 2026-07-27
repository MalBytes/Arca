#ifndef ARCA_SORT_INTERNAL_H
#define ARCA_SORT_INTERNAL_H


#include "sort.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>


#define SORT_EXIT_THRESHOLD 20


static inline void swap_helper(void *a, void *b, size_t size, void *temp);
static inline void sort3_helper(void *a, void *b, void *c, ArcaCompare cmp, size_t size, void *temp);
static void heapsort_helper(char *start, size_t count, size_t elem_size, ArcaCompare cmp, void *temp);
static void build_heap_helper(char *start, size_t count, size_t elem_size, ArcaCompare cmp, void *temp);
static void insertion_sort_helper(char *start, char *end, size_t elem_size, ArcaCompare cmp, void *temp);
static void introsort_helper(char *start, char *end, size_t elem_size, ArcaCompare cmp, size_t depth, void *temp);
static void heap_sift_down_helper(char *start, size_t count, size_t root_index, size_t elem_size, ArcaCompare cmp, void *temp);


#endif // ARCA_SORT_INTERNAL_H