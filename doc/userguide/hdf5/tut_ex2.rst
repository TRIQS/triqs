.. _hdf5_tut_ex2:


Example 2: Storing Green's functions
----------------------------------------------

What about more complex objects? The good news is that **hdf-compliant** objects, such as Green's functions, can be stored easily as well:

.. code-block:: python

   from h5 import HDFArchive
   from triqs.gf import GfImFreq

   # Define a Green function 
   G = GfImFreq ( indices = [1], beta = 10, n_points = 1000)
         
   # Opens the file myfile.h5, in read/write mode
   R = HDFArchive('myfile.h5', 'w')
   # Store the object G under the name 'g1' and mu
   R['g1'] = G
   R['mu'] = 1.29
   del R # closing the files (optional: file is closed when the R reference is deleted)

Of course, we can retrieve G as easily:

.. code-block:: python

   from h5 import HDFArchive
   from triqs.gf import GfImFreq

   R = HDFArchive('myfile.h5', 'r')  # Opens the file myfile.h5 in readonly mode 
   G = R['g1'] # Retrieve the object named g1 in the file as G

   # ... ok now I can work with G

This time, the structure of the HDF file is ::

 $>h5ls -r  myfile.h5 
 /                        Group
 /g1                      Group
 /g1/Data                 Dataset {2, 2, 1000}
 /g1/Indices              Dataset {2}
 /g1/Mesh                 Group
 /g1/Mesh/Beta            Dataset {SCALAR}
 /g1/Mesh/Statistic       Dataset {SCALAR}
 /g1/Mesh/TypeGF          Dataset {SCALAR}
 /g1/Mesh/array           Dataset {1000}
 /g1/Name                 Dataset {SCALAR}
 /g1/Note                 Dataset {SCALAR}
 /mu                      Dataset {SCALAR}
 

.. hint:: How does this work ?  
 
 The Green function implements (as detailed in :ref:`HDF_Protocol`)
 
 * a method :func:`__reduce_to_dict__` that  reduces to the Green function to a dictionary containing a mesh (Mesh), the data (Data), the indices (Indices) and so on.
 * a classmethod :func:`__factory_from_dict__` that reconstructs  the Green function from this dictionary.

