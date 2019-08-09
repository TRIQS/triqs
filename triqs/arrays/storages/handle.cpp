#include <cstddef>
#include <type_traits>

#include "./handle.hpp"
#include "./allocators.hpp"

namespace nda::mem {

  // The global table of reference counters
  rtable_t globals::rtable;

  // The allocator type for nda::mem::handle has to be fixed in the library
  // as combining different allocator types can lead to problems

  //using allocator_base_t = allocators::segregator<8*100, allocators::multiple_bucket<8*100>, allocators::mallocator>;
  using allocator_base_t = allocators::mallocator;

#ifndef NDA_LEAK_CHECK
  using allocator_t = allocator_base_t;
#else
  using allocator_t = allocators::leak_check<allocator_base_t>;
#endif

  allocator_t alloc;
  allocators::blk_t allocate(size_t size) { return alloc.allocate(size); }
  allocators::blk_t allocate_zero(size_t size) {
    if constexpr (std::is_same_v<allocator_t, allocators::mallocator>) {
      return alloc.allocate_zero(size);
    } else {
      auto blk = alloc.allocate(size);
      std::memset(blk.ptr, 0, size);
      return blk;
    }
  }
  void deallocate(allocators::blk_t b) { alloc.deallocate(b); }
} // namespace nda::mem
