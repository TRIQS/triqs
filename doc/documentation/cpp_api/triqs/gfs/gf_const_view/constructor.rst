..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_const_view__constructor:

triqs::gfs::gf_const_view::constructor
======================================

*#include <triqs/gfs/gf_const_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf_const_view` (gf_const_view<Var, Target> const & :param:`x`)

    2. | :red:`gf_const_view` (gf_const_view<Var, Target> && )

    3. | :red:`gf_const_view` ()

    4. | :red:`gf_const_view` (:ref:`gf_view\<Var, Target\> <triqs__gfs__gf_view>` const & :param:`g`)

    5. | :red:`gf_const_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` const & :param:`g`)

    6. | :red:`gf_const_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` & :param:`g`)

    7. | :red:`gf_const_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` && :param:`g`) noexcept

    8. | :green:`template<typename ArrayType>`
       | :red:`gf_const_view` (gf_const_view::mesh_t :param:`m`, ArrayType const & :param:`dat`)

    9. | :green:`template<typename ArrayType>`
       | :red:`gf_const_view` (gf_const_view::mesh_t :param:`m`,
       |              ArrayType const & :param:`dat`,
       |              gf_const_view::indices_t const & :param:`ind`)

Documentation



 **1)**   Copy



 **2)**   Move



 **3)**   Empty view, not binded to anything



 **8)**   Builds a const view on top of a mesh, a data array



 **9)**   Builds a const view on top of a mesh, a data array





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`ArrayType` Type of the data array


Parameters
^^^^^^^^^^

 * :param:`dat` Data array

 * :param:`ind` Indices