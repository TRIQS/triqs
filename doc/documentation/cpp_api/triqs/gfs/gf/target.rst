..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_target:

triqs::gfs::gf::target
======================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Shape of the target`
       | gf::target_and_shape_t :red:`target` () const





        auto target_shape() const { return _data.shape().template front_mpop<arity>(); }  drop arity dims