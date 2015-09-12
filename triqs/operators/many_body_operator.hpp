/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by I. Krivenko, O. Parcollet, M. Ferrero
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
#include <triqs/hilbert_space/fundamental_operator_set.hpp>

#include <ostream>
#include <cmath>
#include <boost/operators.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <triqs/utility/draft/numeric_ops.hpp>
#include <triqs/utility/real_or_complex.hpp>
#include <triqs/h5.hpp>

namespace triqs {
namespace operators {

 using utility::variant_int_string;
 using utility::real_or_complex;

 /// The generic class
 template <typename scalar_t> class many_body_operator_generic;

 /// The indices of the C, C^+ operators are a vector of int/string
 using indices_t = std::vector<variant_int_string>;

 /// The user class
 using many_body_operator = many_body_operator_generic<real_or_complex>;
 inline std::string get_triqs_hdf5_data_scheme(many_body_operator const&) { return "Operator"; }

 //-----------------------------------------------------------------------------------------

 /// The canonical operator: a dagger and some indices
 struct canonical_ops_t {
  bool dagger;
  indices_t indices;
  // Order: dagger < non dagger, and then indices
  // Example: c+_1 < c+_2 < c+_3 < c_3 < c_2 < c_1
  friend bool operator<(canonical_ops_t const& a, canonical_ops_t const& b) {
   if (a.dagger != b.dagger) return (a.dagger > b.dagger);
   if (a.dagger) // a.indices < b.indices
    return std::lexicographical_compare(a.indices.begin(), a.indices.end(), b.indices.begin(), b.indices.end());
   else // b.indices < a.indices
    return std::lexicographical_compare(b.indices.begin(), b.indices.end(), a.indices.begin(), a.indices.end());
  }
  friend bool operator>(canonical_ops_t const& a, canonical_ops_t const& b) { return b < a; }
  friend bool operator==(canonical_ops_t const& a, canonical_ops_t const& b) {
   return (a.dagger == b.dagger && a.indices.size() == b.indices.size() &&
           std::equal(a.indices.begin(), a.indices.end(), b.indices.begin()));
  }

  template <class Archive> void serialize(Archive& ar, const unsigned int version) { ar& dagger& indices; }
 };

 std::ostream& operator<<(std::ostream& os, canonical_ops_t const& op) {
  if (op.dagger) os << "^+";
  os << "(";
  int u = 0;
  for (auto const& i : op.indices) {
   if (u++) os << ",";
   os << i;
  }
  return os << ")";
 }

 //-----------------------------------------------------------------------------------------
 // Monomial: an ordered set of creation/annihilation operators and comparison
 using monomial_t = std::vector<canonical_ops_t>;

 bool operator<(monomial_t const& m1, monomial_t const& m2) {
  return m1.size() != m2.size() ? m1.size() < m2.size()
                                : std::lexicographical_compare(m1.begin(), m1.end(), m2.begin(), m2.end());
 }

 std::ostream& operator<<(std::ostream& os, monomial_t const& m) {
  for (auto const& c : m) { os << "C" << c; }
  return os;
 }

