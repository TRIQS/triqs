..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _map:

map
===


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`the map function itself...`
       | :green:`template<typename F, typename G>`
       | std::enable_if_t<is_block_gf_or_view<G>::value, decltype(impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g)))> :red:`map` (F && :param:`f`,
       |   G && :param:`g`)