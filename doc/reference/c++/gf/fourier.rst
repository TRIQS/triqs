.. highlight:: c

Fourier transforms
###################

The Fourier transforms from real and imaginary frequencies to times, and inverse, are currently implemented,
along with the analogous transformation from the Legendre expansion to imaginary time and frequencies.

Synopsis and example
======================

**Synopsis** :: 

 lazy_object fourier (gf<imfreq,Target,Opt>            const &)
 lazy_object fourier (gf_const_view<imfreq,Target,Opt> const &)

 lazy_object inverse_fourier (gf<imfreq,Target,Opt>            const &)
 lazy_object inverse_fourier (gf_const_view<imfreq,Target,Opt> const &)

The fourier/inverse_fourier functions do **not** perform the Fourier transformation, 
but returns a small lazy object (basically saying "Fourier Transform of XXX"), 
which is then used in an assignment of a *view* of a gf.

Example

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

    gt() = inverse_fourier(gw); // fills the *View* with the contents of the FFT.
    // NB : the mesh must have the same size.

    // make a new fresh gf, with the same size mesh, from the FFT of gt
    auto gw2 = make_gf_from_fourier(gt);
  }

Note that : 

* the LHS of the = must be a view, since the RHS can not compute the domain of the function
  (how many points to use ?).

* In the make_gf_from_fourier function, choice is explicitly made to generate a new gf with the same number of points in the mesh.


Convention 
===========

For real time/frequency:

    .. math:: \tilde G(\omega)=\int_{-\infty}^\infty dt G(t)e^{i\omega t}

    .. math:: G(t)=\int_{-\infty}^\infty \frac{d\omega}{2\pi} \tilde G(\omega)e^{-i\omega t}

For Matsubara (imaginary) time/frequency:

    .. math:: \tilde G(i\omega_n)=\int_{0}^\beta d\tau G(t)e^{i\omega_n \tau}

    .. math:: G(\tau)=\sum_{n=-\infty}^\infty \frac{1}{\beta} \tilde G(i\omega_n)e^{-i\omega_n \tau}

The :math:`\omega_n`'s are :math:`\frac{(2n+1)\pi}{\beta}` for fermions, :math:`\frac{2n\pi}{\beta}` for bosons (as :math:`G(\tau+\beta)=-G(\tau)` for fermions, :math:`G(\tau)` for bosons). 


*
.. toctree::
  
   :maxdepth: 1

   fourier_impl_notes




