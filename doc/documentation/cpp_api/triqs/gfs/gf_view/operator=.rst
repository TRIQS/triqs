..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_view__operator=:

triqs::gfs::gf_view::operator=
==============================

*#include <triqs/gfs/gf_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | gf_view<Var, Target> & :red:`operator=` (gf_view<Var, Target> const & :param:`rhs`)

    2. | :green:`template<typename RHS>`
       | gf_view<Var, Target> & :red:`operator=` (RHS const & :param:`rhs`)

    3. | void :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    4. | void :red:`operator=` (:ref:`mpi_lazy\<tag::scatter, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    5. | void :red:`operator=` (:ref:`mpi_lazy\<tag::gather, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

Documentation



 **1)**   ---------------  operator =  --------------------

 Copy the data, without resizing the view.



 **3)**   Performs MPI reduce



 **4)**   Performs MPI scatter



 **5)**   Performs MPI gather





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`RHS` WHAT IS POSSIBLE THERE ?


Parameters
^^^^^^^^^^

 * :param:`l` The lazy object returned by mpi::reduce


.. warning::
    If the size of `this` is not the same as rhs, the behaviour is undefined.