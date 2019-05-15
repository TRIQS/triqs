..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block2_gf_view_operator=:

triqs::gfs::block2_gf_view::operator=
=====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Assignment operator overload specific for lazy_transform objects`
       | :green:`template<typename L, typename G>`
       | block2_gf_view<Var, Target> & :red:`operator=` (:ref:`lazy_transform_t\<L, G\> <triqs__gfs__lazy_transform_t>` const & :param:`rhs`)

    2. | :cppbrief:`Assignment operator overload specific for mpi_lazy objects (keep before general assignment)`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, block2_gf_view\<Var, Target\>::const_view_type\> <triqs__gfs__mpi_lazy>` :param:`l`)

    3. | :cppbrief:`Assignment operator`
       | :green:`template<typename RHS>`
       | std::enable_if_t<!arrays::is_scalar<RHS>::value, block2_gf_view<Var, Target> &> :red:`operator=` (RHS const & :param:`rhs`)

    4. | :green:`template<typename RHS>`
       | std::enable_if_t<arrays::is_scalar<RHS>::value, block2_gf_view<Var, Target> &> :red:`operator=` (RHS && :param:`rhs`)

    5. | :cppbrief:`Copy the data, without resizing the view.`
       | block2_gf_view<Var, Target> & :red:`operator=` (block2_gf_view<Var, Target> const & :param:`rhs`)

Documentation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **RHS**: Type of the right hand side rhs

 		 RHS can be anything with .block_names() and [n] -> gf


Parameters
^^^^^^^^^^

 * **rhs**: The lazy object returned e.g. by fourier(my_block_gf)

 * **l**: The lazy object returned by reduce