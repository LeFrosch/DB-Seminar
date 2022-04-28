#ifndef SKIP_NODE_H
#define SKIP_NODE_H

#include <stdint.h>
#include <stdbool.h>

#define NULL_PTR 0
typedef uint64_t version_ptr;

// Returns the version of the pointer, bits [0-31]
#define GET_VERSION(ptr) ((uint32_t) (ptr))

// Return the index of the pointer, bits [32-39]
#define GET_INDEX(ptr) ((uint8_t) ((ptr) >> 32))

// Tries to update the pointer atomicity if the pointer is equal to the old
// pointer. If the pointer do not match false is returned otherwise the version
// of the old is increased by one and the index is updated.
bool update_ptr(version_ptr* ptr, version_ptr old, uint8_t index);

#endif 