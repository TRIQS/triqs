.. highlight:: c

.. _gf_imfreq: 

gf<imfreq>
==========================================================


This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara frequencies.
 
Domain & mesh
----------------


Singularity
-------------

:ref:`gf_tail`.


Interpolation method
---------------------

None

Data storage
---------------

* `data_t` : 3d array (C ordered) of complex<double>.

* g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.

HDF5 storage convention
---------------------------

h5 tag : `ImFreq`


Examples
---------


.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; using triqs::clef::placeholder;
    int main(){
     double beta=10;
     int Nfreq =100;

     // First give information to build the mesh, second to build the target
     auto GF1  = gf<imfreq> { {beta,Fermion,Nfreq}, {1,1} };
     // or a more verbose/explicit form ...
     auto GF2  = gf<imfreq> { gf_mesh<imfreq>{beta,Fermion,Nfreq}, make_shape(1,1) };

     // Filling the gf with something...
     placeholder<0> wn_;
     GF1(wn_) << 1/ (wn_ + 2);

     // evaluation at n=3
     std::cout << GF1(3) << " == "<<  1/ ( 1_j * std::acos(-1) / beta  * (2*3+1) + 2) << std::endl;
     // the high frequency expansion was automatically computed.
     std::cout << GF1.singularity() << std::endl;
    }



