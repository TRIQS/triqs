
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

