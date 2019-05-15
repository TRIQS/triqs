..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _imperative_operator_update_coeffs:

triqs::hilbert_space::imperative_operator::update_coeffs
========================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Apply a callable object to each coefficient of the operator by reference`
       | :green:`template<typename Lambda>`
       | void :red:`update_coeffs` (Lambda :param:`L`)






   The callable object must take one argument convertible to `ScalarType &`





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Lambda**: Type of the callable object


Parameters
^^^^^^^^^^

 * **L**: Callable object