..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _tail_fitter_fit_hermitian:

triqs::gfs::tail_fitter::fit_hermitian
======================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename M, int R, int R2>`
       | std::pair<arrays::array<dcomplex, R>, double> :red:`fit_hermitian` (M const & :param:`m`,
       |   array_const_view<dcomplex, R> :param:`g_data`,
       |   int :param:`n`,
       |   bool :param:`normalize`,
       |   array_const_view<dcomplex, R2> :param:`known_moments`,
       |   std::optional<long> :param:`inner_matrix_dim` = {})