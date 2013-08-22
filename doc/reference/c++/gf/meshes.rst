.. highlight:: c



Meshes  
#######


The linear meshes
==================


The mesh kind
--------------

This option is particularly important for the Matsubara Green functions in imaginary time. 

Briefly, if we want to describe a function on an interval:

* ``full_bins`` includes both endpoints, 

* ``half_bins`` includes none of the endpoints

* ``without_last`` includes only the first endpoint.

We then have to be careful for example when we fourier transform the function (to not take twice the same point).

How to access to a mesh point with its index
---------------------------------------------

.. compileblock::

     #include <triqs/gfs/refreq.hpp> 
     using namespace triqs::gfs;

     int main() {
      
      //we construct a GF
      double wmin = 0.0;
      double wmax = 1.0;
      int nw = 101;
      auto Gw = make_gf<refreq, scalar_valued>(wmin, wmax, nw);

      //we print the mesh parameters and print te value of the 10th point
      std::cout << "The kind of the mesh is " << Gw.mesh().kind() << std::endl;
      std::cout << "The smallest mesh point value is w_min=" << Gw.mesh().x_min() << std::endl;
      std::cout << "The largest mesh point value is w_max=" << Gw.mesh().x_max() << std::endl;
      std::cout << "The number of mesh points is n=" << Gw.mesh().size() << std::endl;
      std::cout << "Between two consecutive mesh points: delta=" << Gw.mesh().delta() << std::endl;
      std::cout << "The 10th mesh point is w=" << Gw.mesh()[10] << std::endl;

     }

How to access to a mesh point with a value
-------------------------------------------

In this case, we look for the closest mesh point, but can need the distance of the value to the mesh point. 
``windowing`` gives all these informations: 

.. compileblock::

     #include <triqs/gfs/refreq.hpp> 
     using namespace triqs::gfs;

     int main() {
      double wmin = 0.0;
      double wmax = 1.0;
      int nw=101;
      auto Gw= make_gf<refreq, scalar_valued>(wmin, wmax, nw);
      double w=0.25156;
      size_t index; double wd; bool in;
      std::tie(in, index, wd) = windowing ( Gw.mesh(), w);
      std::cout << "Is the point w="<< w <<" in the mesh range ? " << in << std::endl;
      if(in){
       std::cout << "The point before is the " << index << "th" << std::endl;
       std::cout << "The position in the intervall is " << wd << std::endl;
      }
     }




The four basic linear meshes
============================


Real time
----------

The domain is the set of real numbers. 
By default, the mesh kind is ``full_bins``. 

Be careful to the value of a function at a point in case of discontinuities: is its value equal to the limit from below ? To the limit from above ? By none of these limits ?


Real frequency
---------------

The domain is the set of real numbers. 
By default, the mesh kind is ``full_bins``. 


Matsubara time
---------------

The domain is (approximatively) the set of real numbers between 0 and :math:`\beta`. 
In fact, other points are also in the domain, but the values at these points are given by the values on this restricted domain. 
:math:`G(\tau+\beta)=-G(\tau)` for fermions, :math:`G(\tau+\beta)=G(\tau)` for bosons.

The limits from above or below at these both points can be different. 
Depending on what one needs, we can choose ``full_bins``,  ``half_bins``  or ``without_last``.


Matsubara frequency
--------------------

The domain is discrete. The Matsubara frequencies are :math:`\omega_n=\frac{(2n+1)\pi}{\beta}` for fermions and :math:`\omega_n=\frac{2n\pi}{\beta}` for bosons. 



Products of meshes
===================

We detail the case of a two mesh product, but what follows is true for any number of meshes. 

A mesh point can be labelled by a linear index, or by a tuple of indices. Each mesh point correspond to a point of the domain, which is a tuple of points of the subdomains. 
We can navigate between these representations, through ``closest_mesh_pt``, ``get_closest_pt``, ``index_to_linear``,...


How to access to the closest mesh point
---------------------------------------

.. compileblock::

     #include <triqs/gfs/two_real_times.hpp> 
     using namespace triqs::gfs;

     int main() {
      double tmax = 1.0;
      int nt = 101;
      auto Gtt = make_gf<two_real_times>(tmax, nt, triqs::arrays::make_shape(1,1));

      //does not work for instance
      //double t1 = 0.256, t2 = 0.758;
      //Gtt(closest_mesh_pt(i1,i2)) = 1.5;
      
     }


How to access to a mesh point with its index
---------------------------------------------

.. compileblock::

     #include <triqs/gfs/two_real_times.hpp> 
     using namespace triqs::gfs;

     int main() {
      double tmax = 1.0;
      int nt = 101;
      auto Gtt = make_gf<two_real_times>(tmax, nt, triqs::arrays::make_shape(1,1));

      int i1 = 14, i2 = 86;
      Gtt.on_mesh(i1, i2) = 1.8;
      std::cout << Gtt.on_mesh(i1, i2)(0,0) << std::endl;
      
     }









