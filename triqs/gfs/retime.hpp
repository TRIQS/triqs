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
#ifndef TRIQS_GF_ONE_REAL_TIME_H
#define TRIQS_GF_ONE_REAL_TIME_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/R.hpp"
#include "./meshes/linear.hpp"
#include "./evaluators.hpp"

namespace triqs { namespace gfs {

 struct retime {};

 template<typename Opt> struct gf_mesh<retime,Opt> : linear_mesh<R_domain>  { 
  typedef linear_mesh<R_domain> B;
  gf_mesh() = default;
  gf_mesh(double tmin, double tmax, int n_points, mesh_kind mk=full_bins) : B (typename B::domain_t(), tmin, tmax, n_points, mk){}
 };

 namespace gfs_implementation { 

  // singularity 
  template<typename Opt> struct singularity<retime,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<retime,scalar_valued,Opt>  { typedef local::tail type;};

  // h5 name
  template<typename Opt> struct h5_name<retime,matrix_valued,Opt>      { static std::string invoke(){ return  "ReTime";}};

  /// ---------------------------  evaluator ---------------------------------
 template<> struct evaluator_fnt_on_mesh<retime> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_linear_interpolation);

 template<typename Opt, typename Target> struct evaluator<retime,Target,Opt> : evaluator_one_var<retime>{};

  /// ---------------------------  data access  ---------------------------------
  template<typename Opt> struct data_proxy<retime,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt> struct data_proxy<retime,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

 } // gfs_implementation
}}
#endif

