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
#include <limits>
#include <complex>
#include "./allocators.hpp"
#include "./rtable.hpp"

#define FORCEINLINE __inline__ __attribute__((always_inline))

//#define FORCEINLINE inline [[gnu::always_inline]]

#define restrict __restrict__

namespace nda::mem {

  // -------------- Traits ---------------------------

  template <typename T> struct is_complex : std::false_type {};
  template <typename T> struct is_complex<std::complex<T>> : std::true_type {};

  // -------------- Allocators ---------------------------

  // First the allocator of the memory block (data).
  // FIXME CHANGE alloc at runtime ??
  using allocator1_t = allocators::mallocator;
  //using allocator1_t = allocators::stack_allocator<10000>; //mallocator;
  //using allocator1_t = allocators::free_list<stack_allocator<10000>, 1, 200>; //mallocator;

#ifndef NDA_DEBUG_MEMORY
  using allocator_t = allocator1_t;
#else
  using allocator_t = allocators::stats<allocator1_t>;
#endif

  // -------------- Utilities ---------------------------

  // To have aligned objects, use aligner<T, alignment> instead of T in constructor and get
  template <typename T, int Al> struct alignas(Al) aligner {
    T x;
    FORCEINLINE T &get() { return x; }
    FORCEINLINE T const &get() const { return x; }
  };

  // -------------- Global table  ---------------------------

  struct globals {
    static allocator_t alloc; // global allocator for arrays.
    static rtable_t rtable;   // the table of the ref counter.
  };

  // -------------- Decide if type T will need to be constructed/destructed

  template <typename T>
  //constexpr bool requires_construction_and_destruction = (not(std::is_arithmetic_v<T> || std::is_pod_v<T>));
  constexpr bool requires_construction_and_destruction = (not(std::is_arithmetic_v<T> || is_complex<T>::value || std::is_pod_v<T>));

  // -------------- handle ---------------------------

  /*
   * The block of memory for the arrays
   * R : Regular,  S : Share B : Borrowed (no ownership)
   */
  template <typename T, char rbs> struct handle;

  // ------------------  Borrowed -------------------------------------

  template <typename T> struct handle<T, 'B'> {

    using value_type = T; // FIXME : TRIQS

    T *restrict data = nullptr; // start of data
    size_t size      = 0;       // size of the memory block. Invariant : >0 iif data !=0

    handle() = default;
    handle(T *p, size_t s) : data(p), size(s) {}
    handle(handle<T, 'R'> const &x) : data(x.data), size(x.size) {}
    handle(handle<T, 'S'> const &x) : data(x.data), size(x.size) {}

    T &operator[](long i) const noexcept { return data[i]; }
  };

  // ------------------  Shared -------------------------------------

  template <typename T> struct handle<T, 'S'> {

    using value_type = T; // FIXME : TRIQS

    T *restrict data = nullptr; // start of data
    size_t size      = 0;       // size of the memory block. Invariant : >0 iif data !=0
    long id          = 0;       // the id in the counter ref table : 0 means no counter allocated.

    // Allows to take ownership of a shared pointer with another lib, e.g. numpy.
    void *sptr        = nullptr; // A foreign library shared ptr
    void *release_fnt = nullptr; // void (*)(void *) : release function of the foreign sptr

    using release_fnt_t = void (*)(void *);

    void decref() noexcept {
      if (id == 0) return;                     // id=0 is the default constructed state. No cleaning there
      if (!globals::rtable.decref(id)) return; // if the ref count is still > 0
      if (sptr) {                              // the memory was a foreign lib, release it
        (*(release_fnt_t)release_fnt)(sptr);
        return;
      }
      // Now we need to clean memory. First We destroy object if needed
      if constexpr (requires_construction_and_destruction<T>) {
        for (size_t i = 0; i < size; ++i) data[i].~T();
      }
      // and now dellocate the memory block
      globals::alloc.deallocate({(char *)data, size * sizeof(T)});
    }

    void incref() noexcept {
      if (id) globals::rtable.incref(id);
    }

    private:
    // basic part of copy, no ref handling here
    FORCEINLINE void _copy(handle const &x) noexcept {
      data        = x.data;
      size        = x.size;
      id          = x.id;
      sptr        = x.sptr;
      release_fnt = x.release_fnt;
    }

    public:
    handle() = default;

    handle(handle &&x) noexcept {
      _copy(x);
      x.id = 0; // x is trivially destructible now. we steal the ref from x.
    }

    handle(handle const &x) noexcept {
      _copy(x);
      incref();
    }

    ~handle() noexcept { decref(); }

    //
    handle &operator=(handle const &x) noexcept {
      decref(); // Release my ref if I have one
      _copy(x);
      incref();
      return *this;
    }

    //
    handle &operator=(handle &&x) noexcept {
      decref(); // Release my ref if I have one
      _copy(x);
      x.id = 0; // x is trivially destructible now. we steal the ref from x.
      return *this;
    }

    // Cross construction from Regular. When constructing a shared_view from a regular type.
    handle(handle<T, 'R'> const &x) noexcept : data(x.data), size(x.size) {
      if (x.data == nullptr) return; //id =0 in this case.

      if (!x.id) x.id = globals::rtable.get();
      id = x.id;
      incref(); // to count for this
    }

    long nref() const noexcept { return globals::rtable.nrefs()[id]; }
    T &operator[](long i) const noexcept { return data[i]; }
  };

