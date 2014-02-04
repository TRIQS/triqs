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


.. compileblock:: 

  #include <Python.h>
  #include <iostream>
  #include <triqs/gfs/local/tail.hpp>
  int main(){
   int N1=1, N2=1;
   triqs::gfs::local::tail t(N1,N2);
   t.mask_view() = 5;//only coeffs from -1 to 5 are meaningful
   std::cout << t(0) << std::endl;
   t(2) = .5;
   std::cout << t << std::endl;
  }



Fitting the tail of a Green's function
---------------------------------------

Given an imaginary-frequency Green's function, one can compute the moments of its high-frequency tail with the function ``set_tail_from_fit``:


.. compileblock:: 

  #include <triqs/gfs.hpp>
  #include <triqs/gfs/local/fit_tail.hpp>
  using namespace triqs::gfs; 
  int main(){
   triqs::clef::placeholder<0> iom_;
   double beta =10;
   int N=100;

   auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
   gw(iom_) << 1/(iom_-1);

   size_t wn_min=50; //frequency to start the fit
   size_t wn_max=90; //final fitting frequency (included)
   int n_moments=4;  //number of moments in the final tail (including known ones)
   int  size=1; //means that we know one moment
   int order_min=1; //means that the first moment in the final tail will be the first moment
   auto known_moments = local::tail(make_shape(1,1), size, order_min); //length is 0, first moment to fit is order_min
   known_moments(1)=1.;//set the first moment
   set_tail_from_fit(gw, known_moments, n_moments, wn_min, wn_max, true);//true replace the gf data in the fitting range by the tail values
   std::cout << gw.singularity() << std::endl;
  }


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
