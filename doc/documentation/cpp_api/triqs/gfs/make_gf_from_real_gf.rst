..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_gf_from_real_gf:

triqs::gfs::make_gf_from_real_gf
================================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | :ref:`gf\<imfreq, T\> <triqs__gfs__gf>` :red:`make_gf_from_real_gf` (:ref:`gf_const_view\<imfreq, T\> <triqs__gfs__gf_const_view>` :param:`g`)





This function takes a g(i omega_n) on half mesh (positive omega_n) and returns a gf on the whole mesh

  using G(-i omega_n) = G(i omega_n)^* for real G(tau) functions.