.. highlight:: c

Fourier: implementation notes
#######################################


The FFTW library
---------------------

Documentation on FFTW is on https://www.fftw.org. 
FFTW is a C subroutine library for computing the discrete Fourier transform (DFT) in one or more dimensions, of arbitrary input size, and of both real and complex data. 
It will be used to calculate the (inverse) Fourier transform, in real/imaginary time/frequency, using the fact that the GF values are stored for a finite amount of regularly spaced values. 

The DFT transforms of a sequence of :math:`N` complex numbers :math:`f_0...,f_{N-1}` into a sequence of :math:`N` complex numbers :math:`\tilde f_0...,\tilde f_{N-1}` according to the formula:
    .. math:: \tilde f_k = \sum_{n=0}^{N-1} f_n e^{-i 2 \pi k n / N}.
The inverse DFT formula is
    .. math:: f_n = \frac{1}{N} \sum_{k=0}^{N-1} \tilde f_k e^{i 2 \pi k n / N}.



Implementation in real time/frequency using FFTW
---------------------------------------------------------------

The real time mesh parameters are :math:`t_{min}`, :math:`\delta t` and :math:`N_t`. 
For the real frequency mesh, they are :math:`\omega_{min}`, :math:`\delta \omega` and :math:`N_\omega`. 
The Fourier transform requires :math:`N_\omega=N_t` and :math:`\delta t \delta \omega= \frac{2\pi}{N_t}`. 
The times are :math:`t_k=t_{min}+k\delta t` and the frequencies :math:`\omega_m=\omega_{min}+m\delta \omega`. 

By approximating Eq. :ref:`TF_R` by 
    .. math:: \tilde G(\omega_m) = \delta t \sum_{k=0}^{N_t} G(t_k) e^{i\omega_m t_k}, 
we recognize an inverse DFT (Eq. :ref:`inv_DFT`). To calculate it using FFTW, we first need to prepare the input :math:`\tilde f_k`, then to do the DFT and finally to modify the output to obtain :math:`\tilde G(\omega_m)` using the two formulas:
    .. math:: \tilde f_k = G(t_k) e^{i \omega_{min}t_k},
    .. math:: \tilde G(\omega_m) = \delta t f_m e^{i t_{min}(\omega_m-\omega_{min})}.

Similarly, the inverse transformation is obtained by approximating Eq. :ref:`eq_inv_TF_R` by 
    .. math:: G(t_k)=\frac{\delta\omega}{2\pi}\sum_{m=0}^{N_\omega} \tilde G(\omega_m)e^{-i\omega_m t_k},
we recognize a DFT (Eq. :ref:`DFT`). To calculate it using FFTW, we first need to prepare the input :math:`f_m`, then to do the inverse DFT and finally to modify the output to obtain :math:`G(t_k)`:
    .. math:: f_m = \tilde G(\omega_m) e^{-i t_{min}\omega_m},
    .. math:: G(t_k) = \frac{1}{N_t \delta t}\tilde f_k e^{-i \omega_{min}(t_k-t_{min})}.



Implementation in imaginary time/frequency using FFTW
---------------------------------------------------------------


The imaginary time mesh parameters are :math:`\beta` and :math:`N_\tau`. There is a point both at the beginning
and at the end of the interval and therefore that last point has to be removed for the fourier transform. 
From these parameters, we deduce :math:`\delta\tau=\beta/N_\tau`. 

For the imaginary frequency mesh, the mesh parameters are :math:`\beta`, :math:`n_{min}` and :math:`N_{\omega_n}`. 
From them, we deduce :math:`\delta\omega=\frac{2\pi}{\beta}`. 

The Fourier transform requires :math:`N_\omega=N_\tau`. 
The times are :math:`\tau_k=\tau_{min}+k\delta\tau` and the frequencies :math:`\omega_n=\omega_{min}+n\delta \omega`. 
:math:`\tau_{min}` is either 0 or :math:`\delta\tau/2` depending on the mesh kind.  
:math:`\omega_{min}` is either :math:`\frac{2\pi(n_{min}+1)}{\beta}` or :math:`\frac{2\pi n_{min}}{\beta}` depending on the statistics.

