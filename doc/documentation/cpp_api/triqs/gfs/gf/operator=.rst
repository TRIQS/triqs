..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf__operator=:

triqs::gfs::gf::operator=
=========================

*#include <triqs/gfs/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | gf<Var, Target> & :red:`operator=` (gf<Var, Target> & :param:`rhs`)

    2. | gf<Var, Target> & :red:`operator=` (gf<Var, Target> const & :param:`rhs`)

    3. | gf<Var, Target> & :red:`operator=` (gf<Var, Target> && :param:`rhs`) noexcept

    4. | :green:`template<typename RHS>`
       | gf<Var, Target> & :red:`operator=` (RHS && :param:`rhs`)

    5. | void :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    6. | void :red:`operator=` (:ref:`mpi_lazy\<tag::scatter, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    7. | void :red:`operator=` (:ref:`mpi_lazy\<tag::gather, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

Documentation



 **4)**
The assignment resizes the mesh and the data, invalidating all pointers on them.



 **5)**   Performs MPI reduce



 **6)**   Performs MPI scatter



 **7)**   Performs MPI gather





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`RHS` Type of the right hand side rhs

 		  RHS can be anything modeling the gf concept TBW
 		  In particular lazy expression with Green functions


Parameters
^^^^^^^^^^

 * :param:`rhs`

 * :param:`l` The lazy object returned by mpi::reduce


.. error:: File not found

    Example file /mnt/home/wentzell/Dropbox/Coding/triqs/doc/documentation/examples/triqs/gfs/gf_assign_0.cpp not found