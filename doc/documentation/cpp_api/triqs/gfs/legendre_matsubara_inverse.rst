..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _legendre_matsubara_inverse:

legendre_matsubara_inverse
==========================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`----------------------------`
       | :green:`template<typename G1, typename G2>`
       | std::enable_if_t<is_gf_v<G2, imfreq> > :red:`legendre_matsubara_inverse` (G1 && :param:`gl`, G2 const & :param:`gw`)

    2. | :cppbrief:`----------------------------`
       | :green:`template<typename G1, typename G2>`
       | std::enable_if_t<is_gf_v<G2, imtime> > :red:`legendre_matsubara_inverse` (G1 && :param:`gl`, G2 const & :param:`gt`)

Documentation