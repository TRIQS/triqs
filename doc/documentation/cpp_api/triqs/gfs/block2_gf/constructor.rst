..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block2_gf__constructor:

triqs::gfs::block2_gf::constructor
==================================

*#include <triqs/gfs/block2_gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`block2_gf` (block2_gf<Var, Target> const & :param:`x`)

    2. | :red:`block2_gf` (block2_gf<Var, Target> && )

    3. | :red:`block2_gf` (block2_gf::block_names_t :param:`b`, block2_gf::data_t :param:`d`)

    4. | :red:`block2_gf` ()

    5. | :red:`block2_gf` (block2_gf::view_type const & :param:`g`)

    6. | :red:`block2_gf` (block2_gf::const_view_type const & :param:`g`)

    7. | :green:`template<typename G>`
       | :red:`block2_gf` (G const & :param:`x`, std::enable_if_t<BlockGreenFunction<G>::value> * :param:`dummy` = 0)

    8. | :green:`template<typename Tag>`
       | :red:`block2_gf` (:ref:`mpi_lazy\<Tag, block2_gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`x`)

    9. | :red:`block2_gf` (int :param:`n`, int :param:`p`, block2_gf::g_t const & :param:`g`)

    10. | :red:`block2_gf` (block2_gf::data_t :param:`V`)

Documentation



 **1)**   Copy constructor



 **2)**   Move constructor



 **3)**   Construct from block_names and list of gf



 **4)**   Construct an empty Green function (with empty array).



 **5)**   From a block_gf_view of the same kind



 **6)**   From a const_gf_view of the same kind



 **7)**   Construct from anything which models BlockGreenFunction.
     // TODO: We would like to refine this, G should have the same mesh, target, at least ...



 **8)**   Construct from the mpi lazy class of the implementation class, cf mpi section
     // NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...



 **9)**   Constructs a n blocks with copies of g.



 **10)**   Construct from a vector of gf