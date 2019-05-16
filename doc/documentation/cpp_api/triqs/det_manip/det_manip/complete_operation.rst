..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__complete_operation:

triqs::det_manip::det_manip::complete_operation
===============================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`complete_operation` ()





Finish the move of the last try_xxx called.
Throws if no try_xxx has been done or if the last operation was complete_operation.