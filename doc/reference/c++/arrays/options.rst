.. highlight:: c

.. _option_template:

Options template parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The `Option::options` template can take several parameters  that determine various  aspects of `array` (resp. `matrix`, `vector`)  at **compile time**.
These parameters can be given in any order in the `options` template, but only at most only (they all have a default, that can also be changed
using macros for maximum flexibility).
These options parameters determine : 

* the storage order of indices in memory at **compile time**.

 ===============================     =========================================================================================== 
 Template parameter                  Meaning
 ===============================     =========================================================================================== 
 `Tag::C`     `[default]`            C-style order            
 `Tag::Fortran`                      Fortran-style order ( NB the base index is still 0, not 1 !)
 memory_order<i0,i1,i2,... >         The tuple <i0,i1,i2...> of size rank determines the storage order of indices in memory
 memory_order_p<P>                   P is a triqs::arrays::Permutations::permutation that determines the storage order in memory
                                      P[0] is the fastest index, P[rank - 1]  the slowest index 
 ===============================     =========================================================================================== 

  Example of custom order ::

    custom<Permutation<0,1,2> >  // same as index_order_Fortran, the first index is the fastest
    custom<Permutation<2,1,0> >  // same as index_order_C, the last index is the fastest
    custom<Permutation<1,2,0> >  // if indices are (i,j,k)  : index j is fastest, then k, then i 
 
* whether bound check are done while accessing array elements or slicing the arrays.

 ===============================     ==================================================================================================== 
 Template parameter                  Meaning
 ===============================     ==================================================================================================== 
 `Tag::NoBoundCheck` `[default]`     Do no bound check at all. This is the default, for speed !            
 `Tag::BoundCheck`                   Do bound checks. Boundary errors throw triqs::arrays::key_error exception (See ??)
 ===============================     ==================================================================================================== 

 The macro `TRIQS_ARRAYS_ENFORCE_BOUNDCHECK` change the default to `Tag::BoundCheck` (for debugging purposes).

 Rational :  one may want for some array to activate bound check and catch the exception, while keeping faster unchecked arrays elsewhere.

* whether arrays are initialized at construction.

 ===============================     ==================================================================================================== 
 Template parameter                  Meaning
 ===============================     ==================================================================================================== 
 `Tag::no_init` `[default]`          No initialization of the array at construction (fastest).
 `Tag::default_init`                 Initialize with the default constructor of ValueType
 `Tag::nan_inf_init`                 Initialize to nan for floating type and std::limits<ValueType>::infinity() for integer type
                                     as in D e.g. mainly for detecting easily lack of initialization errors.
 ===============================     ==================================================================================================== 

 Note that : 

  * The  macro `TRIQS_ARRAYS_ENFORCE_INIT_NAN_INF` change the default to `Tag::nan_inf_init`.
  * The  macro `TRIQS_ARRAYS_ENFORCE_INIT_DEFAULT` change the default to `Tag::default_init`.

* Several simple aliases are defined in the namespace Option for the most current cases :

  =============================== ===============================    
  Option                          aliases                            
  =============================== ===============================    
  Option::C                       Option::options<`Tag::C`>            
  Option::Default                 Option::options<`Tag::C`>            
  Option::Fortran                 Option::options<`Tag::Fortran>`       
  =============================== ===============================    


* Examples :: 

   array<long, 3, Option::options<Option::memory_order <2,1,0> > > A0 (2,3,4);
   array<long, 3, Option::options<Option::memory_order <0,1,2> > > A1 (2,3,4);
   array<long, 3, Option::options<Option::memory_order <1,0,2> > > A2 (2,3,4);
   array<long, 3, Option::Fortran > A4 (2,3,4);



