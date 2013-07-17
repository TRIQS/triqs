
Create a Matsubara frequency Green function
-------------------------------------------

The particularity here is that the statistics influences the position of the Matsubara frequencies. 

.. compileblock:: 

    #include <triqs/gf/imfreq.hpp>
    #include <triqs/arrays.hpp>
    
    using triqs::gf::make_gf;
    using triqs::gf::imfreq;

    int main() {
      double beta=1; //inverse temperature
      triqs::gf::statistic_enum stat=triqs::gf::Fermion;
      //we will have 5 points including iw=0 and iw=beta
      size_t n_freq=5;
      auto shape = triqs::arrays::make_shape(1,1);
      auto GF=make_gf<imfreq>(beta, stat, shape, n_freq);  
    };


An alternative declaration with an explicit construction of the underlying mesh:

.. compileblock:: 

 
    #include <triqs/gf/imfreq.hpp>
    using namespace triqs::gf;
    int main(){
     double beta=10;
     int Nfreq =100;
     auto fermionic_imfreq_mesh = make_gf_mesh<imfreq>(beta,triqs::gf::Fermion,Nfreq); 
     auto GF = make_gf<imfreq>(fermionic_imfreq_mesh, triqs::arrays::make_shape(1,1), local::tail(1,1));
    }


Learn more in the full referencen, see :ref:`greenfunctions`

Create a Matsubara time Green function
--------------------------------------
.. compileblock:: 

    #include <triqs/gf/imtime.hpp>
    #include <triqs/arrays.hpp>
    using triqs::gf::make_gf;
    using triqs::gf::imtime;

    int main() {
      double beta=1; //inverse temperature
      triqs::gf::statistic_enum stat=triqs::gf::Fermion;
      size_t n_times=5;
      auto shape = triqs::arrays::make_shape(1,1);
      auto GF=make_gf<imtime>(beta, stat, shape, n_times);  
    };




