..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__tail_fitter__fit:

triqs::gfs::tail_fitter::fit
============================

*#include <triqs/gfs/tail_fitter.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool enforce_hermiticity, typename M, int R, int R2>`
       | std::pair<arrays::array<dcomplex, R>, double> :red:`fit` (M const & :param:`m`,
       |      array_const_view<dcomplex, R> :param:`g_data`,
       |      int :param:`n`,
       |      bool :param:`normalize`,
       |      array_const_view<dcomplex, R2> :param:`known_moments`,
       |      std::optional<long> :param:`inner_matrix_dim` = {})











Parameters
^^^^^^^^^^

 * :param:`m` mesh

 * :param:`data`

 * :param:`n` position of the omega in the data array

 * :param:`normalize` Finish the normalization of the tail coefficient (normally true)

 * :param:`known_moments`  Array of the known_moments

    // FIXME : nda : use dynamic Rank here.