
.. _hdf5_tut_ex2:


Example 2 : A Green function
----------------------------------------------

What about more complex objects ?
The good news is that **hdf-compliant** objects can be stored easily as well.

We can store a Green function in an hdf5 file :download:`[file] <./tut_ex2.py>`:

.. literalinclude:: tut_ex2.py

Of course, we can retrieve G as easily :download:`[file] <./tut_ex2b.py>`:

.. literalinclude:: tut_ex2b.py

The structure of the HDF file is this time ::

 MAC:~>h5ls -r  myfile.h5 
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
 /g1/Tail                 Group
 /g1/Tail/Indices         Dataset {2}
 /g1/Tail/OrderMax        Dataset {SCALAR}
 /g1/Tail/OrderMaxMAX     Dataset {SCALAR}
 /g1/Tail/OrderMinMIN     Dataset {SCALAR}
 /g1/Tail/array           Dataset {13, 2, 2}
 /mu                      Dataset {SCALAR}
 

.. hint:: How does this work ?  
 
 The Green function implements (as detailed in :ref:`HDF_Protocol`)
 
 * a method :func:`__reduce_to_dict__` that  reduces to the Green function to a dictionary containing a mesh (Mesh), a tail (Tail), the data (Data), the indices (Indices) and so on. 
 * a classmethod :func:`__factory_from_dict__` that reconstructs  the Green function from this dictionary.

