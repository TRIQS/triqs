..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__make_zero_state:

triqs::hilbert_space::make_zero_state
=====================================

*#include <triqs/hilbert_space/state.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename HilbertSpace, typename ScalarType, bool BasedOnMap>`
       | :ref:`state\<HilbertSpace, ScalarType, BasedOnMap\> <triqs__hilbert_space__state>` :red:`make_zero_state` (:ref:`state\<HilbertSpace, ScalarType, BasedOnMap\> <triqs__hilbert_space__state>` const & :param:`st`)





Make a copy of a given state with all amplitudes set to 0





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`HilbertSpace` Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]

 * :param:`ScalarType` Amplitude type, normally `double` or `std::complex<double>`

 * :param:`BasedOnMap` Use the implementation based on a map


Parameters
^^^^^^^^^^

 * :param:`st` State object to copy