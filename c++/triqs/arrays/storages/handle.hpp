// Copyright (c) 2019-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once
#include <limits>
#include <complex>
#include <type_traits>
#include <cstring>
#include "./blk.hpp"
#include "./rtable.hpp"

namespace triqs::arrays::mem {

  // -------------- Traits ---------------------------

  template <typename T> struct is_complex : std::false_type {};
  template <typename T> struct is_complex<std::complex<T>> : std::true_type {};

  // -------------- Allocation Functions ---------------------------

  allocators::blk_t allocate(size_t size);
  allocators::blk_t allocate_zero(size_t size);
  void deallocate(allocators::blk_t b);

  // -------------- Utilities ---------------------------

  // To have aligned objects, use aligner<T, alignment> instead of T in constructor and get
  template <typename T, int Al> struct alignas(Al) aligner {
    T x;
    T &get() noexcept { return x; }
    T const &get() const noexcept { return x; }
  };

  // -------------- Global table  ---------------------------

  struct globals {
    static constexpr bool init_dcmplx = true; // initialize dcomplex to 0 globally
    static rtable_t rtable;                   // the table of the ref counter.
  };

  // -------------- handle ---------------------------

  /*
   * The block of memory for the arrays
   * R : Regular (owns the memory)
   * S : Share (shared memory ownership)
   * B : Borrowed (no memory ownership)
   */
  template <typename T, char rbs> struct handle;

  // ------------------  Regular -------------------------------------

  struct do_not_initialize_t {};
  inline static constexpr do_not_initialize_t do_not_initialize{};

  struct init_zero_t {};
  inline static constexpr init_zero_t init_zero{};

  template <typename T> struct handle<T, 'R'> {
    private:
    T *_data     = nullptr; // Pointer to the start of the memory block
    size_t _size = 0;       // Size of the memory block. Invariant: size > 0 iif data != 0

    // The regular handle can share its memory with handle<T, 'S'>
    mutable long _id = 0; // The id in the refcounts table.
                          // id == 0 corresponds to the case of no memory sharing
                          // This field must be mutable for the cross construction of 'S'. Cf 'S'.
                          // Invariant: id == 0 if data == nullptr
    friend handle<T, 'S'>;

    void decref() noexcept {
      static_assert(std::is_nothrow_destructible_v<T>, "triqs::arrays::mem::handle requires the value_type to have a non-throwing constructor");
      if (is_null()) return;

      // Check if the memory is shared and still pointed to
      if (has_shared_memory() and not globals::rtable.decref(_id)) return;

      // If needed, call the T destructors
      if constexpr (!std::is_trivial_v<T>) {
        for (size_t i = 0; i < _size; ++i) _data[i].~T();
      }

      // Deallocate the memory block
      deallocate({(char *)_data, _size * sizeof(T)});
    }

    public:
    using value_type = T;

    ~handle() noexcept { decref(); }

    handle() = default;

    // Construct by making a clone of the data
    handle(handle const &x) : handle(handle<T, 'B'>{x}) {}

    handle(handle &&x) noexcept {
      _data   = x._data;
      _size   = x._size;
      _id     = x._id;
      x._data = nullptr;
      x._size = 0;
      x._id   = 0;
    }

    handle &operator=(handle const &x) {
      *this = handle{x};
      return *this;
    }

    handle &operator=(handle &&x) noexcept {
      decref();
      _data   = x._data;
      _size   = x._size;
      _id     = x._id;
      x._data = nullptr;
      x._size = 0;
      x._id   = 0;
      return *this;
    }

    // Set up a memory block of the correct size without initializing it
    handle(long size, do_not_initialize_t) {
      if (size == 0) return;               // no size -> null handle
      auto b = allocate(size * sizeof(T)); //, alignof(T));
      ASSERT(b.ptr != nullptr);
      _data = (T *)b.ptr;
      _size = size;
    }

    // Set up a memory block of the correct size without initializing it
    handle(long size, init_zero_t) {
      static_assert(std::is_scalar_v<T> or is_complex<T>::value, "Internal Error");
      if (size == 0) return;                    // no size -> null handle
      auto b = allocate_zero(size * sizeof(T)); //, alignof(T));
      ASSERT(b.ptr != nullptr);
      _data = (T *)b.ptr;
      _size = size;
    }

    // Construct a new block of memory of given size and init if needed.
    handle(long size) {
      if (size == 0) return; // no size -> null handle

      allocators::blk_t b;
      if constexpr (is_complex<T>::value && globals::init_dcmplx)
        b = allocate_zero(size * sizeof(T));
      else
        b = allocate(size * sizeof(T));

      ASSERT(b.ptr != nullptr);
      _data = (T *)b.ptr;
      _size = size;

      // Call placement new except for complex types
      if constexpr (!std::is_trivial_v<T> and !is_complex<T>::value) {
        for (size_t i = 0; i < size; ++i) new (_data + i) T();
      }
    }

    // Construct by making a clone of the data
    handle(handle<T, 'S'> const &x) : handle(handle<T, 'B'>{x}) {}

    // Construct by making a clone of the data
    handle(handle<T, 'B'> const &x) : handle(x.size(), do_not_initialize) {
      if (is_null()) return; // nothing to do for null handle
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(_data, x.data(), x.size() * sizeof(T));
      } else {
        for (size_t i = 0; i < _size; ++i) new (_data + i) T(x[i]); // placement new
      }
    }

    T &operator[](long i) const noexcept { return _data[i]; }

