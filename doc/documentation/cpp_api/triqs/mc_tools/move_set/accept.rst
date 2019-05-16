..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__move_set__accept:

triqs::mc_tools::move_set::accept
=================================

*#include <triqs/mc_tools/move_set.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | MCSignType :red:`accept` ()





accept the move previously selected and tried.
Returns the Sign computed as, if M is the move :
Sign = sign (M.attempt()) * M.accept()