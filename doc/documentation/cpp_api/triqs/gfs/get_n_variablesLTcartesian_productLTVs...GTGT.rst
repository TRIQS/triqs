..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__get_n_variablesLTcartesian_productLTVs___GTGT:

triqs::gfs::get_n_variables<cartesian_product<Vs...>>
=====================================================

Defined in header <**>

.. code-block:: c

    template<typename Vs> class get_n_variables<cartesian_product<Vs...>>

template <typename... Vs>  constexpr int get_n_variables(cartesian_product<Vs...>) { return sizeof...(Vs);}