    bool is_null() const noexcept {
#ifdef NDA_DEBUG
      // Check the Invariants in Debug Mode
      EXPECTS(_data != nullptr or _id == 0);
      EXPECTS((_data == nullptr) == (_size == 0));
#endif
      return _data == nullptr;
    }
    bool has_shared_memory() const noexcept { return _id != 0; }

    // Helper function for construction of array<T> when T is not default constructible
    template <typename U> void init_raw(long i, U &&x) { new (_data + i) T{std::forward<U>(x)}; }

    // A const-handle does not entail T const data
    T *data() const noexcept { return _data; }

    long size() const noexcept { return _size; }
  };

  // ------------------  Shared -------------------------------------

  template <typename T> struct handle<T, 'S'> {
    static_assert(std::is_nothrow_destructible_v<T>, "triqs::arrays::mem::handle requires the value_type to have a non-throwing constructor");

    private:
    T *_data     = nullptr; // Pointer to the start of the memory block
    size_t _size = 0;       // Size of the memory block. Invariant: size > 0 iif data != 0

    long _id = 0; // The id in the refcounts table. id == 0 corresponds to a null-handle
                  // Invariant: id == 0 iif data == nullptr

    // Allow to take ownership of a shared pointer from another lib, e.g. numpy.
    void *_foreign_handle = nullptr; // Memory handle of the foreign library
    void *_foreign_decref = nullptr; // Function pointer to decref of the foreign library (void (*)(void *))

    public:
    using value_type = T;

    void decref() noexcept {
      if (is_null()) return;

      // Check if the memory is shared and still pointed to
      if (!globals::rtable.decref(_id)) return;

      // If the memory was allocated in a foreign foreign lib, release it there
      if (_foreign_handle) {
        ((void (*)(void *))_foreign_decref)(_foreign_handle);
        return;
      }

      // If needed, call the T destructors
      if constexpr (!std::is_trivial_v<T>) {
        for (size_t i = 0; i < _size; ++i) _data[i].~T();
      }

      // Deallocate the memory block
      deallocate({(char *)_data, _size * sizeof(T)});
    }

    void incref() noexcept {
#ifdef NDA_DEBUG
      EXPECTS(!is_null());
#endif
      globals::rtable.incref(_id);
    }

    private:
    // basic part of copy, no ref handling here
    void _copy(handle const &x) noexcept {
      _data           = x._data;
      _size           = x._size;
      _id             = x._id;
      _foreign_handle = x._foreign_handle;
      _foreign_decref = x._foreign_decref;
    }

    public:
    ~handle() noexcept { decref(); }

    handle() = default;

    handle(handle const &x) noexcept {
      _copy(x);
      if (!is_null()) incref();
    }

    handle(handle &&x) noexcept {
      _copy(x);

      // Invalidate x so it destructs trivally
      x._data = nullptr;
      x._size = 0;
      x._id   = 0;
    }

    handle &operator=(handle const &x) noexcept {
      decref(); // Release my ref if I have one
      _copy(x);
      incref();
      return *this;
    }

    handle &operator=(handle &&x) noexcept {
      decref(); // Release my ref if I have one
      _copy(x);

      // Invalidate x so it destructs trivially
      x._data = nullptr;
      x._size = 0;
      x._id   = 0;

      return *this;
    }

    // Construct from foreign library shared object
    handle(T *data, size_t size, void *foreign_handle, void *foreign_decref) noexcept
       : _data(data), _size(size), _foreign_handle(foreign_handle), _foreign_decref(foreign_decref) {
      // Only one thread should fetch the id
      std::lock_guard<std::mutex> lock(globals::rtable.mtx);
      _id = globals::rtable.get();
    }

    // Cross construction from a regular handle
    handle(handle<T, 'R'> const &x) noexcept : _data(x.data()), _size(x.size()) {
      if (x.is_null()) return;

      // Get an id if necessary
      if (x._id == 0) {
        // Only one thread should fetch the id
        std::lock_guard<std::mutex> lock(globals::rtable.mtx);
        if (x._id == 0) x._id = globals::rtable.get();
      }
      _id = x._id;
      // Increase refcount
      incref();
    }

    T &operator[](long i) const noexcept { return _data[i]; }

    bool is_null() const noexcept {
#ifdef NDA_DEBUG
      // Check the Invariants in Debug Mode
      EXPECTS((_data == nullptr) == (_size == 0));
      EXPECTS((_data == nullptr) == (_id == 0));
#endif
      return _data == nullptr;
    }

    long refcount() const noexcept { return globals::rtable.refcounts()[_id]; }

    // A constant handle does not entail T const data
    T *data() const noexcept { return _data; }

    long size() const noexcept { return _size; }
  };

  // ------------------  Borrowed -------------------------------------

  template <typename T> struct handle<T, 'B'> {
    private:
    handle<T, 'R'> const *_parent = nullptr; // Parent, Required for regular->shared promotion in Python Converter
    T *_data                      = nullptr; // Pointer to the start of the memory block
    size_t _size                  = 0;       // Size of the memory block. Invariant: size > 0 iif data != 0

    public:
    using value_type = T;

    handle() = default;
    handle(T *ptr, size_t s) noexcept : _data(ptr), _size(s) {}
    handle(handle<T, 'R'> const &x) noexcept : _parent(&x), _data(x.data()), _size(x.size()) {}
    handle(handle<T, 'S'> const &x) noexcept : _data(x.data()), _size(x.size()) {}

    T &operator[](long i) const noexcept { return _data[i]; }

    bool is_null() const noexcept { return _data == nullptr; }

    handle<T, 'R'> const *parent() const { return _parent; }

    // A const-handle does not entail T const data
    T *data() const noexcept { return _data; }

    long size() const noexcept { return _size; }
  };

} // namespace triqs::arrays::mem
