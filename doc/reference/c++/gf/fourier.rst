.. highlight:: c

Fourier transforms
###################

The Fourier transforms from real and imaginary frequencies to times, and inverse, are currently implemented,
along with the analogous transformation from the Legendre expansion to imaginary time and frequencies.

Synopsis 
=========

**Synopsis** :: 

 auto fourier (gf<imfreq,Target,Opt>            const &)
 auto fourier (gf_view<imfreq,Target,Opt>       const &)
 auto fourier (gf_const_view<imfreq,Target,Opt> const &)

 auto inverse_fourier (gf<imfreq,Target,Opt>            const &)
 auto inverse_fourier (gf_view<imfreq,Target,Opt>       const &)
 auto inverse_fourier (gf_const_view<imfreq,Target,Opt> const &)

 gf<imfreq, Target, Opt> make_gf_from_fourier(gf<imtime, Target, Opt>            const&);
 gf<imfreq, Target, Opt> make_gf_from_fourier(gf_view<imtime, Target, Opt>       const&);
 gf<imfreq, Target, Opt> make_gf_from_fourier(gf_const_view<imtime, Target, Opt> const&);
 
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf<imfreq, Target, Opt>            const&);
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf_view<imfreq, Target, Opt>       const&);
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf_const_view<imfreq, Target, Opt> const&);


**fourier, inverse_fourier**

The fourier/inverse_fourier functions do **not** perform the Fourier transformation, 
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
makes no sense : RHS of gf assignment requires a domain (cf concepts).
We therefore use *a view* as LHS::

   gw() = fourier(gt); // correct usage.

**make_gf_from_fourier, make_gf_from_inverse_fourier**

In the case where we want to create a *new* container from the fourier transform of gt, 
we can use the function make_gf_from_fourier, in which choice is explicitly made to generate a new gf with the same number of points in the mesh.
(Cf example below).

DOC TO BE FINISHED.

Example
=========

.. compileblock::

  #include <triqs/gfs.hpp>
  using namespace triqs::gfs;
  int main() {
    double beta =1, a=1;
    int N=10000;
    auto gw = gf<imfreq> {{beta, Fermion, N}, {1,1}};
    auto gt = gf<imtime> {{beta, Fermion, N}, {1,1}};
    
    triqs::clef::placeholder<0> om_;
    gw (om_) << 1/(om_-a);

    // fills a full *view* of gt with the contents of the FFT
    // NB : the mesh of gt *must* have the same size as the mesh of gw.
    gt() = inverse_fourier(gw); 

    // make a new fresh gf, with the same size mesh, from the FFT of gt
    auto gw2 = make_gf_from_fourier(gt);
  }

Convention 
===========

For real time/frequency:

    .. math:: \tilde G(\omega)=\int_{-\infty}^\infty dt G(t)e^{i\omega t}

    .. math:: G(t)=\int_{-\infty}^\infty \frac{d\omega}{2\pi} \tilde G(\omega)e^{-i\omega t}

For Matsubara (imaginary) time/frequency:

    .. math:: \tilde G(i\omega_n)=\int_{0}^\beta d\tau G(t)e^{i\omega_n \tau}

    .. math:: G(\tau)=\sum_{n=-\infty}^\infty \frac{1}{\beta} \tilde G(i\omega_n)e^{-i\omega_n \tau}

The :math:`\omega_n`'s are :math:`\frac{(2n+1)\pi}{\beta}` for fermions, :math:`\frac{2n\pi}{\beta}` for bosons (as :math:`G(\tau+\beta)=-G(\tau)` for fermions, :math:`G(\tau)` for bosons). 



.. toctree::
  
   :maxdepth: 1

   fourier_impl_notes




