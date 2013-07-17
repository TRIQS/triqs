.. highlight:: c

Fourier transforms
###################

Convention
==============

    .. math:: \tilde f(\omega)=\int_{-\infty}^\infty dt f(t)e^{i\omega t}

    .. math:: f(t)=\int_{-\infty}^\infty \frac{d\omega}{2\pi} \tilde f(\omega)e^{-i\omega t}


Effect of a TF on the tail
===========================

The tail is unchanged during a TF, but its value is used to limit the errors. 

The components 1 and 2 of the tail (:math:`t_1` and :math:`t_2`) are used to improve the computation of the GF in the following way:
in the large :math:`\omega` limit, 
    .. math:: G(\omega)\simeq \frac{t_1}{\omega}+\frac{t_2}{\omega^2}\simeq \frac{a_1}{\omega+i}+\frac{a_2}{\omega-i}
with :math:`a_1=\frac{t_1+it_2}{2}` and :math:`a_2=\frac{t_1-it_2}{2}`. 

As these large w terms are badly taken into account if we naively Fourier transform the function described by its values on the mesh in w, we substract them, do the Fourier transform and add their Fourier transform to the result. 

We use the following Fourier tranforms:

    .. math:: \frac{2a}{\omega^2+a^2} \leftrightarrow e^{-a|x|}

    .. math:: \frac{1}{\omega+i} \leftrightarrow -i e^{-x} \theta(x)

    .. math:: \frac{1}{\omega-i} \leftrightarrow i e^{x} \theta(-x)

For the inverse Fourier transform, the inverse procedure is used. 




  

