..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block_gf_view__constructor:

triqs::gfs::block_gf_view::constructor
======================================

*#include <triqs/gfs/block_gf_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`block_gf_view` (block_gf_view<Var, Target> const & :param:`x`)

    2. | :red:`block_gf_view` (block_gf_view<Var, Target> && )

    3. | :red:`block_gf_view` (block_gf_view::block_names_t :param:`b`, block_gf_view::data_t :param:`d`)

    4. | :red:`block_gf_view` ()

    5. | :red:`block_gf_view` (block_gf_view::const_view_type const & :param:`g`)

    6. | :red:`block_gf_view` (block_gf_view::regular_type const & :param:`g`)

    7. | :red:`block_gf_view` (block_gf_view::regular_type & :param:`g`)

    8. | :red:`block_gf_view` (block_gf_view::regular_type && :param:`g`) noexcept

Documentation



 **1)**   Copy constructor



 **2)**   Move constructor



 **3)**   Construct from block_names and list of gf



 **4)**   ---------------  Constructors --------------------



 **7)**   Makes a view



 **8)**   Makes a view