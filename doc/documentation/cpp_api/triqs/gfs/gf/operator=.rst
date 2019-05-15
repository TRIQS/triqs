..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_operator=:

triqs::gfs::gf::operator=
=========================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy assignment`
       | gf<Var, Target> & :red:`operator=` (gf<Var, Target> & :param:`rhs`)

    2. | gf<Var, Target> & :red:`operator=` (gf<Var, Target> const & :param:`rhs`)

    3. | :cppbrief:`Move assignment`
       | gf<Var, Target> & :red:`operator=` (gf<Var, Target> && :param:`rhs`) noexcept

    4. | :cppbrief:`Assignment operator`
       | :green:`template<typename RHS>`
       | gf<Var, Target> & :red:`operator=` (RHS && :param:`rhs`)

    5. | :cppbrief:`Performs MPI reduce`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    6. | :cppbrief:`Performs MPI scatter`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::scatter, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    7. | :cppbrief:`Performs MPI gather`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::gather, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

Documentation



 **4)**
 The assignment resizes the mesh and the data, invalidating all pointers on them.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **RHS**: Type of the right hand side rhs

 		 RHS can be anything modeling the gf concept TBW
 		 In particular lazy expression with Green functions


Parameters
^^^^^^^^^^

 * **rhs**:

 * **l**: The lazy object returned by mpi::reduce

.. error:: File not found

    Example file /mnt/home/wentzell/Dropbox/Coding/triqs/doc/documentation/examples/triqs/gfs/gf_assign_0.cpp not found