#ifndef ARCA_H
#define ARCA_H


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


# define ARCA_INDEX_NOT_FOUND SIZE_MAX


typedef enum {
    ARCA_SUCCESS = 0,
    ARCA_NULL_POINTER,
    ARCA_OUT_OF_MEMORY,
    ARCA_OUT_OF_BOUNDS,
    ARCA_KEY_NOT_FOUND,
    ARCA_EMPTY_CONTAINER,
    ARCA_INVALID_ARGUMENT,

    ARCA_UNKNOWN_ERROR,
} ArcaStatus;


typedef bool (*ArcaEquals)(const void *lhs, const void *rhs);
typedef int  (*ArcaCompare)(const void *lhs, const void *rhs);


#endif // ARCA_H