..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__is_singular:

triqs::det_manip::det_manip::is_singular
========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | bool :red:`is_singular` () const





it the det 0 ? I.e. (singular_threshold <0 ? not std::isnormal(std::abs(det)) : (std::abs(det)<singular_threshold))