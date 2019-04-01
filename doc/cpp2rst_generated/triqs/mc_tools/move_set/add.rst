..
   Generated automatically by cpp2rst

.. highlight:: c


.. _move_set_add:

add
===

**Synopsis**:

.. code-block:: c

    template<typename MoveType>
    void add (MoveType && M, std::string name, double proposition_probability)

Add move M with its probability of being proposed.


Documentation
-------------

 NB : the proposition_probability needs to be >0 but does not need to be
 normalized. Normalization is automatically done with all the added moves
 before starting the run