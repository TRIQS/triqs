..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__is_gf_real_in_tau:

triqs::gfs::is_gf_real_in_tau
=============================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, bool> :red:`is_gf_real_in_tau` (G const & :param:`g`, double :param:`tolerance` = 1.e-13)





Test if gf is real in tau





Parameters
^^^^^^^^^^

 * :param:`g` the gf

 * :param:`tolerance` tolerance threshold :math:`\epsilon`


Returns
^^^^^^^

true iif

.. math::
		\forall n,\; \max_{ab}|g^*_{ab}(-i\omega_n)-g_{ab}(i\omega_n)|<\epsilon

..