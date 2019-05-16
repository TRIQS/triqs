..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_hermitian:

triqs::gfs::make_hermitian
==========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, typename G::regular_type> :red:`make_hermitian` (G const & :param:`g`)





make_hermitian: Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)