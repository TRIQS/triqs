..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_real_in_tau:

make_real_in_tau
================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Make_real_in_tau symmetrize the function in freq, to ensure its FT is real.`
       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, typename G::regular_type> :red:`make_real_in_tau` (G const & :param:`g`)