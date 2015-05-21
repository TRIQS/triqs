/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
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
#include <triqs/arrays.hpp>
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <boost/operators.hpp>
#include "hilbert_space.hpp"
#include <triqs/utility/draft/numeric_ops.hpp>

namespace triqs {
namespace hilbert_space {

// States of a Hilbert space : can either be described by a map
// or by a triqs::vector so there are two implementations controlled by BasedOnMap
template <typename HilbertSpace, typename ScalarType, bool BasedOnMap> class state {};

template <typename HilbertSpace, typename ScalarType, bool BasedOnMap>
state<HilbertSpace, ScalarType, BasedOnMap> make_zero_state(state<HilbertSpace, ScalarType, BasedOnMap> const& st) {
 return {st.get_hilbert()};
}

// -----------------------------------------------------------------------------------
// implementation based on a map : can work
// on huge hilbert spaces as long as there are not too
// many components in the state and not too many monomials
//  in the operator acting on the state...
// -----------------------------------------------------------------------------------
template <typename HilbertSpace, typename ScalarType>
class state<HilbertSpace, ScalarType, true> : boost::additive<state<HilbertSpace, ScalarType, true>>,
                                              boost::multiplicative<state<HilbertSpace, ScalarType, true>, ScalarType> {
 // derivations implement the vector space operations over ScalarType from the compounds operators +=, *=, ....
 const HilbertSpace* hs;
 using amplitude_t = std::unordered_map<std::size_t, ScalarType>;
 amplitude_t ampli;

 public:
 using value_type = ScalarType;
 using hilbert_space_t = HilbertSpace;

 state() : hs(nullptr) {} // non valid state !
 state(HilbertSpace const& hs_) : hs(&hs_) {}

 // Dimension of the Hilbert space
 // What if hs == nullptr ?
 int size() const { return hs->size(); }

 // Access to data
 value_type& operator()(int i) { return ampli[i]; }
 value_type const& operator()(int i) const { return ampli[i]; }

 // Basic operations
 state& operator+=(state const& s2) {
  for (auto const& aa : s2.ampli) {
   auto r = ampli.insert(aa);
   if (!r.second) r.first->second += aa.second;
  }
  prune();
  return *this;
 }

 state& operator-=(state const& s2) {
  for (auto const& aa : s2.ampli) {
   auto r = ampli.insert({aa.first, -aa.second});
   if (!r.second) r.first->second -= aa.second;
  }
  prune();
  return *this;
 }

 state& operator*=(value_type x) {
  for (auto& a : ampli) {
   a.second *= x;
  }
  prune();
  return *this;
 }

 state& operator/=(value_type x) { return operator*=(1 / x); }

 // Scalar product
 friend value_type dot_product(state const& s1, state const& s2) {
  value_type res = 0.0;
  for (auto const& a : s1.ampli) {
   if (s2.ampli.count(a.first) == 1) res += conj_(a.second) * s2.ampli.at(a.first);
  }
  return res;
 }

 // Lambda (fs, amplitude)
 template<typename Lambda>
 friend void foreach(state const& st, Lambda l) {
  const_cast<state&>(st).prune();
  for (auto const& p : st.ampli) l(p.first, p.second);
 }

 //
 // Additions to StateVector concept
 //

 HilbertSpace const& get_hilbert() const { return *hs; }
 void set_hilbert(HilbertSpace const& hs_) { hs = &hs_; }

 private:
 void prune() {
  for(auto it=ampli.begin(); it!=ampli.end();)
  {
   if (triqs::utility::is_zero(it->second))
    it = ampli.erase(it);
   else
    ++it;
  }
 }
};

// -----------------------------------------------------------------------------------
// implementation based on a vector
// -----------------------------------------------------------------------------------
template <typename HilbertSpace, typename ScalarType>
class state<HilbertSpace, ScalarType, false> : boost::additive<state<HilbertSpace, ScalarType, false>>,
                                               boost::multiplicative<state<HilbertSpace, ScalarType, false>, ScalarType> {

 const HilbertSpace* hs;
 using amplitude_t = triqs::arrays::vector<ScalarType>;
 amplitude_t ampli;

 public:
 using value_type = ScalarType;
 using hilbert_space_t = HilbertSpace;

 state() : hs(nullptr) {}
 state(HilbertSpace const& hs_) : hs(&hs_), ampli(hs_.size(), 0.0) {}

 // Dimension of the Hilbert space
 // What if hs == nullptr ?
 int size() const { return hs->size(); }

 // Access to data
 value_type& operator()(int i) { return ampli[i]; }
 value_type const& operator()(int i) const { return ampli[i]; }

 // Basic operations
 state& operator+=(state const& s2) {
  ampli += s2.ampli;
  return *this;
 }

 state& operator-=(state const& s2) {
  ampli -= s2.ampli;
  return *this;
 }

 state& operator*=(value_type x) {
  ampli *= x;
  return *this;
 }

 state& operator/=(value_type x) {
  ampli /= x;
  return *this;
 }

 // Scalar product
 friend value_type dot_product(state const& s1, state const& s2) { return dotc(s1.ampli, s2.ampli); }

 // Lambda (fs, amplitude)
 template<typename Lambda>
 friend void foreach(state const& st, Lambda l) {
  const auto L = st.size();
  for (size_t i = 0; i < L; ++i) l(i, st(i));
 }

 //
 // Additions to StateVector concept
 //

 // Full access to amplitudes
 amplitude_t const& amplitudes() const { return ampli; }
 amplitude_t& amplitudes() { return ampli; }

 // Get access to Hilbert space
 HilbertSpace const& get_hilbert() const { return *hs; }
 void set_hilbert(HilbertSpace const& hs_) { hs = &hs_; }

};

// Print state
template <typename HilbertSpace, typename ScalarType, bool BasedOnMap>
std::ostream& operator<<(std::ostream& os, state<HilbertSpace, ScalarType, BasedOnMap> const& s) {
 bool something_written = false;
 auto const& hs = s.get_hilbert();

 using value_type = typename state<HilbertSpace, ScalarType, BasedOnMap>::value_type;
 foreach(s, [&os,hs,&something_written](int i, value_type ampl){
  if (!triqs::utility::is_zero(ampl)){
   os << " +(" << ampl << ")" << "|" << hs.get_fock_state(i) << ">";
   something_written = true;
  }
 });

 if (!something_written) os << 0;
 return os;
}

#define TRIQS_WORKAROUND_CLANG34_BUG
#ifndef TRIQS_WORKAROUND_CLANG34_BUG
template<typename TargetState, typename OriginalState>
TargetState project(OriginalState const& psi, hilbert_space const& proj_hs) {
 TargetState proj_psi(proj_hs);
 auto const& hs = psi.get_hilbert();
 foreach(psi,[&](int i, typename OriginalState::value_type v){
  proj_psi(hs.get_fock_state(i)) = v;
 });
 return proj_psi;
}

template<typename TargetState, typename OriginalState>
TargetState project(OriginalState const& psi, sub_hilbert_space const& proj_hs) {
 TargetState proj_psi(proj_hs);
 auto const& hs = psi.get_hilbert();
 foreach(psi,[&](int i, typename OriginalState::value_type v){
  auto f = hs.get_fock_state(i);
  if(proj_hs.has_state(f)) proj_psi(proj_hs.get_state_index(f)) = v;
 });
 return proj_psi;
}
#else
// workaround for clang 3.4. Bug 
// Not needed for clang 3.5
template<typename A, typename B> struct __lambda1 { 
 A& proj_psi; B const & hs;
 template<typename VT> void operator()(int i, VT const & v) { 
 proj_psi(hs.get_fock_state(i)) = v; 
 }
};
template<typename A, typename B, typename C> struct __lambda2 { 
 A& proj_psi; B const & proj_hs; C const & hs;
 template<typename VT> void operator()(int i, VT const & v) { 
  auto f = hs.get_fock_state(i);
  if (proj_hs.has_state(f)) proj_psi(proj_hs.get_state_index(f)) = v;
 }
};

template<typename TargetState, typename OriginalState>
TargetState project(OriginalState const& psi, hilbert_space const& proj_hs) {
 TargetState proj_psi(proj_hs);
 auto const& hs = psi.get_hilbert();
 auto f = __lambda1<TargetState,hilbert_space> {proj_psi, hs};
 foreach(psi, f);
 return proj_psi;
}

template<typename TargetState, typename OriginalState>
TargetState project(OriginalState const& psi, sub_hilbert_space const& proj_hs) {
 TargetState proj_psi(proj_hs);
 auto const& hs = psi.get_hilbert();
 auto f = __lambda2<TargetState,sub_hilbert_space,hilbert_space> {proj_psi, proj_hs, hs};
 foreach(psi,f);
 return proj_psi;
}

#endif
}}
