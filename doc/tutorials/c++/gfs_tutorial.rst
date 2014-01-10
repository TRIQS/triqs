Green function cookbook
=======================

.. highlight:: c

.. toctree::
   :maxdepth: 1

The gfs class of TRIQS contains objects representing Green functions over real or imaginary times, rela or imaginary frequencies...  that can be easily manipulated. 
(see  :doc:`here  <../../reference/c++/gf/contents>`). 
Here are a couple of simple examples showing the basic use of this class.

Creation of a real time GF
-----------------------------

Here we create a GF define on the time interval from tmin to tmax. 
If we want the value of the GF at any time to be a scalar, we use:

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;

    int main() {
      double tmin=0, tmax=10; // the time interval
      int n_times=100;        // we will have 100 points
      auto g = gf<retime, scalar_valued>{ {tmin, tmax, n_times}};
    }

If we need a matrix of size n by m, we use:

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;

    int main() {
      double tmin = 0, tmax = 10;    // the time interval
      const int n=2, m=2, n_times = 100;   // we will have 100 points
      auto g = gf<retime, matrix_valued>{ {tmin, tmax, n_times} , {n, m} };
    }
    
Or a tensor !

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;

    int main() {
      double tmin = 0, tmax = 10;    // the time interval
      double beta=1;
      int n_times = 100;   // we will have 100 pointspoints
      auto g =gf<cartesian_product<retime,imtime>, tensor_valued<3>>{ 
        { {tmin, tmax, n_times}, {beta, Fermion, n_times} }, {2,2,2}};
    }

    
Creation of other simple GF's
-------------------------------

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;

    int main() {
      double beta=10; // the time interval
      int n_times=100;        // we will have 100 
      auto g = gf<imtime, scalar_valued>{ {beta, Fermion, n_times}};
    }

Creation of a two real time GF
--------------------------------

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    int main(){
     double tmin = 0, tmax = 1.0;
     int nt = 100;
     auto g = gf< cartesian_product<retime,retime>,scalar_valued>{{{tmin,tmax,nt},{tmin,tmax,nt}}};
    }


How to fill a GF with placeholders
-----------------------------------

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    int main(){
     double tmin = 0, tmax = 1.0;
     int nt = 100;
     auto g = gf< cartesian_product<retime,retime>,scalar_valued>{{{tmin,tmax,nt},{tmin,tmax,nt}}};
     triqs::clef::placeholder<0> t1_;
     triqs::clef::placeholder<1> t2_;
     g( t1_, t2_) << 2*t1_;
    }


How to interpolate the GF value at a point of the domain
---------------------------------------------------------

You simply have to call the GF with the coordinates of the point: 

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    int main(){
     double tmin = 0, tmax = 1.0;
     int nt = 100;
     auto g = gf< cartesian_product<retime, retime>, scalar_valued >{ 
     { {tmin, tmax, nt}, {tmin, tmax, nt} } };
     triqs::clef::placeholder<0> t1_;
     triqs::clef::placeholder<1> t2_;
     g( t1_, t2_) << 2.*t1_;
     std::cout << g(0.24, 0.36) << std::endl;
    }
    
    


Learn more in the full reference, see :ref:`gf`


