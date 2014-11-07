
.. _faqs:

Frequently-asked questions
==========================

Q: How do I write my GF data to text files, e.g. to plot using other plotting tools?

A: We recommend simply saving the GF data using numpy functions. For example, to save 
the imaginary part of the self-energy::

    import numpy as np
    np.savetxt(textfilename,sigma_iw.data.imag)
