#include <cstddef>

#include "./handle.hpp"

namespace nda::mem {

  // The global table of reference counters
  rtable_t globals::rtable;

  // The allocator type for nda::mem::handle has to be fixed in the library
  // as combining different allocator types can lead to problems
#ifndef NDA_LEAK_CHECK
  using allocator_t = allocators::mallocator;
#else
  using allocator_t = allocators::stats<allocators::mallocator>;
#endif
  allocator_t alloc;
  allocators::blk_t allocate(size_t size) { return alloc.allocate(size); }
  void deallocate(allocators::blk_t b) { alloc.deallocate(b); }
} // namespace nda::mem
