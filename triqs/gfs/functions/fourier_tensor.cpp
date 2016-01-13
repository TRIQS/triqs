/****************************************
 *  Copyright (C) 2015 by Thomas Ayral  *
 ****************************************/
#include "./fourier_tensor.hpp"
namespace triqs { namespace gfs{

 gf<imfreq, tensor_valued<3>, nothing> fourier(gf_const_view<imtime, tensor_valued<3>, nothing> g_in, array_const_view<tail, 3> tail, int n_pts, bool positive_frequencies_only){

  auto g_out = gf<imfreq, tensor_valued<3>, nothing>({g_in.mesh().domain().beta, g_in.mesh().domain().statistic, n_pts, positive_frequencies_only? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies}, get_target_shape(g_in));
  auto g_out_single = gf<imfreq, scalar_valued>(g_out.mesh());
  auto g_in_single = gf<imtime, scalar_valued>(g_in.mesh());
  for(int a=0;a<get_target_shape(g_in)[0];a++){
   for(int b=0;b<get_target_shape(g_in)[1];b++){
    for(int c=0;c<get_target_shape(g_in)[2];c++){
     for(auto const & om: g_in_single.mesh())
      g_in_single[om] = g_in[om](a,b,c);
     g_in_single.singularity() = tail(a,b,c);
     g_out_single() = fourier(g_in_single);
     for(auto const & om: g_out_single.mesh())
      g_out[om](a,b,c) = g_out_single[om];
    }//c
   }//b
  }//a

  return g_out;
 }

 gf<imtime, tensor_valued<3>, nothing> inverse_fourier(gf_const_view<imfreq, tensor_valued<3>, nothing> g_in, array_const_view<tail, 3> tail, int n_tau){

  auto g_out = gf<imtime, tensor_valued<3>, nothing>({g_in.mesh().domain().beta, g_in.mesh().domain().statistic, n_tau}, get_target_shape(g_in));
  auto g_out_single = gf<imtime, scalar_valued>(g_out.mesh());
  auto g_in_single = gf<imfreq, scalar_valued>(g_in.mesh());
  for(int a=0;a<get_target_shape(g_in)[0];a++){
   for(int b=0;b<get_target_shape(g_in)[1];b++){
    for(int c=0;c<get_target_shape(g_in)[2];c++){
     for(auto const & om: g_in_single.mesh())
      g_in_single[om] = g_in[om](a,b,c);
     g_in_single.singularity() = tail(a,b,c);
     g_out_single() = inverse_fourier(g_in_single);
     for(auto const & om: g_out_single.mesh())
      g_out[om](a,b,c) = g_out_single[om];
    }//c
   }//b
  }//a

  return g_out;
 }
}}
