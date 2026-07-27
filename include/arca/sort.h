#ifndef ARCA_SORT_H
#define ARCA_SORT_H

#include "arca.h"


ArcaStatus arca_heapsort(void *base, size_t count, size_t elem_size, ArcaCompare cmp);
ArcaStatus arca_introsort(void *base, size_t count, size_t elem_size, ArcaCompare cmp);
ArcaStatus arca_insertion_sort(void *base, size_t count, size_t elem_size, ArcaCompare cmp);


#endif // ARCA_SORT_H