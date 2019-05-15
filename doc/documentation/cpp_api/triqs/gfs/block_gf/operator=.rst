..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block_gf_operator=:

triqs::gfs::block_gf::operator=
===============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy assignment`
       | block_gf<Var, Target> & :red:`operator=` (block_gf<Var, Target> & :param:`rhs`)

    2. | block_gf<Var, Target> & :red:`operator=` (block_gf<Var, Target> const & :param:`rhs`)

    3. | :cppbrief:`Move assignment`
       | block_gf<Var, Target> & :red:`operator=` (block_gf<Var, Target> && :param:`rhs`)

    4. | :cppbrief:`Assignment operator overload specific for mpi_lazy objects (keep before general assignment)`
       | block_gf<Var, Target> & :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, block_gf\<Var, Target\>::const_view_type\> <triqs__gfs__mpi_lazy>` :param:`l`)

    5. | :cppbrief:`Assignment operator`
       | :green:`template<typename RHS>`
       | block_gf<Var, Target> & :red:`operator=` (RHS && :param:`rhs`)

Documentation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **RHS**: Type of the right hand side rhs

             RHS can be anything modeling the gf concept TBW
             In particular lazy expression with Green functions


Parameters
^^^^^^^^^^

 * **l**: The lazy object returned by reduce

 * **rhs**: The assignment resizes the mesh and the data, invalidating all pointers on them.