 //-----------------------------------------------------------------------------------------
 /**
  * many_body_operator_generic is a general operator in second quantification
  */
 template <typename scalar_t>
 class many_body_operator_generic :
     // implements vector space over scalar_t operators
     boost::additive<many_body_operator_generic<scalar_t>>,
     boost::multipliable<many_body_operator_generic<scalar_t>>,
     boost::additive<many_body_operator_generic<scalar_t>, scalar_t>, // op+a a+op op-a
     // boost::subtractable2_left<many_body_operator_generic<scalar_t>, scalar_t>, // a-op
     boost::multipliable<many_body_operator_generic<scalar_t>, scalar_t>, // op*a a*op op/a
     boost::dividable<many_body_operator_generic<scalar_t>, scalar_t> {

  // Map of all monomials with coefficients
  using monomials_map_t = std::map<monomial_t, scalar_t>;

  monomials_map_t monomials;

  friend void h5_write(h5::group g, std::string const& name, many_body_operator_generic const& op);
  friend void h5_write(h5::group g, std::string const& name, many_body_operator_generic const& op,
                       hilbert_space::fundamental_operator_set const& fops);

  friend void h5_read(h5::group g, std::string const& name, many_body_operator_generic& op);
  friend void h5_read(h5::group g, std::string const& name, many_body_operator_generic& op,
                      hilbert_space::fundamental_operator_set& fops);

  public:
  many_body_operator_generic() = default;
  many_body_operator_generic(many_body_operator_generic const&) = default;
  many_body_operator_generic(many_body_operator_generic&&) = default;
  many_body_operator_generic& operator=(many_body_operator_generic const&) = default;
  many_body_operator_generic& operator=(many_body_operator_generic&&) = default;

  template <typename S> many_body_operator_generic(many_body_operator_generic<S> const& x) {
   static_assert(std::is_constructible<scalar_t, S>::value, "Construction is impossible");
   *this = x;
  }

  explicit many_body_operator_generic(scalar_t const& x) { monomials.insert({{}, x}); }

  template <typename S> many_body_operator_generic& operator=(many_body_operator_generic<S> const& x) {
   static_assert(std::is_constructible<scalar_t, S>::value, "Assignment is impossible");
   monomials.clear();
   for (auto const& y : x.get_monomials()) monomials.insert(std::make_pair(monomial_t{y.first}, scalar_t(y.second)));
   return *this;
  }

  // internal, for previous operator =
  monomials_map_t const & get_monomials() const { return monomials;}

  /// Make a minimal fundamental_operator_set with all the canonical operators of this
  hilbert_space::fundamental_operator_set make_fundamental_operator_set() const {
   hilbert_space::fundamental_operator_set fops;
   for (auto const& m : monomials)        // for all monomials of the operator
    for (auto const& c_cdag_op : m.first) // loop over the C C^+ operators of the monomial
     fops.insert_from_indices_t(c_cdag_op.indices);
   return fops;
  }

  // factory for c, cdag
  static many_body_operator_generic make_canonical(bool is_dag, indices_t indices) {
   many_body_operator_generic res;
   auto m = monomial_t{canonical_ops_t{is_dag, indices}};
   res.monomials.insert({m, scalar_t(1.0)});
   return res;
  }

  // We use utility::dressed_iterator to dress iterators
  // _cdress is a simple struct of refs to dress the iterators (Cf doc)
  struct _cdress {
   monomial_t const& monomial;
   scalar_t coef;
   _cdress(typename monomials_map_t::const_iterator _it) : monomial(_it->first), coef(_it->second) {}
   operator std::pair<std::vector<std::pair<bool,indices_t>>,scalar_t>() {
    std::vector<std::pair<bool,indices_t>> tmp_monomial;
    tmp_monomial.reserve(monomial.size());
    for(auto cop : monomial) tmp_monomial.emplace_back(cop.dagger,cop.indices);
    return {tmp_monomial,coef};
   }
  };
  using const_iterator = utility::dressed_iterator<typename monomials_map_t::const_iterator, _cdress>;

  public:
  // Iterators (only const!)
  const_iterator begin() const noexcept { return monomials.begin(); }
  const_iterator end() const noexcept { return monomials.end(); }
  const_iterator cbegin() const noexcept { return monomials.cbegin(); }
  const_iterator cend() const noexcept { return monomials.cend(); }

  // Is zero operator ?
  bool is_zero() const { return monomials.empty(); }

  // Algebraic operations involving scalar_t constants
  many_body_operator_generic operator-() const {
   auto res = *this;
   for (auto& m : res.monomials) m.second = -m.second;
   return res;
  }

  many_body_operator_generic& operator+=(scalar_t alpha) {
   bool is_new_monomial;
   typename monomials_map_t::iterator it;
   std::tie(it, is_new_monomial) = monomials.insert(std::make_pair(monomial_t(0), alpha));
   if (!is_new_monomial) {
    it->second += alpha;
    erase_zero_monomial(monomials, it);
   }
   return *this;
  }

  many_body_operator_generic& operator-=(scalar_t alpha) { return operator+=(-alpha); }

  friend many_body_operator_generic operator-(scalar_t alpha, many_body_operator_generic const& op) { return -op + alpha; }
  //friend many_body_operator_generic operator/ (many_body_operator_generic const & op, scalar_t alpha) { return op/alpha; }

  many_body_operator_generic& operator*=(scalar_t alpha) {
   using triqs::utility::is_zero;
   if (is_zero(alpha)) {
    monomials.clear();
   } else {
    for (auto& m : monomials) m.second *= alpha;
   }
   return *this;
  }

  many_body_operator_generic& operator/=(scalar_t alpha) { return operator*=(1.0/alpha); }

  // Algebraic operations
  many_body_operator_generic& operator+=(many_body_operator_generic const& op) {
   bool is_new_monomial;
   typename monomials_map_t::iterator it;
   for (auto const& m : op.monomials) {
    std::tie(it, is_new_monomial) = monomials.insert(m);
    if (!is_new_monomial) {
     it->second += m.second;
     erase_zero_monomial(monomials, it);
    }
   }
   return *this;
  }

  many_body_operator_generic& operator-=(many_body_operator_generic const& op) {
   bool is_new_monomial;
   typename monomials_map_t::iterator it;
   for (auto const& m : op.monomials) {
    std::tie(it, is_new_monomial) = monomials.insert(std::make_pair(m.first, -m.second));
    if (!is_new_monomial) {
     it->second -= m.second;
     erase_zero_monomial(monomials, it);
    }
   }
   return *this;
  }

  many_body_operator_generic& operator*=(many_body_operator_generic const& op) {
   monomials_map_t tmp_map; // product will be stored here
   for (auto const& m : monomials)
    for (auto const& op_m : op.monomials) {
     // prepare an unnormalized product
     monomial_t product_m;
     product_m.reserve(m.first.size() + op_m.first.size());
     for (auto const& op : m.first) product_m.push_back(op);
     for (auto const& op : op_m.first) product_m.push_back(op);
     // std::copy(m.first.begin(), m.first.end(), std::back_inserter(product_m));
     // std::copy(op_m.first.begin(), op_m.first.end(), std::back_inserter(product_m));
     normalize_and_insert(product_m, m.second * op_m.second, tmp_map);
    }
   std::swap(monomials, tmp_map);
   return *this;
  }

  // implementation details of dagger
  //
  private:

  static canonical_ops_t _dagger(canonical_ops_t const& cop) {
   return {!cop.dagger, cop.indices};
  }

  static monomial_t _dagger(monomial_t const& m) {
   monomial_t res;
   for (auto it = m.rbegin(); it != m.rend(); ++it) res.push_back(_dagger(*it));
   return res;
  }

  public:
  // dagger
  friend many_body_operator_generic dagger(many_body_operator_generic const& op) {
   many_body_operator_generic res;
   for (auto const& x : op) res.monomials.insert({_dagger(x.monomial), conj(x.coef)});
   return res;
  }

  // Boost.Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) { ar& monomials; }

  private:
  // Normalize a monomial and insert into a map
  static void normalize_and_insert(monomial_t& m, scalar_t coeff, monomials_map_t& target) {
   // The normalization is done by employing a simple bubble sort algorithms.
   // Apart from sorting elements this function keeps track of the sign and
   // recursively calls itself if a permutation of two operators produces a new
   // monomial
   if (m.size() >= 2) {
    bool is_swapped;
    do {
     is_swapped = false;
     for (std::size_t n = 1; n < m.size(); ++n) {
      canonical_ops_t& prev_index = m[n - 1];
      canonical_ops_t& cur_index = m[n];
      if (prev_index == cur_index) return; // The monomial is effectively zero
      if (prev_index > cur_index) {
       // Are we swapping C and C^+ with the same indices?
       // A bit ugly ...
       canonical_ops_t cur_index_flipped_type(cur_index);
       cur_index_flipped_type.dagger = !cur_index_flipped_type.dagger;
       if (prev_index == cur_index_flipped_type) {
        monomial_t new_m;
        new_m.reserve(m.size() - 2);
        std::copy(m.begin(), m.begin() + n - 1, std::back_inserter(new_m));
        std::copy(m.begin() + n + 1, m.end(), std::back_inserter(new_m));
        normalize_and_insert(new_m, coeff, target);
       }
       coeff = -coeff;
       std::swap(prev_index, cur_index);
       is_swapped = true;
      }
     }
    } while (is_swapped);
   }

   // Insert the result
   bool is_new_monomial;
   typename monomials_map_t::iterator it;
   std::tie(it, is_new_monomial) = target.insert(std::make_pair(m, coeff));
   if (!is_new_monomial) {
    it->second += coeff;
    erase_zero_monomial(target, it);
   }
  }

  // Erase a monomial with a close-to-zero coefficient.
  static void erase_zero_monomial(monomials_map_t& m, typename monomials_map_t::iterator& it) {
   using triqs::utility::is_zero;
   if (is_zero(it->second)) m.erase(it);
  }

  // Print many_body_operator_generic itself
  friend std::ostream& operator<<(std::ostream& os, many_body_operator_generic const& op) {
   if (op.monomials.size() != 0) {
    bool print_plus = false;
    for (auto const& m : op.monomials) {
     os << (print_plus ? " + " : "") << m.second;
     if (m.first.size()) os << "*";
     os << m.first;
     print_plus = true;
    }
   } else
    os << "0";
   return os;
  }
 };

 // ---- factories --------------

 // Free functions to make creation/annihilation operators
 template <typename scalar_t = real_or_complex, typename... IndexTypes> many_body_operator_generic<scalar_t> c(IndexTypes... indices) {
  return many_body_operator_generic<scalar_t>::make_canonical(false, indices_t{indices...});
  // need to put many_body_operator_generic<double>::indices_t because {} constructor is explicit !?
 }

 template <typename scalar_t = real_or_complex, typename... IndexTypes> many_body_operator_generic<scalar_t> c_dag(IndexTypes... indices) {
  return many_body_operator_generic<scalar_t>::make_canonical(true, indices_t{indices...});
 }

 template <typename scalar_t = real_or_complex, typename... IndexTypes> many_body_operator_generic<scalar_t> n(IndexTypes... indices) {
   return c_dag<scalar_t>(indices...) * c<scalar_t>(indices...);
  }
}
}
