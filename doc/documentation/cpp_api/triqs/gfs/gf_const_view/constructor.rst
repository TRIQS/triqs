..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_const_view_constructor:

triqs::gfs::gf_const_view::constructor
======================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy constructor`
       | :red:`gf_const_view` (gf_const_view<Var, Target> const & :param:`x`)

    2. | :cppbrief:`Move constructor`
       | :red:`gf_const_view` (gf_const_view<Var, Target> && )

    3. | :red:`gf_const_view` ()

    4. | :cppbrief:`Makes a const view`
       | :red:`gf_const_view` (:ref:`gf_view\<Var, Target\> <triqs__gfs__gf_view>` const & :param:`g`)

    5. | :cppbrief:`Makes a const view`
       | :red:`gf_const_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` const & :param:`g`)

    6. | :cppbrief:`Makes a const view`
       | :red:`gf_const_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` & :param:`g`)

    7. | :cppbrief:`Makes a const view`
       | :red:`gf_const_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` && :param:`g`) noexcept

    8. | :cppbrief:`Construct from mesh, data, ....`
       | :green:`template<typename D>`
       | :red:`gf_const_view` (gf_const_view::mesh_t :param:`m`,
       |   D const & :param:`dat`,
       |   gf_const_view::indices_t const & :param:`ind`)

    9. | :cppbrief:`Construct from mesh, data.`
       | :green:`template<typename D>`
       | :red:`gf_const_view` (gf_const_view::mesh_t :param:`m`, D const & :param:`dat`)

Documentation