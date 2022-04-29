#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "node.h"
#include "reservoir.h"
#include "los.h"

#define THREADS 4
#define RESERVOIR_SIZE 10
#define INSERTIONS_PER_THREAD 10000000

// FRAGEN:
// - Braucht die free list auch ein Pointer mit Version

struct thread_ctx {
    struct los* los;
    struct reservoir* reservoir;
    pthread_t thread_id;
    uint8_t index;
};

void* thread_entry(void* arg) {
    struct thread_ctx* ctx = arg;

    uint8_t own = 0;
    struct node* skip_node = NULL;

    for (size_t i = 0; i < INSERTIONS_PER_THREAD; i++) {
        if (!skip_node) {
            own = acquire(ctx->los, own, RESERVOIR_SIZE);
            skip_node = get_node(ctx->los, own);
        }

        if (--skip_node->length <= 0) {
            char buf[100];
            snprintf(buf, 100, "Thread: %i, Tuple: %lu", ctx->index, i);

            insert_into_reservoir(ctx->reservoir, buf, 100, skip_node->index);

            skip_node = NULL;
        }
    }

    return NULL;
}

void print_reservoir(struct reservoir* reservoir) {
    printf("\nReservoir content:\n");

    for (size_t i = 0; i < reservoir->size; i++) {
        printf("\t%lu: %s\n", i, (char*) reservoir->tuple[i].data);
    }

    printf("\n");
}

int main() {
    printf("Threads: %i, Reservoir size: %i\n", THREADS, RESERVOIR_SIZE);

    srand(42);

    struct reservoir* reservoir = create_reservoir(RESERVOIR_SIZE);
    if (!reservoir) {
        fprintf(stderr, "Failed to create reservoir\n");
        return EXIT_FAILURE;
    }

    struct los* los = create_los(THREADS, RESERVOIR_SIZE);
    if (!los) {
        fprintf(stderr, "Failed to create los\n");
        return EXIT_FAILURE;
    }

    printf("Creating threads...\n");

    struct thread_ctx* threads = (struct thread_ctx*) calloc(THREADS, sizeof(struct thread_ctx));
    if (!threads) {
        fprintf(stderr, "Failed to allocate thread contexts\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < THREADS; i++) {
        struct thread_ctx* ctx = threads + i;

        ctx->index = i;
        ctx->los = los;
        ctx->reservoir = reservoir;

        pthread_create(&ctx->thread_id, NULL, thread_entry, ctx);
    }

    printf("Waiting for threads...\n");

    for (size_t i = 0; i < THREADS; i++) {
        pthread_join(threads[i].thread_id, NULL);
    }

    print_reservoir(reservoir);

    free_reservoir(reservoir);
    free_los(los);

    printf("Done\n");
}
