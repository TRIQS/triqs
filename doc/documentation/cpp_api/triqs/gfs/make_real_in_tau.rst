..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_real_in_tau:

triqs::gfs::make_real_in_tau
============================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, typename G::regular_type> :red:`make_real_in_tau` (G const & :param:`g`)





Make_real_in_tau symmetrize the function in freq, to ensure its FT is real.