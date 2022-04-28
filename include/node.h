#ifndef NODE_DART_H
#define NODE_DART_H

#include <stdint.h>

#include "pointer.h"

// Contains information about a skip
struct node {
    float w_value;
    uint32_t index;
    uint64_t length;

    // The index of the successor node, 0 is considered null and therefor not
    // a valid index
    uint8_t successor;
};

// Generate the initial skip and initializes the random number generator
void generate_initial_skip(struct node* node, uint32_t reservoir_size);

// Generates the next skip depending on the preceding skip
void generate_next_skip(struct node* preceding_node, struct node* node, uint32_t reservoir_size);

#endif
