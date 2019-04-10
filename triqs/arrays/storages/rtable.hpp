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
#include <vector>

#define NDA_DEBUG_MEMORY
//#define NDA_DEBUG_MEMORY2

namespace nda::mem {

  // -------------- ref count table -----------------------

  // A simple table of counters to count the references to a memory block (handle, cf below).
  // The counters are allocated and freed along the principle of a freelist allocator.
  class rtable_t {
    public:
    using int_t = int32_t;
    std::vector<int_t> ta; // table of the counters
                           // We dont use 0. (id =0 will mean "no counter" in the handle)

    void release(int_t p) { // the counter p in not in use
      ta[p] = 0;
    }

    public:
    rtable_t(long size = 10) : ta(size, 0) {}

    ~rtable_t() {
#ifdef NDA_DEBUG_MEMORY
      if (!empty()) {
        std::cerr << "Error detected in reference counting ! \n";
        for (long i = 0; i < ta.size(); ++i) { std::cerr << " i = " << i << " ta = " << ta[i] << "\n"; }
        std::abort(); // fail the test
      }
#endif
    }

    // Separated from the destructor for testing purpose only.
    bool empty() const {
      for (auto x : ta) {
        if (x) return false;
      }
      return true;
    }

    // yield the number of a new counter >0, and set the counter to 1.
    long get() {
      for (long i = 1; i < ta.size(); ++i) { // NEVER yield 0, it is not used
        if (ta[i] == 0) {
          ta[i] = 1;
          return i;
        }
      }
      auto l = ta.size();
      ta.resize(l + 10, 0);
      ta[l] = 1;
      return l;
    }

    // access to the refs
    std::vector<int_t> const &nrefs() const noexcept { return ta; }

    // increase the counter number p
    void incref(long p) noexcept {
#ifdef NDA_DEBUG_MEMORY2
      std::cerr << "ta.size() " << ta.size() <<"\n";
      std::cerr << "inccref " << p << " nref : " << ta[p] << std::endl;
#endif
      ++ta[p];
    }

    // decrease the counter number p. Return true iif it has reached 0.
    // If it has reached 0, it also releases the counter.
    bool decref(long p) noexcept {
#ifdef NDA_DEBUG_MEMORY2
      if (p ==0 ) {
	std::cerr << "error 1 " << p <<"\n";
	throw std::runtime_error("AIE ");
      }
      if (ta[p] ==0 ) {
	std::cerr << "error " << p <<"\n";
	throw std::runtime_error("NONO");
      }
      if (p >= ta.size()) throw std::runtime_error("NONO");
#endif
      --ta[p];
#ifdef NDA_DEBUG_MEMORY2
      std::cerr << "decref " << p << " nref : " << ta[p] << std::endl;
#endif
      bool reached0 = (ta[p] == 0);
      if (reached0) release(p);
      return reached0;
    }
  };
} // namespace nda::mem
