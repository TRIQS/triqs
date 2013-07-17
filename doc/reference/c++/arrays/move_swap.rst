
.. _Move:

Move and swap
==================================

Move 
------

Every classes have proper move constructor and move assignment operators.

Swap 
--------

* swap and std::swap (equivalent) can be used to swap to array, array_view and so on.

* swap just exchange the (shared) pointer to the data in memory, 
  it does not affect the data them self.
  This distinction of importance for views in particular.

* **Example** :

.. compileblock::

    #include <triqs/arrays.hpp>
    #include <iostream>
    using triqs::arrays::vector; using triqs::arrays::range; 
    int main () { 
        triqs::arrays::vector<double> V(3), W(4);
        V() = 3; W()=4; // initialize 
        auto VV = V(range (0,2));
        auto VW = W(range (0,2));
      
        std::cout << "V = "<< V << " W = " << W<< " V view "<< VV<< " W view "<< VW<< std::endl;
        swap(V,W);
        std::cout << "V = "<< V << " W = " << W<< " V view "<< VV<< " W view "<< VW<< std::endl;
        swap(VV,VW);
        std::cout << "V = "<< V << " W = " << W<< " V view "<< VV<< " W view "<< VW<< std::endl;
    }

deep_swap
--------------

* deep_swap swaps the data in memory.

* **Example** (compare with swap) :
      
.. compileblock::

    #include <triqs/arrays.hpp>
    #include <iostream>
    using triqs::arrays::vector; using triqs::arrays::range;  
    int main () { 
        triqs::arrays::vector<double> V(3), W(3);
        V() = 3; W()=5; // initialize 
        auto VV = V(range (0,2));
        auto VW = W(range (0,2));
      
        std::cout << "V = "<< V << " W = " << W<< " V view "<< VV<< " W view "<< VW<< std::endl;
        deep_swap(V,W);
        std::cout << "V = "<< V << " W = " << W<< " V view "<< VV<< " W view "<< VW<< std::endl;
        deep_swap(VV,VW);
        std::cout << "V = "<< V << " W = " << W<< " V view "<< VV<< " W view "<< VW<< std::endl;
    }



