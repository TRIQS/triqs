.. highlight:: c

HDF5 write/read convention
================================


All TRIQS objects that can be h5-serialized (i.e. put into an hdf5 file and reconstructed from it)
follow the following convention.

Convention
--------------

For each h5-serializable TRIQS object, one defines **free functions** *h5_read*, *h5_write* with the synopsis ::

  // Write the object into the group or file F, either directly (for array, basic type)
  // or by opening a subgroup and writing its components into this subgroup. 
  h5_read  ( h5::group_or_file F, std::string const & Name, MyNiceObject & obj);

  // The inverse operation. It fills the object from the file. The object must exists before.
  h5_write ( h5::group_or_file F, std::string const & Name, MyNiceObject const & obj);                                                                        |

* h5::group_or_file is a little opaque object that can be transparently constructed
  from the two types H5::Group and H5::H5File (respectively the Group and the File in the HDF5 library).

* Recommended practice : **define these functions as friend functions of the class**.
  
  *Rational* : they will access the data and by resolved by ADL.

Example : 
----------

Suppose I have an object with one array A and a string: 

.. compileblock::

  #include <triqs/arrays.hpp>
  #include <string>
  namespace tqa= triqs::arrays;

  class MyNiceObject { 
     tqa::array<double,2> A;
     double x;
     std::string s;
    public :
     MyNiceObject(): A(2,3), x(3), s("wonderful") { A() = 3;}
    
     friend void h5_write (triqs::h5::group fg, std::string subgroup_name, MyNiceObject const & obj) { 
      auto g = fg.create_group(subgroup_name);
      h5_write(g,"A",obj.A); 
      h5_write(g,"x",obj.x); 
      h5_write(g,"s",obj.s);
     }
     
     friend void h5_read  (triqs::h5::group fg, std::string subgroup_name, MyNiceObject & obj){
      auto g = fg.open_group(subgroup_name);
      h5_read(g,"A",obj.A); 
      h5_read(g,"x",obj.x); 
      h5_read(g,"s",obj.s);
     }
  };
 
  int main() { 
   H5::H5File file("ess.h5", H5F_ACC_TRUNC );
   MyNiceObject a;
   h5_write(file, "a1", a);
   h5_write(file, "a2", a);
  }

The generated file looks like::

  > h5ls -r ess.h5 
  /                        Group
  /a1                      Group
  /a1/A                    Dataset {2, 3}
  /a1/s                    Dataset {SCALAR}
  /a1/x                    Dataset {SCALAR}
  /a2                      Group
  /a2/A                    Dataset {2, 3}
  /a2/s                    Dataset {SCALAR}
  /a2/x                    Dataset {SCALAR}

and the full dump as ::

 HDF5 "ess.h5" {
 GROUP "/" {
   GROUP "a1" {
      DATASET "A" {
         DATATYPE  H5T_IEEE_F64LE
         DATASPACE  SIMPLE { ( 2, 3 ) / ( 2, 3 ) }
         DATA {
         (0,0): 3, 3, 3,
         (1,0): 3, 3, 3
         }
      }
      DATASET "s" {
         DATATYPE  H5T_STRING {
               STRSIZE 9;
               STRPAD H5T_STR_NULLTERM;
               CSET H5T_CSET_ASCII;
               CTYPE H5T_C_S1;
            }
         DATASPACE  SCALAR
         DATA {
         (0): "wonderful"
         }
      }
      DATASET "x" {
         DATATYPE  H5T_IEEE_F64LE
         DATASPACE  SCALAR
         DATA {
         (0): 3
         }
      }
   }
   GROUP "a2" {
      DATASET "A" {
         DATATYPE  H5T_IEEE_F64LE
         DATASPACE  SIMPLE { ( 2, 3 ) / ( 2, 3 ) }
         DATA {
         (0,0): 3, 3, 3,
         (1,0): 3, 3, 3
         }
      }
      DATASET "s" {
         DATATYPE  H5T_STRING {
               STRSIZE 9;
               STRPAD H5T_STR_NULLTERM;
               CSET H5T_CSET_ASCII;
               CTYPE H5T_C_S1;
            }
         DATASPACE  SCALAR
         DATA {
         (0): "wonderful"
         }
      }
      DATASET "x" {
         DATATYPE  H5T_IEEE_F64LE
         DATASPACE  SCALAR
         DATA {
         (0): 3
         }
      }
   }
 }
 }



