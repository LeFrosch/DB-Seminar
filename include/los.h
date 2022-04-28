#ifndef LOS_H
#define LOS_H

#include "pointer.h"

// The list of skips
struct los {
    // Pointer with version to the start of the free list in the nodes array
    version_ptr free_list;

    // Pointer with version to the start of the skip list in the nodes array
    version_ptr skip_list;

    // The preallocate nodes
    struct node* nodes;
};

// Allocates and initializes the list of skips
struct los* create_los(uint8_t threads, uint32_t reservoir_size);

// Deallocates the list of skips
void free_los(struct los* los);

// Acquires a new skip from the los, the own node can be 0/null
uint8_t acquire(struct los* los, uint8_t* own, uint32_t reservoir_size);

// Retrieves the node from the los, the index should not be 0/null
struct node* get_node(struct los* los, uint8_t index);

#endif
