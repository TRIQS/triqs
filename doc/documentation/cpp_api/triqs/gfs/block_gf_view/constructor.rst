..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block_gf_view_constructor:

triqs::gfs::block_gf_view::constructor
======================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy constructor`
       | :red:`block_gf_view` (block_gf_view<Var, Target> const & :param:`x`)

    2. | :cppbrief:`Move constructor`
       | :red:`block_gf_view` (block_gf_view<Var, Target> && )

    3. | :cppbrief:`Construct from block_names and list of gf`
       | :red:`block_gf_view` (block_gf_view::block_names_t :param:`b`, block_gf_view::data_t :param:`d`)

    4. | :cppbrief:`---------------  Constructors --------------------`
       | :red:`block_gf_view` ()

    5. | :red:`block_gf_view` (block_gf_view::const_view_type const & :param:`g`)

    6. | :red:`block_gf_view` (block_gf_view::regular_type const & :param:`g`)

    7. | :cppbrief:`Makes a view`
       | :red:`block_gf_view` (block_gf_view::regular_type & :param:`g`)

    8. | :cppbrief:`Makes a view`
       | :red:`block_gf_view` (block_gf_view::regular_type && :param:`g`) noexcept

Documentation