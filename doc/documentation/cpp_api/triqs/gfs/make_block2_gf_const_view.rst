..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_block2_gf_const_view:

triqs::gfs::make_block2_gf_const_view
=====================================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename Gf>`
       | block2_gf_const_view_of<Gf> :red:`make_block2_gf_const_view` (std::vector<std::vector<Gf> > & :param:`v`)

    2. | :green:`template<typename Gf>`
       | block2_gf_const_view_of<Gf> :red:`make_block2_gf_const_view` (std::vector<std::string> :param:`block_names1`,
       |                            std::vector<std::string> :param:`block_names2`,
       |                            std::vector<std::vector<Gf> > & :param:`v`)

Documentation



 **1)**   Create block2_gf_const_view from vector of views



 **2)**   Create block2_gf_const_view from block_names and vector of views