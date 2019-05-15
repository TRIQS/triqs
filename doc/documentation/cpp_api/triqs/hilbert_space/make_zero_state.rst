..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_zero_state:

make_zero_state
===============


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Make a copy of a given state with all amplitudes set to 0`
       | :green:`template<typename HilbertSpace, typename ScalarType, bool BasedOnMap>`
       | :ref:`state\<HilbertSpace, ScalarType, BasedOnMap\> <triqs__hilbert_space__state>` :red:`make_zero_state` (:ref:`state\<HilbertSpace, ScalarType, BasedOnMap\> <triqs__hilbert_space__state>` const & :param:`st`)







Template parameters
^^^^^^^^^^^^^^^^^^^

 * **HilbertSpace**: Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]

 * **ScalarType**: Amplitude type, normally `double` or `std::complex<double>`

 * **BasedOnMap**: Use the implementation based on a map


Parameters
^^^^^^^^^^

 * **st**: State object to copy