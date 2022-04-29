#ifndef RESERVOIR_H
#define RESERVOIR_H

#include <stdint.h>

struct reservoir_tuple {
    void* data;
    uint32_t skip_index;
    uint8_t lock;
};

struct reservoir {
    size_t size;
    struct reservoir_tuple* tuple;
};

// Allocates and initializes the reservoir
struct reservoir* create_reservoir(size_t size);

// Deallocates the reservoir
void free_reservoir(struct reservoir* reservoir);

// Inserts a tuple at a random position inside the reservoir. The reservoir
// uses a spin-lock to synchronise writes per tuple.
void insert_into_reservoir(struct reservoir* reservoir, void* data, size_t data_size, uint32_t skip_index);

#endif
