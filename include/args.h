#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>
#include <stdbool.h>

// Contains all arguments or default values
struct args {
    // Enables or disables verbose output
    bool verbose;
    // The amount of threads to use for insertion
    size_t threads;
    // The size of the reservoir to fill
    size_t reservoir_size;
    // Insertions per thread
    size_t insertions;
};

void parse_args(struct args* args, int argc, char** argv);

#endif
