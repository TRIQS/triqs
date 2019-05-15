..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _density:

density
=======


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Density`
       | arrays::matrix<dcomplex> :red:`density` (gf_const_view<imfreq> :param:`g`,
       |   array_view<dcomplex, 3>  = {})

    2. | :cppbrief:`-------------------------------------------------------`
       | arrays::matrix<dcomplex> :red:`density` (gf_const_view<refreq> :param:`g`, double :param:`beta`)

    3. | :cppbrief:`-------------------------------------------------------`
       | :green:`template<typename BGf, int R, typename ENABLE_IF = std::enable_if_t<is_block_gf_or_view<BGf>::value,int>`
       | auto :red:`density` (BGf const & :param:`gin`, std::vector<array<dcomplex, R> > const & :param:`known_moments`)

Documentation



 **1)**
 Computes the density of the Gf g, i.e :math:`g(\tau=0^-)`
 Uses tail moments n=1, 2, and 3



 **2)**        For Real Frequency functions
     ------------------------------------------------------



 **3)**        General Version for Block Gf
     ------------------------------------------------------