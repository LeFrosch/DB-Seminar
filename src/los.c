#include <malloc.h>
#include <assert.h>
#include <pthread.h>

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
    los->free_list = 2l << 32;
    los->skip_list = 1l << 32;

    // Set the successor indices for all nodes in the free list, the skip list
    // contains only one node
    for (uint8_t i = 2; i < threads + 1; i++) {
        los->nodes[i].successor = i + 1;
    }

    generate_initial_skip(los->nodes + GET_INDEX(los->skip_list), reservoir_size);

    return los;
}

void free_los(struct los* los) {
    free(los->nodes);
    free(los);
}

// Tries to get or generate the successor for a node from the skip list. If the
// successor could not be acquired this returns null. But in both cases the own
// pointer content could be modified.
uint8_t get_successor(struct  los* los, struct node* node, uint8_t* own, uint32_t reservoir_size) {
    if (node->successor) {
        // If the threads owns a node it needs to be freed
        if (*own) {
            struct node* own_node = los->nodes + *own;

            // Copy the pointer to detect writes by other threads
            version_ptr free_list = los->free_list;
            own_node->successor = GET_INDEX(free_list);

            if (!update_ptr(&los->free_list, free_list, *own)) {
                return NULL_PTR;
            }

            *own = NULL_PTR;
        }

        return node->successor;
    } else {
        // The skip node has no successor, the thread needs to generate a one if
        // the thread owns a node we can just use this, otherwise a new node
        // needs to be allocated
        uint8_t successor = 0;

        if (*own) {
            successor = *own;
        } else {
            // Copy the pointer to detect writes by other threads
            version_ptr free_list = los->free_list;
            successor = GET_INDEX(free_list);

            struct node* successor_node = los->nodes + successor;
            if (!update_ptr(&los->free_list, free_list, successor_node->successor)) {
                return NULL_PTR;
            }

            *own = successor;
            successor_node->successor = NULL_PTR;
        }

        assert(successor);
        generate_next_skip(node, los->nodes + successor, reservoir_size);

        return successor;
    }
}

// Tries to acquire a node. If no node could be acquired this returns null. In
// both cases the own pointer could be modified.
uint8_t try_acquire(struct los* los, uint8_t* own, uint32_t reservoir_size) {
    // Copy the pointer to detect writes by other threads
    version_ptr skip_list = los->skip_list;
    struct node* skip_node = los->nodes + GET_INDEX(skip_list);

    uint8_t successor = get_successor(los, skip_node, own, reservoir_size);
    if (!successor) {
        return NULL_PTR;
    }

    if (!update_ptr(&los->skip_list, skip_list, successor)) {
        return NULL_PTR;
    }

    return GET_INDEX(skip_list);
}

uint8_t acquire(struct los* los, uint8_t own, uint32_t reservoir_size) {
    uint8_t result;
    while (!(result = try_acquire(los, &own, reservoir_size)));

    return result;
}

struct node* get_node(struct los* los, uint8_t index) {
    assert(index);

    return los->nodes + index;
}