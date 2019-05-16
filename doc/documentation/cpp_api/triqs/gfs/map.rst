..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__map:

triqs::gfs::map
===============

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename F, typename G>`
       | std::enable_if_t<is_block_gf_or_view<G>::value, decltype(impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g)))> :red:`map` (F && :param:`f`,
       |      G && :param:`g`)





the map function itself...