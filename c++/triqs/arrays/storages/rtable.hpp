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
#include <vector>
#include <algorithm>
#include <mutex>
#include <limits>
#include "./../../utility/macros.hpp"

namespace triqs::arrays::mem {

  // -------------- ref count table -----------------------

  // A simple table of counters to count the references to a memory block (handle, cf below).
  // The counters are allocated and freed along the principle of a freelist allocator.
  class rtable_t {
    private:
    // The integer-type for the reference counters
    using int_t = uint16_t;

    // A table of reference counters. The position in the vector will be the counter-id
    // We dont use the first element of the vector (id=0 signifies a null memory handle)
    std::vector<int_t> _refcounts;

    public:
    // The mutex
    std::mutex mtx = {};

    rtable_t(long size = 10) : _refcounts(size, uint16_t{0}) {}

    ~rtable_t() {
#ifdef NDA_DEBUG
      EXPECTS(std::all_of(_refcounts.begin(), _refcounts.end(), [](int_t c) { return c == 0; }));
#endif
    }

    // Initialize the next empty counter (to 1) and return the id
    long get() noexcept {

      // NEVER yield 0, id=0 is associated with the null-handle
      for (long i = 1; i < _refcounts.size(); ++i) {
        if (_refcounts[i] == 0) {
          _refcounts[i] = 1;
          return i;
        }
      }

      // If no empty slot was found we have to resize
      auto l = _refcounts.size();
      _refcounts.resize(l + 10, 0);
      _refcounts[l] = 1;
      return l;
    }

    // access to the refs
    std::vector<int_t> const &refcounts() const noexcept { return _refcounts; }

    // increase the counter number p
    void incref(long id) noexcept {
#ifdef NDA_DEBUG
      EXPECTS(_refcounts[id] < std::numeric_limits<int_t>::max());
#endif
      // This function needs to be executed serially
      std::lock_guard<std::mutex> lock(mtx);

      ++_refcounts[id];
    }

    // decrease the counter number p. Return true iif it has reached 0.
    // If it has reached 0, it also releases the counter.
    bool decref(long id) noexcept {
#ifdef NDA_DEBUG
      EXPECTS(id != 0);
      EXPECTS(_refcounts[id] > 0);
      EXPECTS(id < _refcounts.size());
#endif
      // This function needs to be executed serially
      std::lock_guard<std::mutex> lock(mtx);

      --_refcounts[id];
      return _refcounts[id] == 0;
    }
  };
} // namespace triqs::arrays::mem
