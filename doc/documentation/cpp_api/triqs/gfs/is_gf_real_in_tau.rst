..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _is_gf_real_in_tau:

is_gf_real_in_tau
=================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Test if gf is real in tau`
       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, bool> :red:`is_gf_real_in_tau` (G const & :param:`g`, double :param:`tolerance` = 1.e-13)







Parameters
^^^^^^^^^^

 * **g**: the gf

 * **tolerance**: tolerance threshold :math:`\epsilon`


Returns
^^^^^^^

 * true iif

.. math::
		\forall n,\; \max_{ab}|g^*_{ab}(-i\omega_n)-g_{ab}(i\omega_n)|<\epsilon

..