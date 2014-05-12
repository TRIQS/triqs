.. highlight:: c

.. _gf_imfreq: 

Green function on Matsubara frequencies
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for imaginary Matsubara frequencies.

Synopsis
------------

.. code::

  gf<imfreq, Target, Opt>

The *Target* template parameter can take the following values : 
 
+-------------------------+-----------------------------------------------------+
| Target                  | Meaning                                             |
+=========================+=====================================================+
| scalar_valued           | The function is scalar valued (double, complex...). |
+-------------------------+-----------------------------------------------------+
| matrix_valued [default] | The function is matrix valued.                      |
+-------------------------+-----------------------------------------------------+

Domain & mesh
----------------

The domain is :doxy:`matsubara_freq_domain<triqs::gfs::matsubara_domain>`.

The Matsubara frequencies are :math:`\omega_n=\frac{(2n+1)\pi}{\beta}` for fermions and :math:`\omega_n=\frac{2n\pi}{\beta}` for bosons. 

The mesh is :doc:`matsubara_freq_mesh<matsubara_freq_mesh>`. 


Singularity
-------------

The singularity is a high frequency expansion,  :ref:`gf_tail`.


Evaluation method
---------------------

* No interpolation.

* Return type : 

  * If Target==scalar_valued : a complex 
  * If Target==matrix_valued : an object modeling ImmutableMatrix concept.

* When the point is outside of the mesh, the evaluation of the gf returns : 

  * the evaluation of the high frequency tail if no_tail is not set.
  * 0 otherwise


Data storage
---------------

* If Target==scalar_valued :
  
  * `data_t` : 1d array of complex<double>.

  * g.data()(i) is the value of g for the i-th point of the mesh.

* If Target==matrix_valued :

  * `data_t` : 3d array (C ordered) of complex<double>.

  * g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.

HDF5 storage convention
---------------------------

h5 tag : `ImFreq`

TODO : DECIDE if we have 2 tag, one for scalar, one for matrix....

Examples
---------


.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; 
    int main(){
     double beta=10;
     int Nfreq =100;

     // --- first a matrix_valued function ------------

     // First give information to build the mesh, second to build the target
     auto g1  = gf<imfreq> { {beta,Fermion,Nfreq}, {1,1} };
     // or a more verbose/explicit form ...
     auto g2  = gf<imfreq> { gf_mesh<imfreq>{beta,Fermion,Nfreq}, make_shape(1,1) };

     // Filling the gf with something...
     triqs::clef::placeholder<0> wn_;
     g1(wn_) << 1/ (wn_ + 2);

     // evaluation at n=3
     std::cout << g1(3) << " == "<<  1/ ( 1_j * M_PI / beta  * (2*3+1) + 2) << std::endl;
     // the high frequency expansion was automatically computed.
     //std::cout << g1.singularity() << std::endl; // a bit verbose..

     // --- a scalar_valued function ------------
 
     // same a before, but without the same of the target space ...
     auto g3  = gf<imfreq,scalar_valued> { {beta,Fermion,Nfreq} };
     auto g4  = gf<imfreq,scalar_valued> { gf_mesh<imfreq>{beta,Fermion,Nfreq} };

     g3(wn_) << 1/ (wn_ + 2);

     // evaluation at n=3. 
     std::cout << g3(3) << " == "<<  1/ ( 1_j * std::acos(-1) / beta  * (2*3+1) + 2) << std::endl;
    }


