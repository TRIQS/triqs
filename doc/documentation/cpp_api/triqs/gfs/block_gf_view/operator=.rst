..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block_gf_view__operator=:

triqs::gfs::block_gf_view::operator=
====================================

*#include <triqs/gfs/block_gf_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename L, typename G>`
       | block_gf_view<Var, Target> & :red:`operator=` (:ref:`lazy_transform_t\<L, G\> <triqs__gfs__lazy_transform_t>` const & :param:`rhs`)

    2. | void :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, block_gf_view\<Var, Target\>::const_view_type\> <triqs__gfs__mpi_lazy>` :param:`l`)

    3. | :green:`template<typename RHS>`
       | std::enable_if_t<!arrays::is_scalar<RHS>::value, block_gf_view<Var, Target> &> :red:`operator=` (RHS const & :param:`rhs`)

    4. | block_gf_view<Var, Target> & :red:`operator=` (block_gf_view<Var, Target> const & :param:`rhs`)

Documentation



 **1)**   Assignment operator overload specific for lazy_transform objects



 **2)**   Assignment operator overload specific for mpi_lazy objects (keep before general assignment)



 **3)**   Assignment operator



 **4)**   Copy the data, without resizing the view.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`RHS` Type of the right hand side rhs

 		 RHS can be anything with .block_names() and [n] -> gf


Parameters
^^^^^^^^^^

 * :param:`rhs` The lazy object returned e.g. by fourier(my_block_gf)

 * :param:`l` The lazy object returned by reduce