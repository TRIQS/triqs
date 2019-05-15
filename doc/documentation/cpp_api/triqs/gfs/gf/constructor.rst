..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_constructor:

triqs::gfs::gf::constructor
===========================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Construct an empty Green function (with empty array).`
       | :red:`gf` ()

    2. | :cppbrief:`Copy constructor`
       | :red:`gf` (gf<Var, Target> const & :param:`x`)

    3. | :cppbrief:`Move constructor`
       | :red:`gf` (gf<Var, Target> && )

    4. | :cppbrief:`Construct from the data. Using the "pass by value" pattern + move`
       | :red:`gf` (gf::mesh_t :param:`m`, gf::data_t :param:`dat`, gf::indices_t :param:`ind`)

    5. | :cppbrief:`Construct from the data. Using the "pass by value" pattern + move`
       | :red:`gf` (gf::mesh_t :param:`m`,
       |   gf::data_t :param:`dat`,
       |   arrays::memory_layout_t<arity + Target::rank> const & :param:`ml`,
       |   gf::indices_t :param:`ind`)

    6. | :cppbrief:`Construct from mesh, target_shape, memory order`
       | :red:`gf` (gf::mesh_t :param:`m`,
       |   gf::target_shape_t :param:`shape`,
       |   arrays::memory_layout_t<arity + Target::rank> const & :param:`ml`,
       |   gf::indices_t const & :param:`ind` = indices_t{})

    7. | :cppbrief:`Construct from mesh, target_shape, memory order`
       | :red:`gf` (gf::mesh_t :param:`m`,
       |   gf::target_shape_t :param:`shape` = target_shape_t{},
       |   gf::indices_t const & :param:`ind` = indices_t{})

    8. | :cppbrief:`From a gf_view of the same kind`
       | :red:`gf` (gf_view<Var, Target> const & :param:`g`)

    9. | :cppbrief:`From a const_gf_view of the same kind`
       | :red:`gf` (gf_const_view<Var, Target> const & :param:`g`)

    10. | :cppbrief:`Construct from anything which models ImmutableGreenFunction.`
        | :green:`template<typename G>`
        | :red:`gf` (G const & :param:`x`, std::enable_if_t<ImmutableGreenFunction<G>::value> * :param:`dummy` = 0)

    11. | :cppbrief:`Construct from the mpi lazy class of the implementation class, cf mpi section`
        | :green:`template<typename Tag>`
        | :red:`gf` (:ref:`mpi_lazy\<Tag, gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`x`)

Documentation



 **10)**          TODO: We would like to refine this, G should have the same mesh, target, at least ...



 **11)**          NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...




Example
---------

.. code-block:: cpp

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    int main() {
      auto beta = 10.0;
      gf<imfreq> gf{{beta, Fermion}, {1, 1}};
    }