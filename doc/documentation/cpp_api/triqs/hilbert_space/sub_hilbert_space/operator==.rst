..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _sub_hilbert_space_operator==:

triqs::hilbert_space::sub_hilbert_space::operator==
===================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Check two Hilbert subspaces for equality`
       | bool :red:`operator==` (:ref:`sub_hilbert_space <triqs__hilbert_space__sub_hilbert_space>` const & :param:`hs`) const






   Two subspaces are considered equal iff they have the same index and
   equal sets of basis Fock states.





Parameters
^^^^^^^^^^

 * **hs**: Another Hilbert subspace


Returns
^^^^^^^

 * `true` if the two subspaces are equal, `false` otherwise