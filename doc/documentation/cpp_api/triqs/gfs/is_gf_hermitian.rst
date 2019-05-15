..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _is_gf_hermitian:

is_gf_hermitian
===============


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Test if gf is hermitian`
       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, bool> :red:`is_gf_hermitian` (G const & :param:`g`, double :param:`tolerance` = 1.e-13)







Parameters
^^^^^^^^^^

 * **g**: the gf

 * **tolerance**: tolerance threshold :math:`\epsilon`


Returns
^^^^^^^

 * true iif

.. math::
		\forall n,\; \max_{ab}|g^*_{ba}(-i\omega_n)-g_{ab}(i\omega_n)|<\epsilon

..