.. highlight:: c

Jackknife
============

Synopsis
---------


`make_jackknife(T series)` 
 - series: object with **TimeSeries** concept

 returns the jackknifed time series.





Example
---------

.. compileblock:: 

  #include <triqs/statistics.hpp>
  using namespace triqs::statistics;
  int main(){
   observable<double> A;
   A<<1.;A<<1.5;A<<.2;A<<1.1;
   auto A_j = make_jackknife(A);
   std::cout << A_j << std::endl;
   return 0;
  }

