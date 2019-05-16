..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__is_gf_hermitian:

triqs::gfs::is_gf_hermitian
===========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, bool> :red:`is_gf_hermitian` (G const & :param:`g`, double :param:`tolerance` = 1.e-13)





Test if gf is hermitian





Parameters
^^^^^^^^^^

 * :param:`g` the gf

 * :param:`tolerance` tolerance threshold :math:`\epsilon`


Returns
^^^^^^^

true iif

.. math::
		\forall n,\; \max_{ab}|g^*_{ba}(-i\omega_n)-g_{ab}(i\omega_n)|<\epsilon

..