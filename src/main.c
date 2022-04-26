#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define THREADS 1
#define RESERVOIR_SIZE 10

// Fragen:
// - Wie wird ein "Null" index repräsentiert
// - NOP für das einfügen

// pointer in the preallocated node array with version
struct head_ptr {
    uint32_t version;
    // or uint8_t (also no difference in struct size)
    uint32_t index;
};

// contains information about the skip
struct skip_node {
    uint64_t length;
    float w_value;
    uint32_t index;
    // uint32_t vs. ptr (but wouldn't make a difference regarding the struct size)
    struct skip_node* successor;
};

// represents a tuple in the reservoir
struct reservoir_tuple {
    uint32_t skip_index;
    void* tuple;
};

// returns a random float between 0 and 1
float random_float() { return rand() / ((float) RAND_MAX); }

// generates the next skip node for a preceeding node
void generate_skip(struct skip_node* preceeding_node, struct skip_node* node) {
    float w = preceeding_node->w_value * expf(logf(random_float()) / RESERVOIR_SIZE);

    node->w_value = w;
    node->index = preceeding_node->index + 1;
    node->length = floorf(logf(random_float()) / logf(1 - w));
}

void write_tuple(void* tuple, uint32_t skip_index) {
    // acquire the lock for the tuple

    // check if the skip_index is greater then the previous skip_index

    // write the tuple

    // release the lock
}

int thread_entry(struct head_ptr* free_list, struct head_ptr* skip_list) {
    struct skip_node* skip;

    // acquire skip

    for (;;) {
        // insert the tuple

        if (--skip->length <= 0) {
            // insert tuple into reservoir

            // acquire new skip
        }
    }
}

int main() {
    // initialize the random number generator
    srand(42);

    // prealloacted list of nodes, one for every thread and one for the upcoming node
    struct skip_node* nodes = calloc(THREADS + 1, sizeof(struct skip_node));

    // list of skips that are to be distributed
    struct head_ptr skip_list = {.version = 0, .index = 0};

    // list of currently unused nodes
    struct head_ptr free_list = {.version = 0, .index = 1};

    // initialize all successor pointers
    for (size_t i = 0; i < THREADS; i++) {
        nodes[i].successor = nodes + i + 1;
    }

    // creat the inital skip node
    struct skip_node inital = nodes[skip_list.index];

    float w = expf(logf(random_float())) / RESERVOIR_SIZE;

    inital.w_value = w;
    inital.index = 0;
    inital.length = floorf(logf(random_float()) / logf(1 - w));
    inital.successor = NULL;

    // clean up
    free(nodes);
}
