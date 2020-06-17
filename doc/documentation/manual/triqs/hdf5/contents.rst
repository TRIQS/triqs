
.. _hdf5_base:
.. module:: h5

**[triqs/h5]** The HDF5 format
###################################


In TRIQS, the main data storage format is `HDF5 <http://www.hdfgroup.org/HDF5/>`_ (Hierarchical Data Format v5).

HDF5 ("*Hierarchical Data Format*") is a portable scientific data format. It is used to store data such as the Green's functions produced during a simulation either in c++ or in python. Irrespective of the language used to produce it, the data stored in an HDF5 archive can be loaded in Python or c++, or even dumped to a text file for a quick glimpse. The HDF5 format also allows for data **compression**. 

.. note::

	Using HDF5 format has several advantages :

	* Most basic objects of TRIQS, like Green function, are hdf-compliant.
	* TRIQS provides a **simple and intuitive interface HDFArchive** to manipulate them.
	* HDF5 is **standard**, well maintained and widely used.
	* HDF5 is **portable** from various machines (32-bits, 64-bits, various OSs, etc)
	* HDF5 can be read and written in **many langages** (python, C/C++, F90, etc), beyond TRIQS. One is not tied to a particular program.
	* Simple operations to explore and manipulate the tree are provided by simple unix shell commands (e.g. h5ls, h5diff).
	* It is a binary format, hence it is compact and has compression options.
	* It is to a large extent **auto-documented**: the structure of the data speaks for itself.

.. note::
   The keys of dictionaries written to hdf5 files in Python are currently
   assumed to be strings. Undesirable behaviour may occur for other 
   dictionaries with non-string keys! Should you need support for more general
   dictionaries, please contact us.

An hdf5 file can be thought of as a file-system. As such, its structure is that of a **tree**, where: 

* **Leaves** of the tree are basic types: scalars (int, long, double, string) and rectangular arrays of these scalars (any dimension: 1,2,3,4...).
* Subtrees (branches) are called **groups**
* Groups and leaves have a name, so an element of the tree has naturally a **path**: 
  e.g. /group1/subgroup2/leaf1 and so on.
* Any path (groups, leaves) can be optionally tagged with an **attribute**, in addition to their name,
  typically a string (or any scalar)


`Any data with a tree structure with arrays or scalar leaves can be naturally stored in hdf5 files`.

To be more precise, we call hereafter a data **hdf-compliant** `iif` it can be reversibly transformed into 

* a tree structure with scalar/arrays leaves.
* or a dictionary `keys -> values`, where `keys` are strings (field names) and `values` are scalars, arrays or any other hdf-compliant data.

Due to the recursive nature of trees, the two definitions are equivalent.
An hdf-compliant data can be naturally saved in a subgroup of an HDF5 file by adding (cf example below) new leaves for all scalar and arrays
and new subgroup for other hdf-compliant data. 

.. toctree::
   :maxdepth: 5

   ref
   ./../../userguide/hdf5/hdf5_tutorial


