#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "los.h"

#define THREADS 1
#define RESERVOIR_SIZE 10

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
