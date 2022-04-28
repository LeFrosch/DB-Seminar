#include <math.h>
#include <stdlib.h>

#include "node.h"

// returns a random float between 0 and 1
float random_float() { return rand() / ((float) RAND_MAX); }

void generate_initial_skip(struct node* node, uint32_t reservoir_size) {
    srand(42);

    float w = expf(logf(random_float())) / reservoir_size;

    node->w_value = w;
    node->index = 0;
    node->length = floorf(logf(random_float()) / logf(1 - w));
}

void generate_next_skip(struct node* preceding_node, struct node* node, uint32_t reservoir_size) {
    float w = preceding_node->w_value * expf(logf(random_float()) / reservoir_size);

    node->w_value = w;
    node->index = preceding_node->index + 1;
    node->length = floorf(logf(random_float()) / logf(1 - w));
}