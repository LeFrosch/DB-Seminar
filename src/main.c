#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "node.h"
#include "los.h"

#define THREADS 4
#define RESERVOIR_SIZE 10
#define INSERTIONS_PER_THREAD 10000

// FRAGEN:
// - Braucht die free list auch ein Pointer mit Version

void* thread_entry(void* los) {
    pthread_t thread_id = pthread_self();

    uint8_t own = 0;
    struct node* skip_node = NULL;

    for (size_t i = 0; i < INSERTIONS_PER_THREAD; i++) {
        if (!skip_node) {
            uint8_t acquired = NULL_PTR;
            while (!acquired) {
                acquired = acquire(los, &own, RESERVOIR_SIZE);
            }

            own = acquired;
            skip_node = get_node(los, own);

            printf("Thread %lu acquired skip: %d { .index: %d, .length: %lu, .w: %f }\n", thread_id, own, skip_node->index, skip_node->length, skip_node->w_value);
        }

        if (--skip_node->length <= 0) {
            // insert tuple into reservoir

            skip_node = NULL;
        }
    }
}

int main() {
    printf("Threads: %d, Reservoir size: %d\n", THREADS, RESERVOIR_SIZE);

    struct los* los = create_los(THREADS, RESERVOIR_SIZE);
    if (!los) {
        fprintf(stderr, "Failed to create los\n");
        return EXIT_FAILURE;
    }

    printf("Creating threads...\n");

    pthread_t threads[THREADS];
    for (size_t i = 0; i < THREADS; i++) {
        pthread_create(threads + i, NULL, thread_entry, los);
    }

    printf("Waiting for threads...\n");

    for (size_t i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    free_los(los);

    printf("Done\n");
}