We approximate the TF and its inverse by
    .. math:: \tilde G(i\omega_n) = \delta\tau \sum_{k=0}^{N_\tau} G(\tau_k)e^{i\omega_n \tau_k}
    .. math:: G(\tau_k) = \sum_{n=n_{min}}^{N_\tau} \frac{1}{\beta} \tilde G(i\omega_n)e^{-i\omega_n \tau_k}

We use for the TF:
    .. math:: \tilde f_k = G(\tau_k) e^{i \omega_{min}\tau_k},
    .. math:: \tilde G(i\omega_n) = \frac{\beta}{N_\tau} f_n e^{i \tau_{min}(\omega_n-\omega_{min})}.
and for the inverse TF:
    .. math:: f_m = \frac{1}{\beta}\tilde G(i\omega_n) e^{-i t_{min}\omega_n},
    .. math:: G(t_k) = \tilde f_k e^{-i \omega_{min}(\tau_k-\tau_{min})},


Special case of real functions in time for fermions
...........................................................

In this case, :math:`G(i\omega_n)=conj(G(i\omega_n))` and we only store the values of :math:`G(i\omega_n)` for :math:`\omega_n > 0`. 
The Eq. :ref:`inv_DFT_I` becomes: 

    .. math:: G(\tau)=\sum_{n=0}^\infty \frac{2}{\beta} \tilde G(i\omega_n)\cos(\omega_n \tau)

The inverse TF formulas are in this case 
    .. math:: f_m = \frac{2}{\beta}\tilde G(i\omega_n) e^{-i t_{min}\omega_n},
    .. math:: G(t_k) = \tilde f_k e^{-i \omega_{min}(\tau_k-\tau_{min})},

Special case of real functions in time for bosons
...........................................................

In this case, :math:`G(i\omega_n)=conj(G(i\omega_n))` and we only store the values of :math:`G(i\omega_n)` for :math:`\omega_n \ge 0`. 
The Eq. :ref:`inv_DFT_I` becomes: 

    .. math:: G(\tau)=\frac{1}{\beta} \tilde G(0)+\sum_{n=1}^\infty \frac{2}{\beta} \tilde G(i\omega_n)\cos(\omega_n \tau)

The inverse TF formulas are in this case 
    .. math:: f_0 = \frac{1}{\beta}\tilde G(0),
    .. math:: f_m = \frac{2}{\beta}\tilde G(i\omega_n) \cos(t_{min}\omega_n),
    .. math:: G(t_k) = \tilde f_k e^{-i \omega_{min}(\tau_k-\tau_{min})},


Usage of the tail in the TF
---------------------------

The first and second order high-frequency moments (:math:`t_1` and :math:`t_2`) are used to improve the computation of the GF in the following way:
In the large :math:`\omega` limit,
    .. math:: G(\omega)\simeq \frac{t_1}{\omega}+\frac{t_2}{\omega^2}\simeq \frac{a_1}{\omega+i}+\frac{a_2}{\omega-i}
with :math:`a_1=\frac{t_1+it_2}{2}` and :math:`a_2=\frac{t_1-it_2}{2}`. 

As these large w terms are badly taken into account if we naively Fourier transform the function described by its values on the mesh in w, we substract them, do the Fourier transform and add their Fourier transform to the result. 
The required high-frequency moments are determined from the data through a least-square fitting procedure unless provided.

We use the following Fourier tranforms:

    .. math:: \frac{2a}{\omega^2+a^2} \leftrightarrow e^{-a|x|}

    .. math:: \frac{1}{\omega+i} \leftrightarrow -i e^{-x} \theta(x)

    .. math:: \frac{1}{\omega-i} \leftrightarrow i e^{x} \theta(-x)

For the inverse Fourier transform, the inverse procedure is used. 

In the library, :math:`a` is optimized according to the mesh properties (its size :math:`L=G.mesh().size()` and its precision :math:`\delta = G.mesh().delta()`). 
The requirements are :math:`a \gg \delta\omega` and :math:`a \ll L\delta\omega`, or equivalently :math:`a \gg \delta t` and :math:`a \ll L\delta t`.
Thus, we chose :math:`a=\sqrt{L}\delta\omega`



  

