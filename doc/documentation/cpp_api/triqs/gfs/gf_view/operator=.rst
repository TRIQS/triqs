..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_view_operator=:

triqs::gfs::gf_view::operator=
==============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`---------------  operator =  --------------------`
       | gf_view<Var, Target> & :red:`operator=` (gf_view<Var, Target> const & :param:`rhs`)

    2. | :cppbrief:`Assignment`
       | :green:`template<typename RHS>`
       | gf_view<Var, Target> & :red:`operator=` (RHS const & :param:`rhs`)

    3. | :cppbrief:`Performs MPI reduce`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::reduce, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    4. | :cppbrief:`Performs MPI scatter`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::scatter, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

    5. | :cppbrief:`Performs MPI gather`
       | void :red:`operator=` (:ref:`mpi_lazy\<tag::gather, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

Documentation



 **1)**          Copy the data, without resizing the view.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **RHS**: WRITE A LIST OF POSSIBLES ...


Parameters
^^^^^^^^^^

 * **l**: The lazy object returned by mpi::reduce