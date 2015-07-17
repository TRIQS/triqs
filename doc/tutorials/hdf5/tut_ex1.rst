.. _hdf5_tut_ex1:

Example 1: Storing basic data types
----------------------------------------

The simplest way to interact with HDF5 files is to use the TRIQS HDFArchive class, which 
represents the tree structure of the file in a way similar to a dictionary.

Let us start with a very simple example :download:`[file] <./tut_ex1.py>`:

.. literalinclude:: tut_ex1.py

Run this and say ::
 
 MyComputer:~>h5ls -r  myfile.h5 
 /                        Group
 /S                       Group
 /S/a                     Dataset {SCALAR}
 /S/b                     Dataset {3}
 /mu                      Dataset {SCALAR}
 
This shows the tree structure of the file.  We see that :

* `mu` is stored at the root `/`
* `S` is a subgroup, containing `a` and `b`.
* For each leaf, the type (scalar or array) is given.

To dump the content of the file use, for example, the following: (see the HDF5 documentation for more information) ::

 MyComputer:~>h5dump myfile.h5 
 HDF5 "myfile.h5" {
 GROUP "/" {
   GROUP "S" {
      DATASET "a" {
         DATATYPE  H5T_STRING {
               STRSIZE H5T_VARIABLE;
               STRPAD H5T_STR_NULLTERM;
               CSET H5T_CSET_ASCII;
               CTYPE H5T_C_S1;
            }
         DATASPACE  SCALAR
         DATA {
         (0): "a string"
         }
      }
      DATASET "b" {
         DATATYPE  H5T_STD_I32LE
         DATASPACE  SIMPLE { ( 3 ) / ( 3 ) }
         DATA {
         (0): 1, 2, 3
         }
      }
   }
   DATASET "mu" {
      DATATYPE  H5T_IEEE_F64LE
      DATASPACE  SCALAR
      DATA {
      (0): 1.29
      }
   }
 }
 }


