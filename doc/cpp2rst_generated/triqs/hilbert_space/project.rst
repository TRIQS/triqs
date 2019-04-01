..
   Generated automatically by cpp2rst

.. highlight:: c


.. _project:

project
=======

**Synopsis**:

.. code-block:: c

    template<typename TargetState, typename OriginalState>                                                           (1)
    TargetState project (const OriginalState & psi, const
   triqs::hilbert_space::hilbert_space & proj_hs)

    template<typename TargetState, typename OriginalState>                                                           (2)
    TargetState project (const OriginalState & psi, const
   triqs::hilbert_space::sub_hilbert_space & proj_hs)

(1)Construct a projection of a state to a Hilbert space


Template parameters
-------------------

 * **TargetState**: Type of the result

 * **OriginalState**: Type of the state being projected



Parameters
----------

 * **psi**: State to be projected

 * **proj_hs**: Target Hilbert space



Documentation
-------------




(2)Construct a projection of a state to a Hilbert subspace


Template parameters
-------------------

 * **TargetState**: Type of the result

 * **OriginalState**: Type of the state being projected



Parameters
----------

 * **psi**: State to be projected

 * **proj_hs**: Target Hilbert subspace



Documentation
-------------