Manipulating HDF5 archives 
================================


HDF5 ("*Hierarchical Data Format*") is a portable scientific data format. It is used to store data such as the Green's functions produced during a simulation either in c++ or in python. Irrespective of the language used to produce it, the data stored in an HDF5 archive can be loaded in Python or c++, or even dumped to a text file for a quick glimpse. The HDF5 format also allows for data **compression**. 

In the following subsections, we give a very quick tutorial on HDF5 archives. Look :doc:`here <../../reference/hdf5/contents>` for a more comprehensive reference.



.. toctree::
   :maxdepth: 5

   py_tutorial
   cpp_tutorial


.. note::
   The keys of dictionaries written to hdf5 files in Python are currently
   assumed to be strings. Undesirable behaviour may occur for other 
   dictionaries with non-string keys! Should you need support for more general
   dictionaries, please contact us.

