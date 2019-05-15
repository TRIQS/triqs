..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _fit_hermitian_tail_on_window:

fit_hermitian_tail_on_window
============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Fit_tail on a window with the constraint of hermitian moment matrices`
       | :green:`template<typename T>`
       | auto :red:`fit_hermitian_tail_on_window` (G<imfreq, T> const & :param:`g`,
       |   int :param:`n_min`,
       |   int :param:`n_max`,
       |   array_const_view<dcomplex, 3> :param:`known_moments`,
       |   int :param:`n_tail_max`,
       |   int :param:`expansion_order`)