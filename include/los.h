#ifndef LOS_H
#define LOS_H

#include "pointer.h"

// The list of skips
struct los {
    // Pointer to the free list
    node_ptr free_list;

    // Pointer to the skip list
    node_ptr skip_list;

    // The preallocate nodes
    struct node* nodes;
};

// Allocates and initializes the list of skips
struct los* create_los(uint8_t threads, uint32_t reservoir_size);

// Deallocates the list of skips
void free_los(struct los* los);

#endif
