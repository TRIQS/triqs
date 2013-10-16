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
#ifndef TRIQS_GF_FREQ_H
#define TRIQS_GF_FREQ_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/R.hpp"
#include "./meshes/linear.hpp"
#include "./evaluators.hpp"

namespace triqs { namespace gfs {

 struct refreq {};

 template<typename Opt> struct gf_mesh<refreq,Opt> : linear_mesh<R_domain> {
  typedef linear_mesh<R_domain> B;
  gf_mesh() = default;
  gf_mesh (double wmin, double wmax, int n_freq, mesh_kind mk=full_bins) : 
   B(typename B::domain_t(), wmin, wmax, n_freq, mk){}
 };

 namespace gfs_implementation { 

  // singularity 
  template<typename Opt> struct singularity<refreq,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<refreq,scalar_valued,Opt>  { typedef local::tail type;};

  // h5 name
  template<typename Opt> struct h5_name<refreq,matrix_valued,Opt>      { static std::string invoke(){ return "ReFreq";}};

  /// ---------------------------  evaluator ---------------------------------

  template<> struct evaluator_fnt_on_mesh<refreq> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_linear_interpolation);

  template<typename Opt, typename Target> struct evaluator<refreq,Target,Opt> : evaluator_one_var<refreq>{};


  /// ---------------------------  data access  ---------------------------------
  template<typename Opt> struct data_proxy<refreq,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt> struct data_proxy<refreq,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

 }
}}
#endif

