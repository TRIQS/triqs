..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_hermitian:

make_hermitian
==============


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`make_hermitian: Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)`
       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, typename G::regular_type> :red:`make_hermitian` (G const & :param:`g`)