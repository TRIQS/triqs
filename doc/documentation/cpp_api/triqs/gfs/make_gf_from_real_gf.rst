..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_gf_from_real_gf:

make_gf_from_real_gf
====================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`This function takes a g(i omega_n) on half mesh (positive omega_n) and returns a gf on the whole mesh`
       | :green:`template<typename T>`
       | :ref:`gf\<imfreq, T\> <triqs__gfs__gf>` :red:`make_gf_from_real_gf` (:ref:`gf_const_view\<imfreq, T\> <triqs__gfs__gf_const_view>` :param:`g`)






   using G(-i omega_n) = G(i omega_n)^* for real G(tau) functions.