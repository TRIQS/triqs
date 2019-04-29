/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include <iostream>

namespace nda::allocators {

  // ------------------------ The memory block with its size  -------------
  struct blk_t {
    char *ptr = nullptr;
    size_t s  = 0;
  };

  // ------------------------  Utility -------------

  inline size_t round_to_align(size_t s) {
    size_t a = alignof(std::max_align_t);
    return ((s + a - 1) / a) * a; // s = 3a +2  --> (4a + 2)/a = 4 --> 4a
  }

  static constexpr unsigned aligment = alignof(std::max_align_t);

  // -------------------------  Malloc allocator ----------------------------
  //
  // Allocates simply with malloc
  //
  class mallocator {
    public:
    mallocator()                   = default;
    mallocator(mallocator const &) = delete;
    mallocator(mallocator &&)      = default;
    mallocator &operator=(mallocator const &) = delete;
    mallocator &operator=(mallocator &&) = default;

    blk_t allocate(size_t s) { return {(char *)malloc(s), s}; } //NOLINT

    void deallocate(blk_t b) noexcept { free(b.ptr); }
  };

  // -------------------------  Stack allocator ----------------------------
  //
  // Allocates on a fixed size stack, FIFO style
  //
  template <size_t Size> class stack {
    char d[Size]; // stack piece
    char *p = d;  // current position

    public:
    stack()              = default;
    stack(stack const &) = delete;
    stack(stack &&)      = default;
    stack &operator=(stack const &) = delete;
    stack &operator=(stack &&) = default;

    blk_t allocate(size_t s) {
      auto n = round_to_align(s);

      // Check if the stack is full
      if (n > (d + Size) - p) return {nullptr, 0};

      auto p1 = p;
      p += n;
      return {p1, s};
    }

    void deallocate(blk_t b) noexcept {
      // Roll back iif at top of the stack
      if (b.ptr + b.s == p)
        p = b.ptr;
      else
        std::abort();
    }

    bool owns(blk_t b) const noexcept { return b.ptr >= d and b.ptr < d + Size; }
  };

  // -------------------------  Free_list allocator ----------------------------
  //
  // Using a free list technique
  //
  template <typename Parent, size_t Smin, size_t Smax> class free_list {
    Parent parent;
    struct node {
      node *next;
    } *root = nullptr;

    bool size_is_ok(size_t s) { return s >= Smin and s <= Smax; }

    public:
    free_list()                  = default;
    free_list(free_list const &) = delete;
    free_list(free_list &&)      = default;
    free_list &operator=(free_list const &) = delete;
    free_list &operator=(free_list &&) = default;

    blk_t allocate(size_t s) {
      if (size_is_ok(s) and root) {
        auto root1 = root;
        root       = root->next;
        return blk_t{(char *)root, s};
      }
      return parent.allocate(s);
    }

    void deallocate(blk_t b) noexcept {
      if (not size_is_ok(b.s)) return parent.deallocate(b);
      auto *p = (node *)b.ptr;
      p->next = root;
      root    = p;
    }

    bool owns(blk_t b) noexcept { return size_is_ok(b) or parent.owns(b); }
  };

  // -------------------------  segregator allocator ----------------------------
  //
  // Dispatch according to size to two allocators
  //
  template <size_t Threshold, typename A, typename B> class segregator {
    A small;
    B big;

    public:
    segregator()                   = default;
    segregator(segregator const &) = delete;
    segregator(segregator &&)      = default;
    segregator &operator=(segregator const &) = delete;
    segregator &operator=(segregator &&) = default;

    blk_t allocate(size_t s) { return s <= Threshold ? small.allocate(s) : big.allocate(s); }
    void deallocate(blk_t b) noexcept { return b.s <= Threshold ? small.deallocate(b) : big.deallocate(b); }
    bool owns(blk_t b) const noexcept { return small.owns(b) or big.owns(b); }
  };

  // -------------------------  fallback allocator ----------------------------
  //
  // Fallback for composition
  //
  template <typename A, typename F> class fallback : A, F {

    public:
    fallback()                 = default;
    fallback(fallback const &) = delete;
    fallback(fallback &&)      = default;
    fallback &operator=(fallback const &) = delete;
    fallback &operator=(fallback &&) = default;

    blk_t allocate(size_t s) {
      blk_t r = A::allocate(s);
      if (!r.ptr) F::allocate(s);
      return r;
    }

    void deallocate(blk_t b) noexcept {
      if (A::owns(b))
        A::deallocate(b);
      else
        F::deallocate(b);
    }

    bool owns(blk_t b) const noexcept { return A::owns(b) or F::owns(b); }
  };

  // -------------------------   allocator with statistics ----------------------------
  //
  // stats
  //
  template <typename A> class stats : A {

    long memory_used = 0;

    public:
    stats()              = default;
    stats(stats const &) = delete;
    stats(stats &&)      = default;
    stats &operator=(stats const &) = delete;
    stats &operator=(stats &&) = default;

    ~stats() {
      if (!empty()) {
        std::cerr << "Allocator : MEMORY LEAK of " << memory_used << " bytes\n";
        std::abort();
      }
    }

    bool empty() const { return (memory_used == 0); }

    blk_t allocate(size_t s) {
      blk_t b     = A::allocate(s);
      memory_used = memory_used + b.s;
      //      std::cerr<< "Allocating "<< b.s << "Total = "<< memory_used << "\n";
      return b;
    }

    void deallocate(blk_t b) noexcept {
      memory_used -= b.s;
      //    std::cerr<< "Deallocating "<< b.s << "Total = "<< memory_used << "\n";

      if (memory_used < 0) {
        std::cerr << "Allocator : memory_used <0 : " << memory_used << " b.s = " << b.s << " b.ptr = " << (void *)b.ptr;
        std::abort();
      }
      A::deallocate(b);
    }

    bool owns(blk_t b) const noexcept { return A::owns(b); }

    long get_memory_used() const noexcept { return memory_used; }
  };

} // namespace nda::allocators
