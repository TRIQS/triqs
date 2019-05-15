..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _is_gf_real:

is_gf_real
==========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`is_gf_real(g, tolerance).`
       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, bool> :red:`is_gf_real` (G const & :param:`g`, double :param:`tolerance` = 1.e-13)







Template parameters
^^^^^^^^^^^^^^^^^^^

 * **G**: any Gf type


Parameters
^^^^^^^^^^

 * **g**: a gf

 * **tolerance**: tolerance threshold


Returns
^^^^^^^

 * true iif the function g is real up to tolerance