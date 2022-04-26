#include <stdio.h>
#include <stdint.h>

#define THREADS 1

// Pointer in the preallocated node array with version
struct head_ptr {
	uint32_t version;
	uint8_t index;
};

// Contains information about the skip
struct skip_node {
    float w_value;	
    uint64_t length;
    uint32_t index;	
    uint8_t successor;
};

int worker(struct head_ptr* free_list, struct head_ptr* skip_list) {

}

int main() {
	// prealloacted list of nodes, one for every thread and one for the upcoming node
	struct skip_node nodes[THREADS + 1];

	// List of currently unused nodes
	struct head_ptr free_list;

	// List of skips that are to be distributed 
	struct head_ptr skip_list;
}

