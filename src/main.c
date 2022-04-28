#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "los.h"

#define THREADS 1
#define RESERVOIR_SIZE 10

// FRAGEN:
// - Braucht die free list auch ein Pointer mit Version

void thread(struct los* los) {
    uint8_t own = 0;

    for (;;) {
        while(!(own = acquire(los, own, RESERVOIR_SIZE)));
        struct node* skip = get_node(los, own);

        for (;;) {
            // insert the tuple

            if (--skip->length <= 0) {
                // insert tuple into reservoir

                // acquire new skip
            }
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

    free_los(los);

    printf("Done\n");
}
