..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _project:

project
=======


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Construct a projection of a state to a Hilbert space`
       | :green:`template<typename TargetState, typename OriginalState>`
       | TargetState :red:`project` (OriginalState const & :param:`psi`, :ref:`hilbert_space <triqs__hilbert_space__hilbert_space>` const & :param:`proj_hs`)

    2. | :cppbrief:`Construct a projection of a state to a Hilbert subspace`
       | :green:`template<typename TargetState, typename OriginalState>`
       | TargetState :red:`project` (OriginalState const & :param:`psi`, :ref:`sub_hilbert_space <triqs__hilbert_space__sub_hilbert_space>` const & :param:`proj_hs`)

Documentation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **TargetState**: Type of the result

 * **OriginalState**: Type of the state being projected


Parameters
^^^^^^^^^^

 * **psi**: State to be projected

 * **proj_hs**: Target Hilbert space