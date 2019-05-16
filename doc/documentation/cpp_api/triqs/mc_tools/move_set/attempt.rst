..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__move_set__attempt:

triqs::mc_tools::move_set::attempt
==================================

*#include <triqs/mc_tools/move_set.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | double :red:`attempt` ()





- Picks up one of the move at random (weighted by their proposition probability),
- Call attempt method of that move
- Returns the metropolis ratio R (see move concept).
  The sign ratio returned by the try method of the move is kept.