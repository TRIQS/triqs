..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mc_generic_add_move:

triqs::mc_tools::mc_generic::add_move
=====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Register a move`
       | :green:`template<typename MoveType>`
       | void :red:`add_move` (MoveType && :param:`m`, std::string :param:`name`, double :param:`proposition_probability` = 1.0)






 If the move m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **MoveType**: Type of the move. Must model Move concept


Parameters
^^^^^^^^^^

 * **m**: The move. Must model Move concept.

 * **name**: Name of the move

 * **proposition_probability**: Probability that the move will be proposed. Precondition : >0
                                 NB it but does not need to be normalized.
                                 Normalization is automatically done with all the added moves before starting the run.