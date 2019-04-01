..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_constructor:

constructor
===========

**Synopsis**:

.. code-block:: c

    void det_manip<FunctionType> (FunctionType F, size_t init_size)                                                   (1)

    template<typename ArgumentContainer1, typename ArgumentContainer2>                                                (2)
    void det_manip<FunctionType> (FunctionType F, const ArgumentContainer1 & X, const
   ArgumentContainer2 & Y)

    void det_manip<FunctionType> (const det_manip<FunctionType> & )                                                   (3)

    void det_manip<FunctionType> (det_manip<FunctionType> && rhs) noexcept                                            (4)

(1)Constructor.


Documentation
-------------


 \param F         The function (NB : a copy is made of the F object in this class).
 \param init_size The maximum size of the matrix before a resize (like reserve in std::vector).
                  Like std::vector, resize is automatic (by a factor 2) but can yield a performance penalty
                  if it happens too often.

(2)Constructor.


Documentation
-------------


 \param F         The function (NB : a copy is made of the F object in this class).
 \tparam ArgumentContainer
 \param X, Y : container for X,Y.

(3)


(4)