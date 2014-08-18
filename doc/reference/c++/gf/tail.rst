.. highlight:: c

.. _gf_tail:

High frequency tail
===========================

Definition 
----------------------

The tail of a Green's function is defined as the behavior of the Green's
function :math:`G` at large Matsubara frequencies, namely

.. math::  \mathbf{G}(i\omega_n) \stackrel {=}{\infty} \mathbf{a}_{-1}\cdot i\omega_n + \mathbf{a}_{0} +\mathbf{a}_{1}\cdot \frac{1}{ i\omega_n} +\mathbf{a}_{2}\cdot \frac{1}{ (i\omega_n)^2} +\dots  

Generically, the tail is parametrized by matrix-valued coefficients
:math:`\mathbf{a}_{i}` (of size :math:`N_1\times N_2`\ )

.. math:: t = \sum_{i=o_{min}}^{o_{max}} \mathbf{a}_i (i\omega_n)^{-i}

Implementation
--------------

In TRIQS, the tail is implemented as an object ``tail``. Here is a simple example of use: 


.. triqs_example:: ./tail_0.cpp
Fitting the tail of a Green's function
---------------------------------------

Given an imaginary-frequency Green's function, one can compute the moments of its high-frequency tail with the function ``set_tail_from_fit``:


.. triqs_example:: ./tail_1.cpp
The full documentation of ``set_tail_from_fit`` is :doc:`here<set_tail_from_fit>`.

API
****
Here are the main methods of the ``tail`` class:

+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| Member                          | Description                                                                             | Type                     |
+=================================+=========================================================================================+==========================+
| data()                          | 3-dim array of the coefficients: ``data(i,n,m)`` :math:`=(\mathbf{a}_{i+o_{min}})_{nm}` | data_view_type           |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| mask_view()                     | 2-dim (:math:`N_1 \times N_2`) array of the maximum non-zero indices                    | mask_view_type           |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| order_min()                     | minimum order                                                                           | long                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| order_max()                     | maximum order                                                                           | long                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| size()                          | first dim of data()                                                                     | size_t                   |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| shape()                         | shape of data()                                                                         | shape_type               |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| smallest_nonzeros()             | order of the smallest_nonzero coefficient                                               | long                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| is_decreasing_at_infinity()     | true if the tail is decreasing at infinity                                              | bool                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| operator() (int n)              | matrix_valued coefficient :math:`(\mathbf{a}_i)_{nm}`                                   | mv_type                  |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| get_or_zero (int n)             | matrix_valued coefficient :math:`(\mathbf{a}_i)_{nm}`                                   | const_mv_type            |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| evaluate(dcomplex const &omega) | value of the tail at frequency omega                                                    | arrays::matrix<dcomplex> |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
    
   
The tail is DefaultConstructible, H5Serializable and BoostSerializable.
