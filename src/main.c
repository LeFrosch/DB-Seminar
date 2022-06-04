#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "args.h"
#include "node.h"
#include "reservoir.h"
#include "los.h"

#define S_TO_NS 1000000000

// Arguments that are used by every thread
struct thread_ctx {
    size_t insertions;
    size_t reservoir_size;
    struct los* los;
    struct reservoir* reservoir;
    pthread_t thread_id;
    uint8_t index;
};

// Performers some expensive calculations to simulate an expensive insertion
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

    for (size_t i = 0; i < ctx->insertions; i++) {
        if (!skip_node) {
            own = acquire(ctx->los, own, ctx->reservoir_size);
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
    struct thread_ctx* ctx = arg;

    for (size_t i = 0; i <  ctx->insertions; i++) {
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

int main(int argc, char **argv) {
    struct args args;
    parse_args(&args, argc, argv);

    srand(42);

    struct reservoir* reservoir = create_reservoir(args.reservoir_size);
    if (!reservoir) {
        fprintf(stderr, "Failed to create reservoir\n");
        return EXIT_FAILURE;
    }

    struct los* los = create_los(args.threads, args.reservoir_size);
    if (!los) {
        fprintf(stderr, "Failed to create los\n");
        return EXIT_FAILURE;
    }

    printf("Creating threads...\n");

    struct thread_ctx* threads = (struct thread_ctx*) calloc(args.threads, sizeof(struct thread_ctx));
    if (!threads) {
        fprintf(stderr, "Failed to allocate thread contexts\n");
        return EXIT_FAILURE;
    }

    printf("Starting threads...\n");

    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < args.threads; i++) {
        struct thread_ctx* ctx = threads + i;

        ctx->index = i;
        ctx->los = los;
        ctx->reservoir = reservoir;

        pthread_create(&ctx->thread_id, NULL, thread_entry, ctx);
    }

    for (size_t i = 0; i < args.threads; i++) {
        pthread_join(threads[i].thread_id, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

    long ns =  finish.tv_sec * S_TO_NS + finish.tv_nsec - start.tv_sec * S_TO_NS - start.tv_nsec;
    long double ms = ns * 1.0e-6L;

    printf("\nMeasurement:\n");
    printf("\tInsertions: %zu\n", args.threads * args.insertions);
    printf("\tDelta: %.0Lfms (%ld ns)\n", roundl(ms), ns);

#ifndef NO_SAMPLE
    print_reservoir(reservoir);
#endif

    free_reservoir(reservoir);
    free_los(los);

    printf("Done\n");
}
