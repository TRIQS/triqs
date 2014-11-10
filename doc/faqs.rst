
.. _faqs:

Frequently-asked questions
==========================

Q: How do I write my GF data to text files, e.g. to plot using other plotting tools?

A: We recommend simply saving the GF data using numpy functions. 
For example, to simply save the imaginary part of the self-energy::

    import numpy as np
    np.savetxt(textfilename,sigma_iw.data.imag)

A more complicated example where the mesh informations is also stored is::

    from pytriqs.gf.local import *
    import numpy as np

    g = GfImFreq(beta=10, indices = [0,1])
    g << inverse(iOmega_n)

    # Mesh as a NumPy array (n_points,1)
    mesh = np.array([p for p in g.mesh]).reshape(-1,1).imag

    # Im(Data) as a NumPy array (n_points, len(indices)*len(indices))
    data = g.data.imag.reshape((g.data.shape[0],-1))

    mesh_and_data = np.hstack((mesh,data))

    np.savetxt("test.dat",mesh_and_data)
