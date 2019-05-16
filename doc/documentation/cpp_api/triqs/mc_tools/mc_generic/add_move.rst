..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__mc_generic__add_move:

triqs::mc_tools::mc_generic::add_move
=====================================

*#include <triqs/mc_tools/mc_generic.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename MoveType>`
       | void :red:`add_move` (MoveType && :param:`m`, std::string :param:`name`, double :param:`proposition_probability` = 1.0)





Register a move

If the move m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`MoveType`                Type of the move. Must model Move concept


Parameters
^^^^^^^^^^

 * :param:`m`                        The move. Must model Move concept.

 * :param:`name`                     Name of the move

 * :param:`proposition_probability`  Probability that the move will be proposed. Precondition : >0
                                 NB it but does not need to be normalized.
                                 Normalization is automatically done with all the added moves before starting the run.