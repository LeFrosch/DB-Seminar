#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "reservoir.h"

#define LOCK(x) while(!__sync_bool_compare_and_swap(x, 0, 1));
#define UNLOCK(x) x = 0

struct reservoir* create_reservoir(size_t size) {
    struct reservoir* reservoir = (struct reservoir*) malloc(sizeof(struct reservoir));
    if (!reservoir) return NULL;

    reservoir->tuple = (struct reservoir_tuple*) calloc(size, sizeof(struct reservoir_tuple));
    if (!reservoir->tuple) return NULL;

    reservoir->size = size;
    reservoir->counter.lock = 0;
    reservoir->counter.value = 0;

    return reservoir;
}

void free_reservoir(struct reservoir* reservoir) {
    free(reservoir->tuple);
    free(reservoir);
}

void insert_into_reservoir_preload(struct reservoir* reservoir, void* data, size_t data_size, size_t index) {
    assert(index < reservoir->size);
    struct reservoir_tuple* tuple = reservoir->tuple + index;

    LOCK(&tuple->lock);

    if (tuple->data != NULL) {
        goto end;
    }
    
    void* data_copy = malloc(data_size);
    if (!data_copy) {
        goto end;
    }

    memcpy(data_copy, data, data_size);
    tuple->data = data_copy;

    end:
    UNLOCK(tuple->lock);
}

void insert_into_reservoir(struct reservoir* reservoir, void* data, size_t data_size, uint32_t skip_index) {
    size_t index = rand() % reservoir->size;
    struct reservoir_tuple* tuple = reservoir->tuple + index;

    LOCK(&tuple->lock);

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
    UNLOCK(tuple->lock);
}

ssize_t preload(struct reservoir* reservoir) {
    LOCK(&reservoir->counter.lock);

    ssize_t result = -1;

    if (reservoir->counter.value < reservoir->size) {
        result = (ssize_t) reservoir->counter.value;
        reservoir->counter.value++;
    }

    UNLOCK(reservoir->counter.lock);

    return result;
}