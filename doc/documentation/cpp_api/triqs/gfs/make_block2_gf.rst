..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_block2_gf:

triqs::gfs::make_block2_gf
==========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename V, typename T>`
       | :ref:`block2_gf\<V, T\> <triqs__gfs__block2_gf>` :red:`make_block2_gf` (int :param:`n`, int :param:`p`, :ref:`gf\<V, T\> <triqs__gfs__gf>` const & :param:`g`)

    2. | :green:`template<typename V, typename T>`
       | :ref:`block2_gf\<V, T\> <triqs__gfs__block2_gf>` :red:`make_block2_gf` (std::vector<std::string> const & :param:`block_names1`,
       |                 std::vector<std::string> const & :param:`block_names2`,
       |                 std::vector<std::vector<gf<V, T> > > :param:`vv`)

Documentation



 **1)**   From the size n x p and the g from a number and a gf to be copied



 **2)**   from vector<tuple<string,string>>, vector<gf>