#include <cstddef>
#include <iostream>

#include "./handle.hpp"

namespace nda::mem {
  allocator_t globals::alloc; // global allocator for arrays.
  rtable_t globals::rtable;   // the table of the ref counter.

} // namespace nda::mem
