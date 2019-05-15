..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs_gf_view_assign_delegation:

triqs_gf_view_assign_delegation
===============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`------------------------------------------------------------------------------------------------------`
       | :green:`template<typename M, typename T, typename RHS>`
       | void :red:`triqs_gf_view_assign_delegation` (:ref:`gf_view\<M, T\> <triqs__gfs__gf_view>` :param:`g`, RHS const & :param:`rhs`)

    2. | :cppbrief:`realize the call for gx = fourier(gy);`
       | :green:`template<int N, typename V1, typename T1, typename V2, typename T2, typename Args>`
       | void :red:`triqs_gf_view_assign_delegation` (:ref:`gf_view\<V1, T1\> <triqs__gfs__gf_view>` :param:`lhs_g`,
       |   _fourier_lazy<N, gf_const_view<V2, T2>, Args...> const & :param:`rhs`)

    3. | :cppbrief:`----------------------------`
       | :green:`template<typename T>`
       | void :red:`triqs_gf_view_assign_delegation` (:ref:`gf_view\<imfreq, T\> <triqs__gfs__gf_view>` :param:`gw`,
       |   :ref:`gf_keeper\<tags::legendre, legendre, T\> <triqs__gfs__gf_keeper>` const & :param:`L`)

Documentation



 **1)**                                        View  assignment
-----------------------------------------------------------------------------------------------------