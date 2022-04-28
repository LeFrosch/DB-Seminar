#include <stdint.h>
#include <stdio.h>

#include "pointer.h"

#define THREADS 1
#define RESERVOIR_SIZE 10

int main() {
    node_ptr p = 0;

    printf("Index: %d, Version: %d\n", GET_INDEX(p), GET_VERSION(p));

    update_ptr(&p, 0, 13);

    printf("Index: %d, Version: %d\n", GET_INDEX(p), GET_VERSION(p));
}
