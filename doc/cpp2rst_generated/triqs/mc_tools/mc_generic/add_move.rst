..
   Generated automatically by cpp2rst

.. highlight:: c


.. _mc_generic_add_move:

add_move
========

**Synopsis**:

.. code-block:: c

    template<typename MoveType>
    void add_move (MoveType && m, std::string name, double proposition_probability = 1.0)

Register a move


Template parameters
-------------------

 * **MoveType**:                Type of the move. Must model Move concept



Parameters
----------

 * **m**:                        The move. Must model Move concept.

 * **name**:                     Name of the move

 * **proposition_probability**:  Probability that the move will be proposed. Precondition : >0
                                 NB it but does not need to be normalized.
                                 Normalization is automatically done with all the added moves before starting the run.


Documentation
-------------


 If the move m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.