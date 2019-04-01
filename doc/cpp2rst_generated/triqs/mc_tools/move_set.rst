..
   Generated automatically by cpp2rst

.. highlight:: c


.. _move_set:

move_set
==========

A vector of (moves, proposition_probability), which is also a move itself



**Synopsis**:

.. code-block:: c

    template<typename MCSignType> class move_set;


Member functions
----------------

+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| Member function                                             | Comment                                                                           |
+=============================================================+===================================================================================+
| :ref:`operator= <move_set_operator=>`                       |                                                                                   |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`add <move_set_add>`                                   | Add move M with its probability of being proposed.                                |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`attempt <move_set_attempt>`                           | - Picks up one of the move at random (weighted by their proposition probability), |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`accept <move_set_accept>`                             | accept the move previously selected and tried.                                    |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`reject <move_set_reject>`                             | reject the move :  Call the reject() method of the move previously selected       |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`collect_statistics <move_set_collect_statistics>`     |                                                                                   |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`get_acceptance_rates <move_set_get_acceptance_rates>` | Acceptance rate of all moves as a map name:string -> acceptance_rate:double       |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`get_statistics <move_set_get_statistics>`             | Pretty printing of the acceptance probability of the moves.                       |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+
| :ref:`constructor <move_set_constructor>`                   | Need a random_generator for attempt, see below...                                 |
+-------------------------------------------------------------+-----------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    move_set/operator=
    move_set/add
    move_set/attempt
    move_set/accept
    move_set/reject
    move_set/collect_statistics
    move_set/get_acceptance_rates
    move_set/get_statistics
    move_set/constructor


Non Member functions
--------------------

+-------------------------------------+---------+
| Non member function                 | Comment |
+=====================================+=========+
| :ref:`h5_write <move_set_h5_write>` |         |
+-------------------------------------+---------+
| :ref:`h5_read <move_set_h5_read>`   |         |
+-------------------------------------+---------+

.. toctree::
    :hidden:

    move_set/h5_write
    move_set/h5_read