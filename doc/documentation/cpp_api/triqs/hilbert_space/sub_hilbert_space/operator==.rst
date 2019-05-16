..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__sub_hilbert_space__operator==:

triqs::hilbert_space::sub_hilbert_space::operator==
===================================================

*#include <triqs/hilbert_space/sub_hilbert_space.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | bool :red:`operator==` (:ref:`sub_hilbert_space <triqs__hilbert_space__sub_hilbert_space>` const & :param:`hs`) const





Check two Hilbert subspaces for equality

  Two subspaces are considered equal iff they have the same index and
  equal sets of basis Fock states.





Parameters
^^^^^^^^^^

 * :param:`hs` Another Hilbert subspace


Returns
^^^^^^^

`true` if the two subspaces are equal, `false` otherwise