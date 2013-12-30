.. highlight:: c

.. _gf_clef:

Interaction with CLEF expressions 
============================================

The gf containers and their view classes can be used with the :doc:`../clef/contents` library : 

* They can be called with CLEF expressions.
* :doc:`Automatic assignment<../clef/assign>` has been set up.

Using the CLEF library offers a quick and efficient way to fill an array with multiple advantages : 

* It is simpler and more readeable than a series of for loops.
* It is more optimal since the loops are automatically written in the best order for memory traversal.

**Example** : 

.. compileblock::

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; using triqs::clef::placeholder;
    int main(){

     // Cf gf<imfreq> specialisation page for the constructor
     double beta=10; int Nfreq =100;
     auto g  = gf<imfreq> { {beta,Fermion,Nfreq}, {1,1} };

     // Filling the gf with something...
     placeholder<0> wn_;
     g(wn_) << 1/ (wn_ + 2);
     g(wn_) << 1/ (wn_ + 2 + g(wn_) );
    }

.. note:: 

   The LHS uses () and not brackets, even though it is on the mesh, because of the strange C++ limitation 
   that [] can not be overloaded for several variables...



  

