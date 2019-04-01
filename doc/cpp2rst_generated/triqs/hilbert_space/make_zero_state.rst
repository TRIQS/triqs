..
   Generated automatically by cpp2rst

.. highlight:: c


.. _make_zero_state:

make_zero_state
===============

**Synopsis**:

.. code-block:: c

    template<typename HilbertSpace, typename ScalarType, bool BasedOnMap>
    state<HilbertSpace, ScalarType, BasedOnMap> make_zero_state (const
   state<HilbertSpace, ScalarType, BasedOnMap> & st)

Make a copy of a given state with all amplitudes set to 0


Template parameters
-------------------

 * **HilbertSpace**: Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]

 * **ScalarType**: Amplitude type, normally `double` or `std::complex<double>`

 * **BasedOnMap**: Use the implementation based on a map



Parameters
----------

 * **st**: State object to copy



Documentation
-------------