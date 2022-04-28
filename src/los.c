#include <malloc.h>

#include "los.h"
#include "node.h"

struct los* create_los(uint8_t threads, uint32_t reservoir_size) {
    struct los* los = (struct los*) malloc(sizeof(struct los));
    if (!los) return NULL;

    // Preallocate list of nodes, one for every thread, one for the upcoming
    // node and one for the null pointer/index
    los->nodes = calloc(threads + 2, sizeof(struct node));
    if (!los->nodes) return NULL;

    // Initially all nodes except one are in the free list, therefore the free
    // list starts at index 2 and the skip list at index 1
    los->free_list = 2;
    los->skip_list = 1;

    // Set the successor indices for all nodes in the free list, the skip list
    // contains only one node
    for (uint8_t i = 2; i < threads; i++) {
        los->nodes[i].successor = i + 1;
    }

    generate_initial_skip(los->nodes + GET_INDEX(los->skip_list), reservoir_size);

    return los;
}

void free_los(struct los* los) {
    free(los->nodes);
    free(los);
}