/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_UTILITY_SCOPE_GUARD_H
#define TRIQS_UTILITY_SCOPE_GUARD_H

namespace triqs { namespace utility {

// adapted from A. Alexandrescu talk, C++ & beyond 2013.
template <class Fun>
class scope_guard {
 Fun f_;
 bool active_;
 public:
 scope_guard(Fun f) : f_(std::move(f)) , active_(true) {}
 ~scope_guard() { if (active_) f_(); }
 void dismiss() { active_ = false; }
 bool active() const { return active_;}
 scope_guard() = delete;
#ifdef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS
 private:
 scope_guard(const scope_guard&) = default;
 public:
#else
 scope_guard(const scope_guard&) = delete;
#endif
 scope_guard(scope_guard&& rhs) : f_(std::move(rhs.f_)) , active_(rhs.active_) { rhs.dismiss(); }
 scope_guard& operator=(const scope_guard&) = delete;
 scope_guard& operator=(scope_guard&& rhs) { using std::swap; swap(f_,rhs.f_); swap(active_, rhs.active_); return *this;}
};

template <class Fun> scope_guard<Fun> scopeGuard(Fun f) { return scope_guard<Fun>(std::move(f)); }
}}
#endif
