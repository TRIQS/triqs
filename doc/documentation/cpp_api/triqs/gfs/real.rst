..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _real:

real
====


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Takes the real part of g without check, and returns a new gf with a real target`
       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, typename G::regular_type::real_t> :red:`real` (G const & :param:`g`)





     real(g).





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **G**: any Gf, BlockGf or Block2Gf type


Parameters
^^^^^^^^^^

 * **g**: a gf