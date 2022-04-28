#include <malloc.h>
#include <assert.h>

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

uint8_t acquire(struct los* los, uint8_t own, uint32_t reservoir_size) {
    // Copy the skip_list pointer to prevent overwrites by other threads
    version_ptr skip_list = los->skip_list;
    struct node* skip_node = los->nodes + GET_INDEX(skip_list);

    // Check whether the current skip_node has a successor
    uint8_t successor = 0;
    if (skip_node->successor) {
        // If the skip has successor just use this one
        successor = skip_node->successor;

        // But if the threads owns a node it needs to be freed
        if (own) {
            struct node* own_node = los->nodes + own;

            // Copy the pointer to detect writes by other threads
            version_ptr free_list = los->free_list;
            own_node->successor = GET_INDEX(free_list);

            if (!update_ptr(&los->free_list, free_list, own)) {
                return NULL_PTR;
            }
        }
    } else {
        // The skip node has no successor, the thread needs to generate a one if
        // the thread owns a node we can just use this, otherwise a new node
        // needs to be allocated
        if (own) {
            successor = own;
        } else {
            // Copy the pointer to detect writes by other threads
            version_ptr free_list = los->free_list;
            successor = GET_INDEX(free_list);
            struct node* successor_node = los->nodes + successor;

            assert(successor_node->successor);
            if (!update_ptr(&los->free_list, free_list, successor_node->successor)) {
                return NULL_PTR;
            }
        }

        generate_next_skip(skip_node, los->nodes + successor, reservoir_size);
    }

    assert(successor);
    if (!update_ptr(&los->skip_list, skip_list, successor)) {
        return NULL_PTR;
    }

    return GET_INDEX(skip_list);
}

struct node* get_node(struct los* los, uint8_t index) {
    assert(index);

    return los->nodes + index;
}