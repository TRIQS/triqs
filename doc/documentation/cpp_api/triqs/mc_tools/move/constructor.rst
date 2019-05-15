..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _move_constructor:

triqs::mc_tools::move::constructor
==================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Construct from any m modeling MoveType. bool is here to disambiguate with basic copy/move construction.`
       | :green:`template<typename MoveType>`
       | :red:`move` (bool , MoveType && :param:`m`)

    2. | :cppbrief:`no default constructor.`
       | :red:`move` (move<MCSignType> const & :param:`rhs`)

    3. | :red:`move` (move<MCSignType> && :param:`rhs`)

Documentation