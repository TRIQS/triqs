..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__move__constructor:

triqs::mc_tools::move::constructor
==================================

*#include <triqs/mc_tools/move.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename MoveType>`
       | :red:`move` (bool , MoveType && :param:`m`)

    2. | :red:`move` (move<MCSignType> const & :param:`rhs`)

    3. | :red:`move` (move<MCSignType> && :param:`rhs`)

Documentation



 **1)**   Construct from any m modeling MoveType. bool is here to disambiguate with basic copy/move construction.



 **2)**   no default constructor.