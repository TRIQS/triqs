#include <triqs/statistics.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

using namespace triqs::statistics; using namespace boost;
///construct correlated series a of expectation value 10 and compute average/error bar
int main(){
 int corr_length = 400;//correlation length
 std::vector<double> a(4000000);

 variate_generator<mt19937,normal_distribution<>> generator(mt19937(100405), normal_distribution<>());

 a[0] = generator();
 double f = exp(-1. / corr_length);
 for (int i = 1; i < a.size(); i++) a[i] = f * a[i-1] + sqrt(1 - f * f) * generator();

 for(auto & x : a) x+=10;

 //std::cout << "tau from autocorr. function  = " << autocorrelation_time(a) << std::endl;
 std::cout << "tau from binning  = " << autocorrelation_time_from_binning(a) << std::endl;

 //fill observable with the series 
 observable<double> A;
 for(auto const & x : a) A << x ;
  
 std::cout << "average of a = " << average_and_error(A) << std::endl;
 std::cout << "average of a*a = " << average_and_error(A*A) << std::endl;

 return 0;
}
