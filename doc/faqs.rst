
.. _faqs:

Frequently-asked questions
==========================

Should you be encountering issues with TRIQS, please first take a look our issue page on GitHub: `<https://github.com/TRIQS/triqs/issues>`_ and make sure it has not been solved in previous posts. 

Q: How do I write my GF data to text files, e.g. to plot using other plotting tools?
----------------------------------------------------------------------------------------

A: We recommend simply saving the GF data using numpy functions. 
For example, to simply save the imaginary part of the self-energy::

    import numpy as np
    np.savetxt(textfilename,sigma_iw.data.imag)

A more complicated example where the mesh information is also stored is::

    from triqs.gf import *
    import numpy as np

    g = GfImFreq(beta=10, indices = [0,1])
    g << inverse(iOmega_n)

    # Mesh as a NumPy array (n_points,1)
    mesh = np.array([p for p in g.mesh]).reshape(-1,1).imag

    # Im(Data) as a NumPy array (n_points, len(indices)*len(indices))
    data = g.data.imag.reshape((g.data.shape[0],-1))

    mesh_and_data = np.hstack((mesh,data))

    np.savetxt("test.dat",mesh_and_data)

Q: How do I save the triqs hash and script for debugging purposes?
------------------------------------------------------------------

A: Simply add this to your script::

    from triqs import version
    if mpi.is_master_node():
      Results = HDFArchive(filename+".h5",'a')
      if "log" not in Results: Results.create_group("log")
      log = Results["log"]
      log["version"] = version.release
      log["git_hash"] = version.git_hash
      log["script"] = open(sys.argv[0]).read() # read myself !
      del Results
