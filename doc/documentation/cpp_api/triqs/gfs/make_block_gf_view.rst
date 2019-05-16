..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_block_gf_view:

triqs::gfs::make_block_gf_view
==============================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename G0, typename G>`
       | block_gf_view_of<G0> :red:`make_block_gf_view` (G0 && :param:`g0`, G &&... :param:`g`)

    2. | :green:`template<typename Gf>`
       | block_gf_view_of<Gf> :red:`make_block_gf_view` (std::vector<Gf> & :param:`v`)

    3. | :green:`template<typename Gf>`
       | block_gf_view_of<Gf> :red:`make_block_gf_view` (std::vector<std::string> :param:`b`, std::vector<Gf> & :param:`v`)

Documentation



 **1)**   Make a block view from the G. Indices are '0', '1', '2', ....



 **2)**   Create block_gf_view from vector of views



 **3)**   Create block_gf_view from block_names and vector of views