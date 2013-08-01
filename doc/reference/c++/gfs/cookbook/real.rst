.. highlight:: c

Create a real time Green function
---------------------------------

.. compileblock:: 

    #include <triqs/gfs/retime.hpp>
    #include <triqs/arrays.hpp>
    
    using triqs::gfs::make_gf;
    using triqs::gfs::retime;
    
    int main() {
      double tmin=0;
      double tmax=10;
      //we will have 5 points
      size_t n_times=5;
      //we want a Green function whose values are complex numbers
      auto shape = triqs::arrays::make_shape(1,1);
      // the type of GF is triqs::gfs::gf<triqs::gfs::retime>
      auto GF=make_gf<retime>(tmin, tmax, n_times, shape);  
    };

Create a real frequency Green function 
--------------------------------------
.. compileblock:: 

    #include <triqs/arrays.hpp>
    #include <triqs/gfs/refreq.hpp>
    
    using triqs::gfs::make_gf;
    using triqs::gfs::refreq;

    int main() {
      double wmin=0;
      double wmax=10;
      size_t n_freq=5;
      //we want a Green function whose values are 2*2 matrices of complex numbers
      auto shape = triqs::arrays::make_shape(2,2);
      auto GF=make_gf<refreq>(wmin, wmax, n_freq, shape);
    };

