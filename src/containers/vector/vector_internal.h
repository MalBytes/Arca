#ifndef ARCA_VECTOR_INTERNAL_H
#define ARCA_VECTOR_INTERNAL_H


#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "sort.h"


#define VECTOR_MIN_CAPACITY 8
#define VECTOR_GROWTH_FACTOR 2
#define VECTOR_SHRINK_THRESHOLD 4
#define VECTOR_SHRINK_FACTOR 2


struct Vector {
    void *data;
    size_t size;
    bool is_sorted; 
    size_t capacity;
    size_t elem_size;
};


// Helper Prototypes
static bool vector_grow_helper(ArcaVector *vector);
static void vector_shrink_helper(ArcaVector *vector);


#endif // ARCA_VECTOR_INTERNAL_H