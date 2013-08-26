.. _welcome:

Welcome
=======

TRIQS is a scientific project that provides a set of tools for the study of
interacting quantum systems. It allows to quickly implement many-body
calculations and explore new theoretical and phenomenological ideas within a
flexible, easy-to-use python environment (à la Matlab). 

Powerful c++ libraries
----------------------

.. code-block:: c++

  #include <triqs/arrays.hpp>
  #include <triqs/arrays/linalg/inverse.hpp>
  #include <triqs/arrays/linalg/determinant.hpp>

  using triqs::arrays::array;  using triqs::arrays::matrix;  using triqs::clef::placeholder;
  int main(){
    placeholder<0> i_;
    placeholder<1> j_;
    matrix<double> A(2,2);
    A(i_,j_) << i_+j_;

    matrix<double> B = inverse(A);
    double C = determinant(A);

    std::cout << "A^(-1) = "<< B << std::endl;
    std::cout << "det(A) = " <<C <<std::endl;
  }

User-friendly python interface
------------------------------

.. plot::
   :include-source:
   :scale: 70

    from pytriqs.gf.local import *
    g = GfReFreq(indices = ['s','d'], window = [-2,2], n_points = 1000)
    g['d','d'] = Omega - 0.3
    g['d','s'] = 0.2
    g['s','d'] = 0.2
    g['s','s'] = inverse( Wilson(1.0) )
    g.invert()

    from pytriqs.plot.mpl_interface import oplot
    oplot( g['d','d'], '-o', RI = 'S', x_window  = (-1.8,1.8), name = "Impurity" )
    oplot( g['s','s'], '-x', RI = 'S', x_window  = (-1.8,1.8), name = "Bath" )
