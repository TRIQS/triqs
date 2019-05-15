..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_const_view_target:

triqs::gfs::gf_const_view::target
=================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Shape of the target`
       | gf_const_view::target_and_shape_t :red:`target` () const





        auto target_shape() const { return _data.shape().template front_mpop<arity>(); }  drop arity dims