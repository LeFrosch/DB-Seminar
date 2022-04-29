#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "reservoir.h"

struct reservoir* create_reservoir(size_t size) {
    struct reservoir* reservoir = (struct reservoir*) malloc(sizeof(struct reservoir));
    if (!reservoir) return NULL;

    reservoir->tuple = (struct reservoir_tuple*) calloc(size, sizeof(struct reservoir_tuple));
    if (!reservoir->tuple) return NULL;

    reservoir->size = size;

    return reservoir;
}

void free_reservoir(struct reservoir* reservoir) {
    free(reservoir->tuple);
    free(reservoir);
}

void insert_into_reservoir(struct reservoir* reservoir, void* data, size_t data_size, uint32_t skip_index) {
    size_t index = rand() % reservoir->size;


    struct reservoir_tuple* tuple = reservoir->tuple + index;

    while(!__sync_bool_compare_and_swap(&tuple->lock, 0, 1));

    if (tuple->skip_index > skip_index) {
        goto end;
    }
    if (tuple->data) {
        free(tuple->data);
        tuple->data = NULL;
    }

    void* data_copy = malloc(data_size);
    if (!data_copy) {
        goto end;
    }

    memcpy(data_copy, data, data_size);

    tuple->data = data_copy;
    tuple->skip_index = skip_index;

    end:
    tuple->lock = 0;
}
