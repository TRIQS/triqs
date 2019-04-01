..
   Generated automatically by cpp2rst

.. highlight:: c


.. _sub_hilbert_space_operator== !=:

operator== !=
=============

**Synopsis**:

.. code-block:: c

    bool operator== (const triqs::hilbert_space::sub_hilbert_space & hs)     (1)

    bool operator!= (const triqs::hilbert_space::sub_hilbert_space & hs)     (2)

(1)Check two Hilbert subspaces for equality


Parameters
----------

 * **hs**: Another Hilbert subspace



Return value
------------

`true` if the two subspaces are equal, `false` otherwise

Documentation
-------------


   Two subspaces are considered equal iff they have the same index and
   equal sets of basis Fock states.



(2)Check two Hilbert subspaces for inequality


Parameters
----------

 * **hs**: Another Hilbert subspace



Return value
------------

`false` if the two subspaces are equal, `true` otherwise

Documentation
-------------


   Two subspaces are considered equal iff they have the same index and
   equal sets of basis Fock states.