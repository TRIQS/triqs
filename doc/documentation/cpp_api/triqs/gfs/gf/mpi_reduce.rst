..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf__mpi_reduce:

triqs::gfs::gf::mpi_reduce
==========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :ref:`mpi_lazy\<tag::reduce, gf::const_view_type\> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (gf::this_t const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)





mako ${mpidoc("Reduce")}