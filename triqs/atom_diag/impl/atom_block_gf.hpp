/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
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

#include <string>
#include <vector>
#include <cmath>
#include <triqs/arrays/matrix.hpp>
#include <triqs/utility/legendre.hpp>
#include <triqs/gfs.hpp>
/*
namespace triqs {
namespace atom_diag {

using namespace triqs::gfs;
using triqs::arrays::matrix;
using triqs::arrays::vector;

// Green's function of a finite fermionic system
// Represented as a sum of resonant terms
class atom_block_gf {

 // One resonant term, residue / (i\omega - pole) in frequency domain
 struct term_t {
  double pole;
  std::complex<double> residue;

  using vector_t = vector<std::complex<double>>;

  term_t(double pole, std::complex<double> const& residue) : pole(pole), residue(residue) {}

  inline vector_t operator()(gf_mesh<imtime> const& mesh, double beta) const {
   vector_t res(mesh.size());
   auto w = -residue / (pole > 0 ? (1 + std::exp(-beta*pole)) : (std::exp(beta*pole) + 1));
   for(auto tau : mesh)
    res[tau.linear_index()] = w * (pole > 0 ? std::exp(-double(tau)*pole) :
                                              std::exp((beta-double(tau))*pole));
   return std::move(res);
  }
  inline vector_t operator()(gf_mesh<imfreq> const& mesh, double) const {
   vector_t res(mesh.size());
   for(auto iw : mesh) res[iw.linear_index()] = residue / (iw - pole);
   return std::move(res);
  }
  inline vector_t operator()(gf_mesh<legendre> const& mesh, double beta) const {
   vector_t res(mesh.size());
   double x = beta*pole/2;
   double w = -beta / (2 * std::cosh(x));
   for(auto l : mesh) {
    auto ll = int(l);
    res[l.linear_index()] = residue * w * std::sqrt(2*ll+1)
                                    * (ll%2==0 ? 1 : std::copysign(1,-x))
                                    * triqs::utility::mod_cyl_bessel_i(ll,std::abs(x));
   }
   return std::move(res);
  }
 };
 using terms_list_t = std::vector<term_t>;

 std::vector<std::string> block_names;
 std::vector<matrix<terms_list_t>> terms;
 double beta;

 void inline set_singularity(gf_view<imtime> g) const { g.singularity()(1) = 1.0; }
 void inline set_singularity(gf_view<imfreq> g) const { g.singularity()(1) = 1.0; }
 void inline set_singularity(gf_view<legendre> g) const {}

 template<typename... T>
 inline void operator()(gf_view<T...> g) const {
  if(g.mesh().domain().names() != block_names)
   TRIQS_RUNTIME_ERROR << "atom_block_gf: RHS has a different block structure";
  for(int bl : range(block_names.size())) {
   auto shape = get_target_shape(g[bl]);
   if(shape != terms[bl].shape())
    TRIQS_RUNTIME_ERROR << "atom_block_gf: RHS block '" << block_names[bl]
                        << "' has a wrong shape " << get_target_shape(g[bl]);
   auto g_block = g[bl];
   auto const& mesh = g_block.mesh();
   g_block() = 0;
   for(int i1 : range(shape[0]))
   for(int i2 : range(shape[1])) {
    vector_view<std::complex<double>> d = g_block.data()(range(), i1, i2);
    for(term_t const& term : terms[bl](i1,i2)) d += term(mesh, beta);
   }
   set_singularity(g_block);
  }
 }

public:

 atom_block_gf(double beta, gf_struct_t const& gf_struct) : beta(beta) {
  block_names.reserve(gf_struct.size());
  terms.reserve(gf_struct.size());
  for(auto const& b : gf_struct) {
   block_names.push_back(b.first);
   terms.emplace_back(b.second.size(), b.second.size());
  }
 }

 matrix<terms_list_t> & operator[](int bl) { return terms[bl]; }
 matrix<terms_list_t> const& operator[](int bl) const { return terms[bl]; }

 template<typename Tag>
 friend void triqs_gf_view_assign_delegation(block_gf_view<Tag> g, atom_block_gf const& agf) { agf(g); }
};

}}*/
