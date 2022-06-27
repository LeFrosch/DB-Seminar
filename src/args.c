#include <argp.h>

#include "args.h"

#define THREADS 4
#define RESERVOIR_SIZE 10
#define INSERTIONS 10000000

const char *argp_program_version = "DB-Seminar: Concurrent only sampling implementation -- "
#ifndef NO_SAMPLE
                                   "sampling";
#else
                                   "not sampling";
#endif

const char *argp_program_bug_address = "daniel.brauner@tum.de";

static char doc[] = "Simple example implementation of the concurrent only sampling algorithm. There are two binaries, one"
                    "that actually builds the reservoir and one that only iterates over the tuples for comparison. "
#ifndef NO_SAMPLE
                    "This one builds the reservoir.";
#else
                    "This one will NOT build the reservoir.";
#endif

static struct argp_option options[] = {
        {"verbose",    'v', 0,       0, "Produce verbose output",                   0},
        {"threads",    't', "COUNT", 0, "Amount of threads for parallel insertion", 0},
#ifndef NO_SAMPLE
        {"size",       's', "COUNT", 0, "Size of the reservoir",                    0},
#endif
        {"insertions", 'i', "COUNT", 0, "Insertions per thread",                    0},
        {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct args* args = state->input;

    switch (key) {
        case 'v':
            args->verbose = true;
            break;

        case 't':
            if (!arg) argp_error(state, "No value for threads");

            args->threads = strtoul(arg, NULL, 0);

            if (args->threads < 1) argp_error(state, "Invalid value for threads");
            break;

        case 's':
            if (!arg) argp_error(state, "No value for size");

            args->reservoir_size = strtoul(arg, NULL, 0);

            if (args->reservoir_size < 1) argp_error(state, "Invalid value for size");
            break;

        case 'i':
            if (!arg) argp_error(state, "No value for insertions");

            args->insertions = strtoul(arg, NULL, 0);

            if (args->insertions < 1) argp_error(state, "Invalid value for insertions");
            break;

        default:
            break;
    }

    return 0;
}

static struct argp argp = {
        .options = options,
        .parser = parse_opt,
        .args_doc = NULL,
        .doc = doc
};

void parse_args(struct args* args, int argc, char** argv) {
    args->verbose = false;
    args->reservoir_size = RESERVOIR_SIZE;
    args->insertions = INSERTIONS;
    args->threads = THREADS;

    argp_parse(&argp, argc, argv, 0, 0, args);

    if (args->verbose) {
        printf("Reservoir size: %zu\n"
               "Insertions per thread: %zu\n"
               "Thread count: %zu\n\n",
               args->reservoir_size, args->insertions, args->threads
        );
    }
}
