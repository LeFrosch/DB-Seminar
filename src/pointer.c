#include "pointer.h"

bool update_ptr(version_ptr* ptr, version_ptr old, uint8_t index) {
    version_ptr new = ((uint64_t) index << 32) | (GET_VERSION(old) + 1);
    return __sync_bool_compare_and_swap(ptr, old, new);
}

