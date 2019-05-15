..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_tuple_com:

make_tuple_com
==============


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`deduction rule. broken on clang ?`
       | :green:`template<typename T>`
       | :ref:`tuple_com\<std::decay_t\<T\>...\> <triqs__gfs__tuple_com>` :red:`make_tuple_com` (T &&... :param:`x`)





          template <typename... T> tuple_com(T&& ...x) -> tuple_com<std::decay_t<T>...>;