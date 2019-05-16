..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__state:

triqs::hilbert_space::state
===========================

*#include <triqs/hilbert_space/state.hpp>*

.. rst-class:: cppsynopsis

     template<typename HilbertSpace, typename ScalarType, bool BasedOnMap> class  :red:`state`

Many-body state as a list of amplitudes in a *basis of Fock states*

 The amplitudes can either be stored using a map (only non-vanishing elements are stored)
 or in a `triqs::arrays::vector`. In order to make this class model [[statevector_concept]]
 free function [[make_zero_state]] is defined. Specializations on `BasedOnMap` provide
 different extensions of the concept, and are documented separately:

 - :ref:`state, map-based storage <state\<HilbertSpace,ScalarType,true\>>`
 - :ref:`state, vector-based storage <state\<HilbertSpace,ScalarType,false\>>`

 Function [[project]] can be used to project a state from one Hilbert space to another Hilbert space/subspace.


Template parameters
-------------------

 * **HilbertSpace**: Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]

 * **ScalarType**: Amplitude type, normally `double` or `std::complex<double>`

 * **BasedOnMap**: Use the implementation based on a map