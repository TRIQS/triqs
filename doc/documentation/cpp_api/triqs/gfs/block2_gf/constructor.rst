..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block2_gf_constructor:

triqs::gfs::block2_gf::constructor
==================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy constructor`
       | :red:`block2_gf` (block2_gf<Var, Target> const & :param:`x`)

    2. | :cppbrief:`Move constructor`
       | :red:`block2_gf` (block2_gf<Var, Target> && )

    3. | :cppbrief:`Construct from block_names and list of gf`
       | :red:`block2_gf` (block2_gf::block_names_t :param:`b`, block2_gf::data_t :param:`d`)

    4. | :cppbrief:`Construct an empty Green function (with empty array).`
       | :red:`block2_gf` ()

    5. | :cppbrief:`From a block_gf_view of the same kind`
       | :red:`block2_gf` (block2_gf::view_type const & :param:`g`)

    6. | :cppbrief:`From a const_gf_view of the same kind`
       | :red:`block2_gf` (block2_gf::const_view_type const & :param:`g`)

    7. | :cppbrief:`Construct from anything which models BlockGreenFunction.`
       | :green:`template<typename G>`
       | :red:`block2_gf` (G const & :param:`x`, std::enable_if_t<BlockGreenFunction<G>::value> * :param:`dummy` = 0)

    8. | :cppbrief:`Construct from the mpi lazy class of the implementation class, cf mpi section`
       | :green:`template<typename Tag>`
       | :red:`block2_gf` (:ref:`mpi_lazy\<Tag, block2_gf_const_view\<Var, Target\> \> <triqs__gfs__mpi_lazy>` :param:`x`)

    9. | :cppbrief:`Constructs a n blocks with copies of g.`
       | :red:`block2_gf` (int :param:`n`, int :param:`p`, block2_gf::g_t const & :param:`g`)

    10. | :cppbrief:`Construct from a vector of gf`
        | :red:`block2_gf` (block2_gf::data_t :param:`V`)

Documentation



 **7)**          TODO: We would like to refine this, G should have the same mesh, target, at least ...



 **8)**          NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...