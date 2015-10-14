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
#include <tuple>
#include <triqs/utility/first_include.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/arrays.hpp>

namespace triqs {
namespace operators {
namespace util {

template<typename scalar_t> using op_t = utility::many_body_operator<scalar_t>;
template<typename scalar_t> using U_dict2_t = std::map<std::tuple<typename op_t<scalar_t>::indices_t,
                                                                  typename op_t<scalar_t>::indices_t>,
                                                       scalar_t>;

using gf_struct_t = std::map<std::string, hilbert_space::fundamental_operator_set::indices_t> ;

/// Return dictionary of density-density interactions starting from a Hamiltonian
/**
  * throw exception if non-density-density terms
  * otherwise return std::map<std::tuple<op::indices_t,op::indices_t>, scalar_t>
  */
template<typename scalar_t>
U_dict2_t<scalar_t> extract_U_dict2(op_t<scalar_t> const & h, bool ignore_irrelevant = false){

 auto U_dict = U_dict2_t<scalar_t>{};

 for(auto const & term : h){
  scalar_t coef = term.coef;
  auto monomial = term.monomial;

  if(monomial.size()==4){
   if( !(monomial[0].dagger && monomial[1].dagger && !monomial[2].dagger && !monomial[3].dagger)
     || (monomial[0].indices != monomial[3].indices)
     || (monomial[1].indices != monomial[2].indices)){
    if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "monomial is not of the form c_dag(i) c_dag(j) c(j) c(i)";
   }
   else{//everything ok
    U_dict.insert({std::make_tuple(monomial[0].indices, monomial[1].indices),coef});
    U_dict.insert({std::make_tuple(monomial[1].indices, monomial[0].indices),coef});
   }
  }
  else{
   if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "monomial must have 4 operators";
  }
 }

 return U_dict;
}

///converts dictionary of interactions to matrix, given the structure of the gf
template<typename scalar_t>
triqs::arrays::array<scalar_t,2> dict_to_matrix(U_dict2_t<scalar_t> const& U_dict,
                                                hilbert_space::fundamental_operator_set const& fs){

 triqs::arrays::array<scalar_t,2> U_matrix(fs.size(),fs.size());
 U_matrix() = scalar_t{};

 for(auto const & kv : U_dict){
  auto const& indices1 = std::get<0>(kv.first);
  auto const& indices2 = std::get<1>(kv.first);

  if (!fs.has_indices(indices1)) TRIQS_RUNTIME_ERROR << "key [" << indices1 << "] of U_dict not in gf_struct";
  if (!fs.has_indices(indices2)) TRIQS_RUNTIME_ERROR << "key [" << indices2 << "] of U_dict not in gf_struct";
  U_matrix(fs[indices1],fs[indices2]) = kv.second;
 }
 return U_matrix;
}

template<typename scalar_t>
triqs::arrays::array<scalar_t,2> dict_to_matrix(U_dict2_t<scalar_t> const& U_dict,
                                                gf_struct_t const& gf_struct){
 hilbert_space::fundamental_operator_set fs;
 for(auto const& block : gf_struct){
  for(auto const& inner : block.second){
   fs.insert(block.first, inner);
  }
 }
 return dict_to_matrix(U_dict,fs);
}

}}}
