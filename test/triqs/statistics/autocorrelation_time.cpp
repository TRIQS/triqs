#include <triqs/arrays.hpp>
#include <triqs/statistics.hpp>
#include "./correlated_gaussian.hpp"
#include <iostream>
#include <ctime>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
using namespace triqs::statistics;

void test_0(){
 //very simple test to test that everything works

 observable<double> X;
 X<<1.0;
 X<<-1.0;
 X<<.5;
 X<<.0;
 std::cout << average_and_error(X) << std::endl;
 std::cout << average_and_error(X*X) << std::endl;

}

void test_1(int argc, char ** argv){
 int N=100000, L=100;
 if(argc==3){
  N = atoi(argv[1]); //size
  L = atoi(argv[2]); //autocorrelation time
 }
 std::cout << "N = " << N << std::endl;
 std::cout << "L = " << L << std::endl;
 int seed= 1567;
 std::vector<double> A(N);
 correlated_gaussian_vector(A, seed, L);
 double intrinsic_variance = 1;

 //auto t1 = clock();
 TEST( autocorrelation_time(A));
 //std::cout << "time = " << double( clock()-t1)/CLOCKS_PER_SEC << std::endl;
 //t1 = clock();
 TEST( autocorrelation_time_from_binning(A,intrinsic_variance));
 TEST( autocorrelation_time_from_binning(A));
 //std::cout << "time = " << double( clock()-t1)/CLOCKS_PER_SEC << std::endl;
 //t1 = clock();
 //TEST( autocorrelation_time_from_binning2(A));
 //std::cout << "time = " << double( clock()-t1)/CLOCKS_PER_SEC << std::endl;
}

void test_2(int argc, char ** argv){
 int N=100000, L=40;
 if(argc==3){
  N = atoi(argv[1]); //size
  L = atoi(argv[2]); //autocorrelation time
 }
 int seed= 1567;
 double avg = 2;
 std::vector<double> A(N);
 correlated_gaussian_vector(A, seed, L, avg);

 observable<double> V;
 for (auto & x:A) V << x;
 TEST(autocorrelation_time(V));
 TEST(autocorrelation_time_from_binning(V));
 //TEST(autocorrelation_time_from_binning2(V));
 TEST(autocorrelation_time(V*V)); 
}


int main(int argc, char ** argv){
 try{  test_0(); } catch(std::exception const & e){ std::cerr << e.what() << std::endl;}
 try{ test_1(argc,argv); } catch(std::exception const & e){ std::cerr << e.what() << std::endl;}
 try{  test_2(argc,argv); } catch(std::exception const & e){ std::cerr << e.what() << std::endl;}
}

