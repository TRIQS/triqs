..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__make_gf_from_fourier:

triqs::gfs::make_gf_from_fourier
================================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<int N, typename V1, typename V2, typename T, typename OptArgs>`
       | auto :red:`make_gf_from_fourier` (:ref:`gf_const_view\<V1, T\> <triqs__gfs__gf_const_view>` :param:`gin`,
       |                       :ref:`gf_mesh\<V2\> <triqs__gfs__gf_mesh>` const & :param:`mesh`,
       |                       OptArgs const &... :param:`opt_args`)

    2. | :green:`template<int N, typename T>`
       | auto :red:`make_gf_from_fourier` (:ref:`gf_const_view\<triqs::lattice::brillouin_zone, T\> <triqs__gfs__gf_const_view>` :param:`gin`)

    3. | :green:`template<int N, typename G, typename M, int R>`
       | auto :red:`make_gf_from_fourier` (G const & :param:`gin`,
       |                       M const & :param:`m`,
       |                       std::vector<array<dcomplex, R> > const & :param:`known_moments`)

    4. | :green:`template<int N, int Ns, typename V, typename T, typename Args>`
       | auto :red:`make_gf_from_fourier` (:ref:`gf_view\<V, T\> <triqs__gfs__gf_view>` :param:`gin`, Args &&... :param:`args`)

Documentation



 **1)**   FIXME DOC
 // split : No N for 1 mesh ...



 **2)**   -----------------------------------------------------------------------------------------------------

 make_gf_from_fourier : Specialized makers for different meshes

 *-----------------------------------------------------------------------------------------------------



 **3)**   -----------------------------------------------------------------------------------------------------

 make_gf_from_fourier : Block / Block2 Gf

 *-----------------------------------------------------------------------------------------------------



 **4)**   -----------------------------------------------------------------------------------------------------

 make_gf_from_fourier : Give proper overloads for gf and gf_view

 *-----------------------------------------------------------------------------------------------------