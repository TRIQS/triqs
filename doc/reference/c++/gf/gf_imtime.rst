.. highlight:: c

.. _gf_imtime: 

gf<imtime> 
===================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara time.
 
Domain & mesh
----------------

Singularity
-------------

Interpolation method
---------------------

None.

Data storage
---------------

* `data_t` : 3d array (C ordered) of complex<double>.

* g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.


HDF5 storage convention
---------------------------

h5 tag : `ImTime`


Examples
---------

.. compileblock::

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; using triqs::clef::placeholder;
    int main(){
     double beta=10, a = 1;
     int n_times=5;

     // First give information to build the mesh, second to build the target
     auto GF1  = gf<imtime> { {beta,Fermion,n_times}, {1,1} };
     // or a more verbose/explicit form ...
     auto GF2  = gf<imtime> { gf_mesh<imtime>{beta,Fermion,n_times}, make_shape(1,1) };
 
     // Filling the gf with something...
     placeholder<0> tau_;
     //GF1(tau_) << exp ( - a * tau_) / (1 + exp(- beta * a));

     // evaluation at n=3
     std::cout << make_matrix(GF1(3)) << " == "<<   exp ( - a * 3) / (1 + exp(- beta * a)) << std::endl;
    }




