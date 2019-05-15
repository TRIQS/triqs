..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _fit_tail:

fit_tail
========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Product Green Functions`
       | :green:`template<int N, typename T, typename M>`
       | auto :red:`fit_tail` (G<cartesian_product<M...>, T> const & :param:`g`)

    2. | :cppbrief:`Product Green Functions with known_moments`
       | :green:`template<int N, typename T, typename A, typename M>`
       | auto :red:`fit_tail` (G<cartesian_product<M...>, T> const & :param:`g`, A const & :param:`known_moments`)

    3. | :cppbrief:`G(iw) || G(w)`
       | :green:`template<typename V, typename T>`
       | auto :red:`fit_tail` (G<V, T> const & :param:`g`)

    4. | :cppbrief:`G(iw) || G(w) + known_moments`
       | :green:`template<typename V, typename T, typename A>`
       | auto :red:`fit_tail` (G<V, T> const & :param:`g`, A const & :param:`known_moments`)

    5. | :cppbrief:`Adjust configuration of tail-fitting`
       | :green:`template<typename V, typename T, typename A>`
       | auto :red:`fit_tail` (G<V, T> const & :param:`g`,
       |   A const & :param:`known_moments`,
       |   double :param:`tail_fraction`,
       |   int :param:`n_tail_max` = tail_fitter::default_n_tail_max,
       |   std::optional<int> :param:`expansion_order` = {})

Documentation