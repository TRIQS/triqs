..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__move_set__add:

triqs::mc_tools::move_set::add
==============================

*#include <triqs/mc_tools/move_set.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename MoveType>`
       | void :red:`add` (MoveType && :param:`M`, std::string :param:`name`, double :param:`proposition_probability`)





Add move M with its probability of being proposed.
NB : the proposition_probability needs to be >0 but does not need to be
normalized. Normalization is automatically done with all the added moves
before starting the run