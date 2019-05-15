..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block_gf_constructor:

triqs::gfs::block_gf::constructor
=================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy constructor`
       | :red:`block_gf` (block_gf<Var, Target> const & :param:`x`)

    2. | :cppbrief:`Move constructor`
       | :red:`block_gf` (block_gf<Var, Target> && )

    3. | :cppbrief:`Construct from block_names and list of gf`
       | :red:`block_gf` (block_gf::block_names_t :param:`b`, block_gf::data_t :param:`d`)

    4. | :cppbrief:`Construct an empty Green function (with empty array).`
       | :red:`block_gf` ()

    5. | :cppbrief:`From a block_gf_view of the same kind`
       | :red:`block_gf` (block_gf::view_type const & :param:`g`)

    6. | :cppbrief:`From a const_gf_view of the same kind`
       | :red:`block_gf` (block_gf::const_view_type const & :param:`g`)

    7. | :cppbrief:`Construct from anything which models BlockGreenFunction.`
       | :green:`template<typename G>`
       | :red:`block_gf` (G const & :param:`x`, std::enable_if_t<BlockGreenFunction<G>::value> * :param:`dummy` = 0)

    8. | :cppbrief:`Construct from the mpi lazy class of the implementation class, cf mpi section`
       | :green:`template<typename Tag>`
       | :red:`block_gf` (:ref:`mpi_lazy\<Tag, block_gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`x`)

    9. | :cppbrief:`Construct from a vector of gf`
       | :red:`block_gf` (block_gf::data_t :param:`V`)

    10. | :cppbrief:`Constructs a n block`
        | :red:`block_gf` (int :param:`n`)

    11. | :cppbrief:`Constructs a n block with copies of g.`
        | :red:`block_gf` (int :param:`n`, block_gf::g_t const & :param:`g`)

    12. | :cppbrief:`Construct from the vector of names and one gf to be copied`
        | :red:`block_gf` (block_gf::block_names_t :param:`b`, block_gf::g_t const & :param:`g`)

    13. | :cppbrief:`Construct from the vector of names`
        | :red:`block_gf` (block_gf::block_names_t :param:`b`)

    14. | :cppbrief:`Create Block Green function from Mesh and gf_struct`
        | :red:`block_gf` (:ref:`gf_mesh\<Var\> <triqs__gfs__gf_mesh>` const & :param:`m`, gf_struct_t const & :param:`gf_struct`)

Documentation



 **7)**          TODO: We would like to refine this, G should have the same mesh, target, at least ...



 **8)**          NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...