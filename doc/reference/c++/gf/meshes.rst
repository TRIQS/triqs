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


The four basic meshes
=====================


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

The domain is discrete. The Matsubara frequencies are :math:`\omega_n=\frac{(2n+1)\pi}{beta}` for fermions and :math:`\omega_n=\frac{2n\pi}{beta}` for bosons. 









