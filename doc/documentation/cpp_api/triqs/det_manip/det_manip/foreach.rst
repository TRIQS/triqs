..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _det_manip_foreach:

triqs::det_manip::det_manip::foreach
====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Given a lambda f : x,y,M, it calls f(x_i,y_j,M_ji) for all i,j`
       | :green:`template<typename LambdaType>`
       | void :red:`foreach` (det_manip<FunctionType> const & :param:`d`, LambdaType const & :param:`f`)





         Order of iteration is NOT fixed, it is optimised (for memory traversal)