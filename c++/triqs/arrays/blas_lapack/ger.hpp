// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Philipp D, Olivier Parcollet, Nils Wentzell

#pragma once

#include "f77/cxx_interface.hpp"
#include "tools.hpp"
#include "qcache.hpp"

namespace triqs::arrays::blas {

  using namespace blas_lapack_tools;

  /**
  * Calls ger : A += alpha * x * ty
  * Takes care of making temporary copies if necessary
  */
  template <typename VTX, typename VTY, typename MT>
  typename std::enable_if<is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type<VTX, VTY, MT>::value>::type
  ger(typename VTX::value_type alpha, VTX const &X, VTY const &Y, MT &A) {
    static_assert(is_amv_value_or_view_class<MT>::value, "ger : A must be a matrix or a matrix_view");
    if ((first_dim(A) != Y.size()) || (second_dim(A) != X.size()))
      TRIQS_RUNTIME_ERROR << "Dimension mismatch in ger : A : " << get_shape(A()) << " while X : " << get_shape(X()) << " and Y : " << get_shape(Y());
    const_qcache<VTX> Cx(X); // mettre la condition a la main
    const_qcache<VTY> Cy(Y); // mettre la condition a la main
    reflexive_qcache<MT> Ca(A);
    if (Ca().memory_layout_is_c()) // tA += alpha y tx
      f77::ger(get_n_rows(Ca()), get_n_cols(Ca()), alpha, Cy().data_start(), Cy().stride(), Cx().data_start(), Cx().stride(), Ca().data_start(),
               get_ld(Ca()));
    else
      f77::ger(get_n_rows(Ca()), get_n_cols(Ca()), alpha, Cx().data_start(), Cx().stride(), Cy().data_start(), Cy().stride(), Ca().data_start(),
               get_ld(Ca()));

    /* std::cerr << " Meme labout  C"<< Ca().memory_layout_is_c()  << "  "<<A.memory_layout_is_c()<<std::endl ;
   std::cerr<< " has_contiguous_data(A) : "<< has_contiguous_data(A) << std::endl;
   std::cerr<< Ca()<< std::endl;
   std::cerr<< Ca()(0,0) <<  "  "<< Ca()(1,0) <<  "  "<< Ca()(0,1) <<  "  "<< Ca()(1,1) <<  "  "<< std::endl;
   std::cerr<< Ca().data_start()[0]<< "  "<< Ca().data_start()[1]<< "  "<< Ca().data_start()[2]<< "  " << Ca().data_start()[3]<< "  "<<std::endl;

   std::cerr<< A<< std::endl;
   std::cerr<< A(0,0) <<  "  "<< A(1,0) <<  "  "<< A(0,1) <<  "  "<< A(1,1) <<  "  "<< std::endl;
   std::cerr<< A.data_start()[0]<< "  "<< A.data_start()[1]<< "  "<< A.data_start()[2]<< "  " << A.data_start()[3]<< "  "<<std::endl;
*/
  }

  // to allow ger (alpha, x,y, M(..., ...)) i.e. a temporary view, which is not matched by previos templates
  // which require an lvalue
  template <typename A, typename VTX, typename VTY, typename V, char B_S>
  void ger(A alpha, VTX const &x, VTY const &y, matrix_view<V, B_S> &&r) {
    ger(alpha, x, y, r);
  }

} // namespace triqs::arrays::blas
