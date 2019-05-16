..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block_gf__constructor:

triqs::gfs::block_gf::constructor
=================================

*#include <triqs/gfs/block_gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`block_gf` (block_gf<Var, Target> const & :param:`x`)

    2. | :red:`block_gf` (block_gf<Var, Target> && )

    3. | :red:`block_gf` (block_gf::block_names_t :param:`b`, block_gf::data_t :param:`d`)

    4. | :red:`block_gf` ()

    5. | :red:`block_gf` (block_gf::view_type const & :param:`g`)

    6. | :red:`block_gf` (block_gf::const_view_type const & :param:`g`)

    7. | :green:`template<typename G>`
       | :red:`block_gf` (G const & :param:`x`, std::enable_if_t<BlockGreenFunction<G>::value> * :param:`dummy` = 0)

    8. | :green:`template<typename Tag>`
       | :red:`block_gf` (:ref:`mpi_lazy\<Tag, block_gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`x`)

    9. | :red:`block_gf` (block_gf::data_t :param:`V`)

    10. | :red:`block_gf` (int :param:`n`)

    11. | :red:`block_gf` (int :param:`n`, block_gf::g_t const & :param:`g`)

    12. | :red:`block_gf` (block_gf::block_names_t :param:`b`, block_gf::g_t const & :param:`g`)

    13. | :red:`block_gf` (block_gf::block_names_t :param:`b`)

    14. | :red:`block_gf` (:ref:`gf_mesh\<Var\> <triqs__gfs__gf_mesh>` const & :param:`m`, gf_struct_t const & :param:`gf_struct`)

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



 **9)**   Construct from a vector of gf



 **10)**   Constructs a n block



 **11)**   Constructs a n block with copies of g.



 **12)**   Construct from the vector of names and one gf to be copied



 **13)**   Construct from the vector of names



 **14)**   Create Block Green function from Mesh and gf_struct