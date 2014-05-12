Binning
==========

Synopsis
----------
 
`make_binned_series(T series, int bin_size)` 
 - series: object with **TimeSeries** concept
 - bin_size: size of the bin

 returns the binned time series.

Example
--------

.. compileblock:: 

  #include <triqs/statistics.hpp>
  using namespace triqs::statistics;
  int main(){
   observable<double> A;
   A<<1.;A<<1.5;A<<.2;A<<1.1;
   auto A_b = make_binned_series(A,2);
   std::cout << A_b << std::endl;
   return 0;
  }
