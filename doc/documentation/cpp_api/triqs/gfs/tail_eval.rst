..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _tail_eval:

tail_eval
=========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Computes sum A_n / om^n`
       | :green:`template<int R>`
       | auto :red:`tail_eval` (arrays::array_const_view<dcomplex, R> :param:`A`, dcomplex :param:`om`)





     Return array<dcomplex, R -1 > if R>1 else dcomplex
   FIXME : use dynamic R array when available.
   FIXME : array of dimension 0