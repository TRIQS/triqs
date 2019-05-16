..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block_gf__operator=:

triqs::gfs::block_gf::operator=
===============================

*#include <triqs/gfs/block_gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | block_gf<Var, Target> & :red:`operator=` (block_gf<Var, Target> & :param:`rhs`)

    2. | block_gf<Var, Target> & :red:`operator=` (block_gf<Var, Target> && :param:`rhs`)

    3. | block_gf<Var, Target> & :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, block_gf\<Var, Target\>::const_view_type\> <triqs__gfs__mpi_lazy>` :param:`l`)

    4. | :green:`template<typename RHS>`
       | block_gf<Var, Target> & :red:`operator=` (RHS && :param:`rhs`)

Documentation



 **1)**   Copy assignment



 **2)**   Move assignment



 **3)**   Assignment operator overload specific for mpi_lazy objects (keep before general assignment)



 **4)**   Assignment operator





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`RHS` Type of the right hand side rhs

             RHS can be anything modeling the gf concept TBW
             In particular lazy expression with Green functions


Parameters
^^^^^^^^^^

 * :param:`l` The lazy object returned by reduce

 * :param:`rhs` The assignment resizes the mesh and the data, invalidating all pointers on them.