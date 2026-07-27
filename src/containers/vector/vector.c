#include "vector_internal.h"


ArcaVector *arca_vector_new(size_t element_size, size_t initial_capacity) {
    if (element_size == 0) {
        return NULL; // Invalid element size
    }

    if (initial_capacity < VECTOR_MIN_CAPACITY) {
        initial_capacity = VECTOR_MIN_CAPACITY;
    }

    void *arr = calloc(initial_capacity, element_size);
    if (arr == NULL) {
        return NULL; // Return NULL if allocation fails
    }

    ArcaVector *vector = malloc(sizeof(ArcaVector));
    if (vector == NULL) {
        free(arr);
        return NULL; // Return NULL if allocation fails
    }

    vector->data = arr;
    vector->size = 0;
    vector->capacity = initial_capacity;
    vector->elem_size = element_size;
    
    return vector;
}

ArcaVector *arca_vector_copy(ArcaVector *vector) {
    if (vector == NULL) {
        return NULL;
    }

    ArcaVector *new_vector = arca_vector_new(vector->elem_size, vector->capacity);
    if (new_vector == NULL) {
        return NULL; // Return NULL if allocation fails
    }

    if (vector->size > 0) {
        memcpy(new_vector->data, vector->data, vector->size * vector->elem_size);
    }
    new_vector->size = vector->size;

    return new_vector;
}

void arca_vector_free(ArcaVector *vector){
    if (vector == NULL) {
        return; // Nothing to free
    }

    free(vector->data);
    free(vector);
}

size_t arca_vector_len(ArcaVector *vector) {
    return vector->size;    
}

ArcaStatus arca_vector_push(ArcaVector *vector, const void *element) {
    /* 
     * Don't care for integer overflow here, as it is unlikely to happen in practice.
     * Plus overflow checks cost 2 extra division operations which is a noticible overhead 
     * for a function that is called very frequently.
    */

    if (vector->size >= vector->capacity) {
        if (!vector_grow_helper(vector)) {
            return ARCA_OUT_OF_MEMORY; // Allocation failed
        }
    }

    void *dest = (vector->size * vector->elem_size) + (char *)vector->data;
    memcpy(dest, element, vector->elem_size);
    vector->size++;

    return ARCA_SUCCESS;
}

ArcaStatus arca_vector_pop(ArcaVector *vector, void *out) {
    if (vector == NULL || out == NULL) {
        return ARCA_INVALID_ARGUMENT; // Invalid input
    }
    
    if (vector->size <= 0) {
        return ARCA_EMPTY_CONTAINER; // Vector is empty
    }

    vector->size--;
    void *src = (vector->size * vector->elem_size) + (char *)vector->data;
    memcpy(out, src, vector->elem_size);
    memset(src, 0, vector->elem_size); // Clear the popped element

    if (vector->size <= vector->capacity / VECTOR_SHRINK_THRESHOLD && vector->capacity > VECTOR_MIN_CAPACITY) {
        vector_shrink_helper(vector);
    }

    return ARCA_SUCCESS;
}

ArcaStatus arca_vector_get(ArcaVector *vector, size_t index, void *out){
    if (vector == NULL || out == NULL) {
        return ARCA_INVALID_ARGUMENT; // Invalid input
    }

    if (index >= vector->size) {
        return ARCA_OUT_OF_BOUNDS; // Index out of bounds
    }

    void *src = (char *)vector->data + (index * vector->elem_size);
    memcpy(out, src, vector->elem_size);
    
    return ARCA_SUCCESS;
}

ArcaStatus arca_vector_set(ArcaVector *vector, size_t index, const void *element){
    if (vector == NULL || element == NULL) {
        return ARCA_INVALID_ARGUMENT; // Invalid input
    }

    if (index >= vector->size) {
        return ARCA_OUT_OF_BOUNDS; // Index out of bounds
    }

    void *dest = (char *)vector->data + (index * vector->elem_size);
    memcpy(dest, element, vector->elem_size);

    return ARCA_SUCCESS;
}


ArcaStatus arca_vector_reverse(ArcaVector *vector) {
    if (vector == NULL) {
        return ARCA_INVALID_ARGUMENT; // Invalid input
    }

    if (vector->size <= 1) {
        return ARCA_SUCCESS;
    }
    size_t elem_size = vector->elem_size;
    char *start, *end;
    start = vector->data;
    end = (char *)start + (vector->size - 1) * elem_size;
    void *temp = malloc(elem_size);

    if (temp == NULL) {
        return ARCA_OUT_OF_MEMORY; // Return if allocation fails
    }

    while (start < end) {
        memcpy(temp, start, elem_size);
        memcpy(start, end, elem_size);
        memcpy(end, temp, elem_size);
        start += elem_size;
        end -= elem_size;
    }

    free(temp);
    return ARCA_SUCCESS;
}

ArcaStatus arca_vector_to_array(ArcaVector *vector, size_t *out_size, void **out_array) {
    if (vector == NULL || out_size == NULL || out_array == NULL) {
        if (out_size) {
            *out_size = 0;
        }
        return ARCA_INVALID_ARGUMENT; // Return error for invalid input
    }

    void *arr = malloc(vector->size * vector->elem_size);
    if (arr == NULL) {
        return ARCA_OUT_OF_MEMORY; // Return error if allocation fails
    }

    memcpy(arr, vector->data, vector->size * vector->elem_size);
    *out_size = vector->size;
    *out_array = arr;

    return ARCA_SUCCESS;
}


// User assisted vector operations
ArcaStatus arca_vector_sort(ArcaVector *vector, ArcaCompare cmp) {
    return arca_introsort(vector->data, vector->size, vector->elem_size, cmp);
}

size_t arca_vector_find_elem(ArcaVector *vector, const void *element, ArcaEquals cmp) {
    if (vector == NULL || element == NULL || cmp == NULL || vector->size == 0) {
        return ARCA_INDEX_NOT_FOUND; // Invalid input or empty vector
    }

    char *current = (char *) vector->data;
    for (size_t i = 0; i < vector->size; ++i) {
        if (cmp(element, current)) {
            return i; // Element found
        }
        current = current + vector->elem_size;
    }

    return ARCA_INDEX_NOT_FOUND; // Not found
}


// Helper Fucntions
static bool vector_grow_helper(ArcaVector *vector) {
    void *arr = realloc(vector->data, vector->elem_size * vector->capacity * VECTOR_GROWTH_FACTOR);
    if (!arr) {
        return false; // Allocation failed
    }
    vector->data = arr;
    vector->capacity *= VECTOR_GROWTH_FACTOR;        

    return true;
}

static void vector_shrink_helper(ArcaVector *vector) {
    size_t new_cap = vector->capacity / VECTOR_SHRINK_FACTOR;
    if (new_cap < VECTOR_MIN_CAPACITY) {
        new_cap = VECTOR_MIN_CAPACITY;
    }

    void *arr = realloc(vector->data, vector->elem_size * new_cap);
    if (arr) {
        vector->data = arr;
        vector->capacity = new_cap;
    }
}
