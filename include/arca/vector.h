#ifndef ARCA_VECTOR_H
#define ARCA_VECTOR_H

#include "arca.h"
#include <stdint.h>
#include <stdbool.h>

#define arca_vector_create(type, capacity) \
    arca_vector_new(sizeof(type), (capacity))

typedef struct Vector ArcaVector;


// Standard Vector Operations (Standalone Functions)
void arca_vector_free(ArcaVector *vector);
ArcaVector *arca_vector_copy(ArcaVector *vector);
ArcaVector *arca_vector_new(size_t element_size, size_t initial_capacity);

size_t arca_vector_len(ArcaVector *vector);
ArcaStatus arca_vector_pop(ArcaVector *vector, void *out);
ArcaStatus arca_vector_push(ArcaVector *vector, const void *element);
ArcaStatus arca_vector_get(ArcaVector *vector, size_t index, void *out);
ArcaStatus arca_vector_set(ArcaVector *vector, size_t index, const void *element);

ArcaStatus arca_vector_reverse(ArcaVector *vector);
ArcaStatus arca_vector_to_array(ArcaVector *vector, size_t *out_size, void **out_array);

// User assisted vector operations
ArcaStatus arca_vector_sort(ArcaVector *vector, ArcaCompare cmp);
size_t arca_vector_find_elem(ArcaVector *vector, const void *element, ArcaEquals cmp);

#endif // ARCA_VECTOR_H