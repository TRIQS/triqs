..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_tuple_com:

triqs::gfs::make_tuple_com
==========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | :ref:`tuple_com\<std::decay_t\<T\>...\> <triqs__gfs__tuple_com>` :red:`make_tuple_com` (T &&... :param:`x`)





deduction rule. broken on clang ?
   //    template <typename... T> tuple_com(T&& ...x) -> tuple_com<std::decay_t<T>...>;