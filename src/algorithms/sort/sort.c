#include "sort_internal.h"
#include <stdlib.h>


ArcaStatus arca_heapsort(void *base, size_t count, size_t elem_size, ArcaCompare cmp) {
    if (base == NULL || cmp == NULL || elem_size == 0) {
        return ARCA_INVALID_ARGUMENT;
    }

    if (count <= 1) {
        return ARCA_SUCCESS;
    }

    char buffer[256];
    bool heap_allocated = elem_size > sizeof(buffer);

    void *temp = heap_allocated ? malloc(elem_size) : buffer;

    if (temp == NULL) {
        return ARCA_OUT_OF_MEMORY;
    }

    heapsort_helper(base, count, elem_size, cmp, temp);

    if (heap_allocated) {
        free(temp);
    }

    return ARCA_SUCCESS;
}

ArcaStatus arca_introsort(void *base, size_t count, size_t elem_size, ArcaCompare cmp) {
    if (base == NULL || cmp == NULL || elem_size == 0) {
        return ARCA_INVALID_ARGUMENT; // Invalid input
    }

    if (count <= 1) {
        return ARCA_SUCCESS; // No need to sort
    }

    char buffer[256];
    bool heap_allocated = elem_size > sizeof(buffer);
    void *temp = heap_allocated ? malloc(elem_size) : buffer;

    if (temp == NULL) {
        return ARCA_OUT_OF_MEMORY; // Return if allocation fails
    }
    
    size_t depth = 2 * (size_t)floor(log2(count));
    char *start = (char *)base;
    char *end = start + (count * elem_size);
    introsort_helper(start, end, elem_size, cmp, depth, temp);

    /* Use insertion sort for the remaining small partailly sorted subarrays */
    insertion_sort_helper(start, end, elem_size, cmp, temp);

    if (heap_allocated) {
        free(temp);
    }

    return ARCA_SUCCESS;
}

ArcaStatus arca_insertion_sort(void *base, size_t count, size_t elem_size, ArcaCompare cmp) {
    if (base == NULL || cmp == NULL || elem_size == 0) {
        return ARCA_INVALID_ARGUMENT;
    }

    if (count <= 1) {
        return ARCA_SUCCESS;
    }

    char buffer[256];
    bool heap_allocated = elem_size > sizeof(buffer);

    void *temp = heap_allocated ? malloc(elem_size) : buffer;

    if (temp == NULL) {
        return ARCA_OUT_OF_MEMORY;
    }

    char *start = (char *)base;
    char *end = start + (count * elem_size);
    insertion_sort_helper(start, end, elem_size, cmp, temp);
    

    if (heap_allocated) {
        free(temp);
    }

    return ARCA_SUCCESS;
}


/* Helper Functions */
static void insertion_sort_helper(char *start, char *end, size_t elem_size, ArcaCompare cmp, void *temp) {
    char *i = start + elem_size;
    while (i < end) {
        memcpy(temp, i, elem_size);
        char *j = i;

        while (j > start && cmp(temp, j - elem_size) < 0) {
            memcpy(j, j - elem_size, elem_size); 
            j -= elem_size; 
        }

        memcpy(j, temp, elem_size);
        i += elem_size;
    }
}

static void introsort_helper(char *start, char *end, size_t elem_size, ArcaCompare cmp, size_t depth, void *temp) {
    size_t count;
    
    while (true) {    
        count = (end - start) / elem_size;
        if (count <= SORT_EXIT_THRESHOLD) {
            return;
        }
        
        /* Use Heapsort if depth limit reached */
        if (depth == 0) {
            heapsort_helper(start, count, elem_size, cmp, temp);
            return;
        }

        /* Use quicksort if depth is not zero */
        
        /* Pivot selection using median-of-three */
        char *last = end - elem_size;
        char *middle = start + (count / 2) * elem_size;
        sort3_helper(start, middle, last, cmp, elem_size, temp);

        /* Move pivot to the (end - 1)th position so it doesnt interfere with the partitioning comparison */
        swap_helper(middle, last - elem_size, elem_size, temp);
        char *pivot = last - elem_size;
        
        /* Hoare's partitioning with tail recursion elimination */
    
        // initialize 2 pointers for the partitioning
        char *i = start + elem_size, *j = last - 2 * elem_size;

        // partitioning loop
        while (true) {
            // move the pointers until we find elements that are on the wrong side of the pivot
            while (cmp(i, pivot) < 0) i += elem_size;
            while (cmp(j, pivot) > 0) j -= elem_size;

            // if the pointers have crossed, we are done partitioning
            if (i >= j) break;
            
            // swap the elements at the pointers
            swap_helper(i, j, elem_size, temp);
            
            // move the pointers to the next elements
            i += elem_size;
            j -= elem_size;
        }
        
        /* Move the pivot to its final position */
        swap_helper(i, pivot, elem_size, temp); 
        depth--; // Decrease depth for the next recursive call
        
        if ((i - start) < (end - (i + elem_size))) {
            introsort_helper(start, i, elem_size, cmp, depth, temp);
            start = i + elem_size; // Move start to the next element after the pivot  
        } else {
            introsort_helper(i + elem_size, end, elem_size, cmp, depth, temp);
            end = i; // Move end to the the pivot
        } 
    }
}

static void heapsort_helper(char *start, size_t count, size_t elem_size, ArcaCompare cmp, void *temp) {
    if (count <= 1) {
        return; // No need to sort
    }

    /* Build a max heap from the input data */
    build_heap_helper(start, count, elem_size, cmp, temp);

    /* One by one extract elements from the heap */
    
    for (size_t i = count - 1; i > 0; --i) {
        // move the current root (maximum) to the end of the array
        swap_helper(start, start + i * elem_size, elem_size, temp);
        // restore the heap property on the reduced heap
        heap_sift_down_helper(start, i, 0, elem_size, cmp, temp);
    }
}

static void build_heap_helper(char *start, size_t count, size_t elem_size, ArcaCompare cmp, void *temp) {
    if (count <= 1) {
        return; // No need to build heap
    }
    
    size_t i = (count / 2);
    while (i > 0) {
        --i;
        heap_sift_down_helper(start, count, i, elem_size, cmp, temp);
    }
}

static void heap_sift_down_helper(char *start, size_t count, size_t root_index, size_t elem_size, ArcaCompare cmp, void *temp) {
    /* Copy the root element to a temporary location */
    char *root = start + root_index * elem_size;
    memcpy(temp, root, elem_size);
    
    while (true) {
        /* Compute the indices of the left and right children of the root */
        size_t left_index  = 2 * root_index + 1, right_index = 2 * root_index + 2, largest_index = root_index;
        char *left = start + left_index * elem_size, *right = start + right_index * elem_size, *largest = temp;

        if (left_index < count && cmp(left, temp) > 0) {
            largest_index = left_index;
            largest = left;
        }

        if (right_index < count && cmp(right, largest) > 0) {
            largest_index = right_index;
            largest = right;
        }

        if (largest_index == root_index) {
            break;
        }

        memcpy(root, largest, elem_size);
        root = largest;
        root_index = largest_index; // Move down the heap
    }

    memcpy(root, temp, elem_size);
}

static inline void swap_helper(void *a, void *b, size_t size, void *temp) {
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

static inline void sort3_helper(void *a, void *b, void *c, ArcaCompare cmp, size_t size, void *temp) {
    if (cmp(a, b) > 0) swap_helper(a, b, size, temp);
    if (cmp(b, c) > 0) swap_helper(b, c, size, temp);
    if (cmp(a, b) > 0) swap_helper(a, b, size, temp);
}