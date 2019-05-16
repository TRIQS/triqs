..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_view__constructor:

triqs::gfs::gf_view::constructor
================================

*#include <triqs/gfs/gf_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf_view` (gf_view<Var, Target> const & :param:`x`)

    2. | :red:`gf_view` (gf_view<Var, Target> && )

    3. | :red:`gf_view` ()

    4. | :red:`gf_view` (gf_const_view<Var, Target> const & :param:`g`)

    5. | :red:`gf_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` const & :param:`g`)

    6. | :red:`gf_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` & :param:`g`)

    7. | :red:`gf_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` && :param:`g`) noexcept

    8. | :green:`template<typename ArrayType>`
       | :red:`gf_view` (gf_view::mesh_t :param:`m`,
       |        ArrayType && :param:`dat`,
       |        gf_view::indices_t const & :param:`ind` = indices_t{})

    9. | :green:`template<typename ArrayType>`
       | :red:`gf_view` (gf_view::mesh_t :param:`m`, ArrayType const & :param:`dat`)

Documentation



 **1)**   Copy



 **2)**   Move



 **4)**   NO DOC



 **5)**   NO DOC



 **8)**   Builds a view on top of a mesh, a data array



 **9)**   Builds a view on top of a mesh, a data array





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`ArrayType` Type of the data array


Parameters
^^^^^^^^^^

 * :param:`dat` Data array