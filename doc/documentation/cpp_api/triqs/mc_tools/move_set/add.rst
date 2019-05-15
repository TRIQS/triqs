..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _move_set_add:

triqs::mc_tools::move_set::add
==============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Add move M with its probability of being proposed.`
       | :green:`template<typename MoveType>`
       | void :red:`add` (MoveType && :param:`M`, std::string :param:`name`, double :param:`proposition_probability`)





   NB : the proposition_probability needs to be >0 but does not need to be
 normalized. Normalization is automatically done with all the added moves
 before starting the run