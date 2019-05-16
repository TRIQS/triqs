..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__fit_tail:

triqs::gfs::fit_tail
====================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<int N, typename T, typename M>`
       | auto :red:`fit_tail` (G<cartesian_product<M...>, T> const & :param:`g`)

    2. | :green:`template<int N, typename T, typename A, typename M>`
       | auto :red:`fit_tail` (G<cartesian_product<M...>, T> const & :param:`g`, A const & :param:`known_moments`)

    3. | :green:`template<typename V, typename T>`
       | auto :red:`fit_tail` (G<V, T> const & :param:`g`)

    4. | :green:`template<typename V, typename T, typename A>`
       | auto :red:`fit_tail` (G<V, T> const & :param:`g`, A const & :param:`known_moments`)

    5. | :green:`template<typename V, typename T, typename A>`
       | auto :red:`fit_tail` (G<V, T> const & :param:`g`,
       |           A const & :param:`known_moments`,
       |           double :param:`tail_fraction`,
       |           int :param:`n_tail_max` = tail_fitter::default_n_tail_max,
       |           std::optional<int> :param:`expansion_order` = {})

Documentation



 **1)**   Product Green Functions



 **2)**   Product Green Functions with known_moments



 **3)**   G(iw) || G(w)



 **4)**   G(iw) || G(w) + known_moments



 **5)**   Adjust configuration of tail-fitting