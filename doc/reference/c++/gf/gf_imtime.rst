.. highlight:: c

.. _gf_imtime: 

Matsubara imaginary time
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for imaginary Matsubara time.

Synopsis
------------

.. code::

  gf<imtime, Target, Opt>

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

The domain is  the set of real numbers between 0 and :math:`\beta`
since the function is periodic (resp. antiperiodic) for bosons (resp. fermions),  i.e.

* :math:`G(\tau+\beta)=-G(\tau)`  for fermions
* :math:`G(\tau+\beta)=G(\tau)` for bosons.

The domain is implemented in the class :doxy:`matsubara_time_domain<triqs::gfs::matsubara_domain>`.

The mesh is :doxy:`matsubara_time_mesh<triqs::gfs::matsubara_time_mesh>`.


Singularity
-------------

The singularity is a high frequency expansion,  :ref:`gf_tail`.


Evaluation method
---------------------

* Use a linear interpolation between the two closest point of the mesh.

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


TO DO : complex OR DOUBLE : FIX and document !!

HDF5 storage convention
---------------------------

h5 tag : `ImTime`


Examples
---------

.. compileblock::

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; 
    int main(){
     double beta=10, a = 1;
     int n_times=1000;

     // --- first a matrix_valued function ------------

     // First give information to build the mesh, second to build the target
     auto g1  = gf<imtime, matrix_valued, no_tail> { {beta,Fermion,n_times}, {1,1} };
     
     // or a more verbose/explicit form ...
     auto g2  =  gf<imtime> { gf_mesh<imtime>{beta,Fermion,n_times}, make_shape(1,1) };

     // Filling the gf with something... COMMENT HERE : ok only because of no_tail
     triqs::clef::placeholder<0> tau_;
     g1(tau_) << exp ( - a * tau_) / (1 + exp(- beta * a));

     // evaluation at tau=3.2
     std::cout << triqs::arrays::make_matrix(g1(3.2)) << " == "<<   exp ( - a * 3.2) / (1 + exp(- beta * a)) << std::endl;

     // --- a scalar_valued function ------------
 
     // same a before, but without the same of the target space ...
     auto g3  = gf<imtime, scalar_valued, no_tail> { {beta,Fermion,n_times} };

     g3(tau_) << exp ( - a * tau_) / (1 + exp(- beta * a));

     // evaluation at tau=3.2
     std::cout << g3(3.2) << " == "<<   exp ( - a * 3.2) / (1 + exp(- beta * a)) << std::endl;
    }





