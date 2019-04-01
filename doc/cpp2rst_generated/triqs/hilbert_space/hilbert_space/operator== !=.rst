..
   Generated automatically by cpp2rst

.. highlight:: c


.. _hilbert_space_operator== !=:

operator== !=
=============

**Synopsis**:

.. code-block:: c

    bool operator== (const triqs::hilbert_space::hilbert_space & hs)     (1)

    bool operator!= (const triqs::hilbert_space::hilbert_space & hs)     (2)

(1)Check two Hilbert spaces for equality


Parameters
----------

 * **hs**: Another Hilbert space



Return value
------------

`true` if the two spaces are equal, `false` otherwise

Documentation
-------------


   For the full Hilbert spaces this check is equivalent to a dimension equality check



(2)Check two Hilbert spaces for inequality


Parameters
----------

 * **hs**: Another Hilbert space



Return value
------------

`false` if the two spaces are equal, `true` otherwise

Documentation
-------------


   For the full Hilbert spaces this check is equivalent to a dimension inequality check