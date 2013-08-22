.. highlight:: c

.. _gf_imtime: 

gf<imtime> 
===================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara time.
 
Domain & mesh
----------------


Singularity
-------------

Factories
-------------

Code ::
   
   
  make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t)
  make_gf(double beta, statistic_enum S,  tqa::mini_vector<size_t,2> shape, size_t Nmax=1025, mesh_kind mk= half_bins)
  make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape, size_t Nmax, mesh_kind mk, local::tail_view const & t)


Interpolation method
---------------------

Data storage
---------------


HDF5 storage convention
---------------------------



Examples
---------

.. compileblock:: 

    #include <triqs/gfs/imtime.hpp>
    using namespace triqs::gfs;

    int main() {
      double beta=1; //inverse temperature
      triqs::gfs::statistic_enum stat=triqs::gfs::Fermion;
      size_t n_times=5;
      auto shape = triqs::arrays::make_shape(1,1);
      auto GF=make_gf<imtime>(beta, stat, shape, n_times);  
    };



