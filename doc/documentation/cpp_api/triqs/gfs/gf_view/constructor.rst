..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_view_constructor:

triqs::gfs::gf_view::constructor
================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Copy constructor`
       | :red:`gf_view` (gf_view<Var, Target> const & :param:`x`)

    2. | :cppbrief:`Move constructor`
       | :red:`gf_view` (gf_view<Var, Target> && )

    3. | :red:`gf_view` ()

    4. | :cppbrief:`Allow to construct a view from a gf with a different evaluator, except const_views ...`
       | :red:`gf_view` (gf_const_view<Var, Target> const & :param:`g`)

    5. | :cppbrief:`Makes a view`
       | :red:`gf_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` const & :param:`g`)

    6. | :cppbrief:`Makes a view`
       | :red:`gf_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` & :param:`g`)

    7. | :cppbrief:`Makes a view`
       | :red:`gf_view` (:ref:`gf\<Var, Target\> <triqs__gfs__gf>` && :param:`g`) noexcept

    8. | :cppbrief:`Construct from mesh, data, ....`
       | :green:`template<typename D>`
       | :red:`gf_view` (gf_view::mesh_t :param:`m`,
       |   D && :param:`dat`,
       |   gf_view::indices_t const & :param:`ind` = indices_t{})

    9. | :cppbrief:`Construct from mesh, data. Only for partial_eval`
       | :green:`template<typename D>`
       | :red:`gf_view` (gf_view::mesh_t :param:`m`, D const & :param:`dat`)

Documentation



 **4)**          Makes a view