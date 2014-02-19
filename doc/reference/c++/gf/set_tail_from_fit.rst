Fitting the tail of a Matsubara Green's function
#################################################

API
~~~~~

The tail of a given ``gf<imfreq>/gf<block_index, gf<imfreq>> gw`` can be fitted using the two following functions:

 ``void set_tail_from_fit(gf<imfreq> &gf, tail_view known_moments, int n_moments, size_t n_min, size_t n_max, bool replace_by_fit = false);``

 ``void set_tail_from_fit(gf<block_index, gf<imfreq>> &block_gf, tail_view known_moments, int n_moments, size_t n_min,  size_t n_max, bool replace_by_fit = false);``


where

+------------+----------------+----------------------------------------------------------------------+---------+
| type       | name           | description                                                          | default |
+============+================+======================================================================+=========+
| gf<imfreq> | gf             | Green's function to be fit (bosonic/fermionic)                       | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| tail_view  | known_moments  | known part of the tail                                               | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| int        | n_moments      | number of moments in the final tail (including known ones)           | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| size_t     | n_min          | Matsubara index on mesh to start the fit                             | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| size_t     | n_max          | final Matsubara index for fit (included)                             | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| bool       | replace_by_fit | if true, replace the gf data in the fitting range by the tail values | true    |
+------------+----------------+----------------------------------------------------------------------+---------+


Example
~~~~~~~~

.. compileblock:: 

  #include <triqs/gfs.hpp>
  #include <triqs/gfs/local/fit_tail.hpp>
  using namespace triqs::gfs; 
  int main(){
   triqs::clef::placeholder<0> iom_;
   double beta =10;
   int N=100;

   auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
   gw(iom_) << 1/(iom_-1);

   size_t n_min=50, n_max=90;
   int n_moments=4;  
   int  size=1; //means that we know one moment
   int order_min=1; //means that the first moment in the final tail will be the first moment
   auto known_moments = local::tail(make_shape(1,1), size, order_min); //length is 0, first moment to fit is order_min
   known_moments(1)=1.;//set the first moment

   set_tail_from_fit(gw, known_moments, n_moments, n_min, n_max, true);
   
   std::cout << gw.singularity() << std::endl;
  }



Implementation
~~~~~~~~~~~~~~~

The fitting problem is written as a linear system of equations, which is solved by SVD.