  // ------------------  Simple tag -------------------------------------

  struct do_not_initialize_t {};
  inline static constexpr do_not_initialize_t do_not_initialize{};

  // ------------------  Regular -------------------------------------

  template <typename T> struct handle<T, 'R'> {

    using value_type = T; // FIXME : TRIQS

    static constexpr bool _destructor_of_T_is_no_except = noexcept(std::declval<T>().~T());

    T *restrict data = nullptr; // start of data
    size_t size      = 0;       // size of the memory block. Invariant : >0 iif data !=0
    mutable long id  = 0;       // the id in the counter ref table : 0 means no counter.
                                // must be mutable for the cross construction of S. Cf S.

    private:
    void decref() const noexcept(_destructor_of_T_is_no_except) {
      if (data == nullptr) return; // nothing to do
      // If id ==0 , we want to deallocate here
      // if id !=0, we dellocate only if rtable decref is true
      if (id and (!globals::rtable.decref(id))) return;         // do NOT dellocate
      if constexpr (requires_construction_and_destruction<T>) { // if we need to call a destructor, call it.
        for (size_t i = 0; i < size; ++i) data[i].~T();
      }
      globals::alloc.deallocate({(char *)data, size * sizeof(T)}); // dellocate the memory block
    }

    public:
    // if the block was used by a shared block, we need to clean it like a shared block too.
    ~handle() noexcept(_destructor_of_T_is_no_except) { // noexcept iif ~T is no except !
      decref();
    }

    handle() = default;

    handle(handle &&x) noexcept {
      data   = x.data;
      size   = x.size;
      id     = x.id;
      x.data = nullptr; // x is destructible with no operation
      x.id   = 0;
    }

    handle &operator=(handle &&x) noexcept {
      decref();
      data   = x.data;
      size   = x.size;
      id     = x.id;
      x.data = nullptr; // x is destructible with no operation
      x.id   = 0;
      return *this;
    }

    // Using copy and move
    handle &operator=(handle const &x) noexcept {
      *this = handle{x};
      return *this;
    }

    // set up a memory block of the correct size. No init.
    handle(long s, do_not_initialize_t) {
      if (s == 0) return;                              // no size, nullptr
      auto b = globals::alloc.allocate(s * sizeof(T)); //, alignof(T));
      if (!b.ptr) throw std::bad_alloc();
      data = (T *)b.ptr;
      size = s;
    }

    // For construction of array<T> when T is not default constructible
    template <typename U> void init_raw(long i, U &&x) { new (data + i) T{std::forward<U>(x)}; }

    // Construct a new block of memory of given size and init if needed.
    handle(long size) : handle(size, do_not_initialize) {
      if constexpr (requires_construction_and_destruction<T>) {
        for (size_t i = 0; i < size; ++i) new (data + i) T();
      }
      //if constexpr (is_complex<T>::value) {
        //for (size_t i = 0; i < size; ++i) data[i] = std::numeric_limits<double>::quiet_NaN();
      //}

    }

    // NB : do not use template, or it will not be selected.
    // Construct by making a clone of the data
    handle(handle<T, 'R'> const &x) : handle(handle<T, 'B'>{x}) {}

    // Construct by making a clone of the data
    handle(handle<T, 'S'> const &x) : handle(handle<T, 'B'>{x}) {}

    // Construct by making a clone of the data
    handle(handle<T, 'B'> const &x) : handle(x.size, do_not_initialize) {
      for (size_t i = 0; i < size; ++i) new (data + i) T(x.data[i]); // placement new
    }

    T &operator[](long i) const noexcept { return data[i]; }
  };

} // namespace nda::mem
