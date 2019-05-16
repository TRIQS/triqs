..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__project:

triqs::hilbert_space::project
=============================

*#include <triqs/hilbert_space/state.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename TargetState, typename OriginalState>`
       | TargetState :red:`project` (OriginalState const & :param:`psi`, :ref:`hilbert_space <triqs__hilbert_space__hilbert_space>` const & :param:`proj_hs`)

    2. | :green:`template<typename TargetState, typename OriginalState>`
       | TargetState :red:`project` (OriginalState const & :param:`psi`, :ref:`sub_hilbert_space <triqs__hilbert_space__sub_hilbert_space>` const & :param:`proj_hs`)

Documentation



 **1)**   Construct a projection of a state to a Hilbert space



 **2)**   Construct a projection of a state to a Hilbert subspace





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`TargetState` Type of the result

 * :param:`OriginalState` Type of the state being projected


Parameters
^^^^^^^^^^

 * :param:`psi` State to be projected

 * :param:`proj_hs` Target Hilbert space