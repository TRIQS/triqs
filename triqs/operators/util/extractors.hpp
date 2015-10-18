/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by T. Ayral, I. Krivenko
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
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/arrays.hpp>

namespace triqs {
namespace operators {
namespace util {

template<typename scalar_t> using op_t = utility::many_body_operator<scalar_t>;
template<typename scalar_t> using dict2_t = std::map<std::tuple<typename op_t<scalar_t>::indices_t,
                                                                typename op_t<scalar_t>::indices_t>,
                                                       scalar_t>;
template<typename scalar_t> using dict4_t = std::map<std::tuple<typename op_t<scalar_t>::indices_t,
                                                                typename op_t<scalar_t>::indices_t,
                                                                typename op_t<scalar_t>::indices_t,
                                                                typename op_t<scalar_t>::indices_t>,
                                                       scalar_t>;

template<typename DictType> using matrix_t = triqs::arrays::array<typename DictType::mapped_type,
                                                                  std::tuple_size<typename DictType::key_type>::value>;
using gf_struct_t = std::map<std::string, hilbert_space::fundamental_operator_set::indices_t>;

/// Return dictionary of quadractic term coefficients starting from a Hamiltonian
/**
  * throw exception if non-quadratic terms are met
  * otherwise return std::map<std::tuple<op::indices_t,op::indices_t>, scalar_t>
  */
template<typename scalar_t>
dict2_t<scalar_t> extract_h_dict(op_t<scalar_t> const & h, bool ignore_irrelevant = false){

 auto h_dict = dict2_t<scalar_t>{};

 for(auto const & term : h){
  auto const& coef = term.coef;
  auto const& m = term.monomial;

  if(m.size()==2){
   if( !(m[0].dagger && !m[1].dagger)){
    if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_h_dict: monomial is not of the form C^+(i) C(j)";
   }
   else{//everything ok
    h_dict.insert({std::make_tuple(m[0].indices, m[1].indices),coef});
   }
  }
  else{
   if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_h_dict: monomial must have 2 operators";
  }
 }

 return h_dict;
}

/// Return dictionary of density-density interactions starting from a Hamiltonian
/**
  * throw exception if non-density-density terms are met
  * otherwise return std::map<std::tuple<op::indices_t,op::indices_t>, scalar_t>
  */
template<typename scalar_t>
dict2_t<scalar_t> extract_U_dict2(op_t<scalar_t> const & h, bool ignore_irrelevant = false){

 auto U_dict = dict2_t<scalar_t>{};

 for(auto const & term : h){
  auto const& coef = term.coef;
  auto const& m = term.monomial;

  if(m.size()==4){
   if( !(m[0].dagger && m[1].dagger && !m[2].dagger && !m[3].dagger)
     || (m[0].indices != m[3].indices)
     || (m[1].indices != m[2].indices)){
    if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict2: monomial is not of the form C^+(i) C^+(j) C(j) C(i)";
   }
   else{//everything ok
    U_dict.insert({std::make_tuple(m[0].indices, m[1].indices),coef});
    U_dict.insert({std::make_tuple(m[1].indices, m[0].indices),coef});
   }
  }
  else{
   if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict2: monomial must have 4 operators";
  }
 }

 return U_dict;
}

/// Return dictionary of quartic interactions starting from a Hamiltonian
/**
  * throw exception if non-quartic terms are met
  * otherwise return std::map<std::tuple<op::indices_t,op::indices_t,op::indices_t,op::indices_t>, scalar_t>
  */
template<typename scalar_t>
dict4_t<scalar_t> extract_U_dict4(op_t<scalar_t> const & h, bool ignore_irrelevant = false){

 auto U_dict = dict4_t<scalar_t>{};

 for(auto const & term : h){
  scalar_t const& coef = term.coef;
  auto const& m = term.monomial;

  if(m.size()==4){
   if( !(m[0].dagger && m[1].dagger && !m[2].dagger && !m[3].dagger)){
    if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict4: monomial is not of the form C^+(i) C^+(j) C(l) C(k)";
   }
   else{//everything ok
    U_dict.insert({std::make_tuple(m[0].indices, m[1].indices, m[3].indices, m[2].indices),scalar_t(0.5)*coef});
    U_dict.insert({std::make_tuple(m[1].indices, m[0].indices, m[2].indices, m[3].indices),scalar_t(0.5)*coef});
    U_dict.insert({std::make_tuple(m[0].indices, m[1].indices, m[2].indices, m[3].indices),scalar_t(-0.5)*coef});
    U_dict.insert({std::make_tuple(m[1].indices, m[0].indices, m[3].indices, m[2].indices),scalar_t(-0.5)*coef});
   }
  }
  else{
   if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict4: monomial must have 4 operators";
  }
 }

 return U_dict;
}

/// Converts dictionary of interactions to matrix, given a fundamental operator set
template<typename DictType>
matrix_t<DictType> dict_to_matrix(DictType const& dict,
                                  hilbert_space::fundamental_operator_set const& fs){

 using namespace triqs::tuple;
 using key_t = typename DictType::key_type;
 using scalar_t = typename DictType::mapped_type;
 using indices_t = typename std::tuple_element<0,key_t>::type;
 constexpr int rank = std::tuple_size<key_t>::value;

 auto mat = apply_construct_parenthesis<matrix_t<DictType>>(make_tuple_repeat<rank>(fs.size()));
 mat() = scalar_t{};

 auto indices_to_linear = [&fs](indices_t const& indices) {
  if (!fs.has_indices(indices))
   TRIQS_RUNTIME_ERROR << "dict_to_matrix: key [" << indices << "] of dict not in fundamental_operator_set/gf_struct";
  return fs[indices];
 };

 for(auto const& kv : dict){
  apply(mat,map(indices_to_linear,kv.first)) = kv.second;
 }

 return mat;
}

/// Converts dictionary of interactions to matrix, given the structure of the gf
template<typename DictType>
matrix_t<DictType> dict_to_matrix(DictType const& dict, gf_struct_t const& gf_struct){
 hilbert_space::fundamental_operator_set fs;
 for(auto const& block : gf_struct){
  for(auto const& inner : block.second){
   fs.insert(block.first, inner);
  }
 }
 return dict_to_matrix(dict,fs);
}

}}}
