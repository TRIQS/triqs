.. highlight:: c

Create a Matsubara frequency Green function
-------------------------------------------

The particularity here is that the statistics influences the position of the Matsubara frequencies. 

.. compileblock:: 

    #include <triqs/gfs/imfreq.hpp>
    #include <triqs/arrays.hpp>
    
    using triqs::gfs::make_gf;
    using triqs::gfs::imfreq;

    int main() {
      double beta=1; //inverse temperature
      triqs::gfs::statistic_enum stat=triqs::gfs::Fermion;
      //we will have 5 points including iw=0 and iw=beta
      size_t n_freq=5;
      auto shape = triqs::arrays::make_shape(1,1);
      auto GF=make_gf<imfreq>(beta, stat, shape, n_freq);  
    };


An alternative declaration with an explicit construction of the underlying mesh:

.. compileblock:: 

 
    #include <triqs/gfs/imfreq.hpp>
    using namespace triqs::gfs;
    int main(){
     double beta=10;
     int Nfreq =100;
     auto fermionic_imfreq_mesh = make_gf_mesh<imfreq>(beta,triqs::gfs::Fermion,Nfreq); 
     auto GF = make_gf<imfreq>(fermionic_imfreq_mesh, triqs::arrays::make_shape(1,1), local::tail(1,1));
    }


Learn more in the full reference, see :ref:`greenfunctions`

Create a Matsubara time Green function
--------------------------------------
.. compileblock:: 

    #include <triqs/gfs/imtime.hpp>
    #include <triqs/arrays.hpp>
    using triqs::gfs::make_gf;
    using triqs::gfs::imtime;

    int main() {
      double beta=1; //inverse temperature
      triqs::gfs::statistic_enum stat=triqs::gfs::Fermion;
      size_t n_times=5;
      auto shape = triqs::arrays::make_shape(1,1);
      auto GF=make_gf<imtime>(beta, stat, shape, n_times);  
    };




