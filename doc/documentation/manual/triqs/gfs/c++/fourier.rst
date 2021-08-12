.. highlight:: c

Fourier transforms
##################

Synopsis
--------

Here is a synopsis (V = imfreq, imtime, refreq, retime)::

 auto fourier(gf_const_view<V, T> g);
 auto fourier(gf_const_view<V, T> g, array_const_view<dcomplex, 1 + T::rank> known_moments);

 gf<adj(V), T> make_gf_from_fourier(gf_const_view<V, T> g);
 gf<adj(V), T> make_gf_from_fourier(gf_const_view<V, T> g, V m);
 gf<adj(V), T> make_gf_from_fourier(gf_const_view<V, T> g, V m, array_const_view<dcomplex, 1 + T::rank> known_moments);

fourier
-------

The fourier function does **not** perform the Fourier transformation,
but returns a small lazy object (basically saying "Fourier Transform of XXX"),
which is then used in an assignment of a *view* of a gf.

The reason is the following: when putting e.g. a Fourier transform of a function in time, say gt,
into a Green function in frequencies, say gw, we want to say something like::

  gw = fourier(gt); // ??? (1)

However, if the fourier function performs the transformation, how could it know the details
of the mesh of gw ? That information is not available when calling *fourier*.

Since *fourier* returns a small lazy object, the library can then rewrite (1) internally into something like ::

  call_the_fourier_implementation(gt, gw);

where all the information about the mesh of gw is now available to the implementation.

Moreover, since fourier(gt) does not possess a domain (for the same reason), (1)
makes no sense: RHS of gf assignment requires a domain (cf concepts).
We therefore use *a view* as LHS::

   gw() = fourier(gt); // correct usage.

**make_gf_from_fourier**

In the case where we want to create a *new* container from the fourier transform of gt,
we can use the function make_gf_from_fourier.

Example
-------

.. literalinclude:: /documentation/examples/triqs/gfs/fourier_0.cpp
:language: cpp

.. literalinclude:: /documentation/examples/triqs/gfs/fourier_0.output

.. toctree::
   :maxdepth: 1

   fourier_impl_notes
