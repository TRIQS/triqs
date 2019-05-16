..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__is_gf_real:

triqs::gfs::is_gf_real
======================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, bool> :red:`is_gf_real` (G const & :param:`g`, double :param:`tolerance` = 1.e-13)





is_gf_real(g, tolerance).





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`G` any Gf type


Parameters
^^^^^^^^^^

 * :param:`g` a gf

 * :param:`tolerance` tolerance threshold


Returns
^^^^^^^

true iif the function g is real up to tolerance