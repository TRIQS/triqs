..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf__constructor:

triqs::gfs::gf::constructor
===========================

*#include <triqs/gfs/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf` ()

    2. | :red:`gf` (gf<Var, Target> const & :param:`x`)

    3. | :red:`gf` (gf<Var, Target> && )

    4. | :red:`gf` (gf::mesh_t :param:`m`,
       |   gf::target_shape_t :param:`shape`,
       |   arrays::memory_layout_t<arity + Target::rank> const & :param:`ml`,
       |   gf::indices_t const & :param:`ind` = indices_t{})

    5. | :red:`gf` (gf::mesh_t :param:`m`, gf::data_t :param:`dat`, gf::indices_t :param:`ind`)

    6. | :red:`gf` (gf::mesh_t :param:`m`,
       |   gf::target_shape_t :param:`shape` = target_shape_t{},
       |   gf::indices_t const & :param:`ind` = indices_t{})

    7. | :red:`gf` (gf::mesh_t :param:`m`,
       |   gf::data_t :param:`dat`,
       |   arrays::memory_layout_t<arity + Target::rank> const & :param:`ml`,
       |   gf::indices_t :param:`ind`)

    8. | :red:`gf` (gf_view<Var, Target> const & :param:`g`)

    9. | :red:`gf` (gf_const_view<Var, Target> const & :param:`g`)

    10. | :green:`template<typename G>`
        | :red:`gf` (G const & :param:`g`)

    11. | :green:`template<typename Tag>`
        | :red:`gf` (:ref:`mpi_lazy\<Tag, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`l`)

Documentation



 **1)**   Empty Green function (with empty array).



 **2)**   Copy



 **3)**   Move



 **8)**   Makes a deep copy of the data



 **9)**   Makes a deep copy of the data



 **10)**   From any object modeling the ImmutableGreenFunction concept.



 **11)**   from the mpi lazy operation. Cf MPI section !





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`G` A type modeling ImmutableGreenFunction.

 * :param:`Tag`


Parameters
^^^^^^^^^^

 * :param:`m` Mesh

 * :param:`shape` Target shape

 * :param:`ml` Memory layout for the *whole* data array

 * :param:`ind` Indices

 * :param:`dat` data arrray

 * :param:`g`

 * :param:`l` The lazy object

  NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...


Example
^^^^^^^

..
   Included automatically from /mnt/home/wentzell/Dropbox/Coding/triqs/doc/documentation/examples/triqs/gfs/gf_constructors_0.cpp

.. code-block:: cpp

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    int main() {
      auto beta = 10.0;
      gf<imfreq> gf{{beta, Fermion}, {1, 1}};
    }