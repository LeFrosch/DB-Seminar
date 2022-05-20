#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "node.h"
#include "reservoir.h"
#include "los.h"

#define THREADS 4
#define RESERVOIR_SIZE 10
#define INSERTIONS_PER_THREAD 10000000
#define S_TO_NS 1000000000

// FRAGEN:
// - Braucht die free list auch ein Pointer mit Version

struct thread_ctx {
    struct los* los;
    struct reservoir* reservoir;
    pthread_t thread_id;
    uint8_t index;
};

void insertion_placeholder() {
    for (size_t i = 0; i < 10; i++) {
        long double result = atan2l(i, i);
        __asm__ volatile("" : "+g" (result) : :);
    }
}

#ifndef NO_SAMPLE
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

        insertion_placeholder();
    }

    return NULL;
}

#else

void* thread_entry(void* arg) {
    (void) arg;

    for (size_t i = 0; i < INSERTIONS_PER_THREAD; i++) {
        insertion_placeholder();
    }

    return NULL;
}

#endif

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

    printf("Starting threads...\n");

    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < THREADS; i++) {
        struct thread_ctx* ctx = threads + i;

        ctx->index = i;
        ctx->los = los;
        ctx->reservoir = reservoir;

        pthread_create(&ctx->thread_id, NULL, thread_entry, ctx);
    }

    for (size_t i = 0; i < THREADS; i++) {
        pthread_join(threads[i].thread_id, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

    long ns =  finish.tv_sec * S_TO_NS + finish.tv_nsec - start.tv_sec * S_TO_NS - start.tv_nsec;
    long double ms = ns * 1.0e-6L;

    printf("\nMeasurement:\n");
    printf("\tInsertions: %d\n", INSERTIONS_PER_THREAD *  THREADS);
    printf("\tDelta: %.0Lfms (%ld ns)\n", roundl(ms), ns);

#ifndef NO_SAMPLE
    print_reservoir(reservoir);
#endif

    free_reservoir(reservoir);
    free_los(los);

    printf("Done\n");
}
