#include <triqs/clef/clef.hpp>
#include <triqs/clef/io.hpp>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl;

struct F1{ 
 double v; 
 F1(double v_): v(v_){} 
 F1(F1 const &) = delete; // non copyable
 double operator() (double x) const { return 10*x;}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<const F1>,Args...>::type
  operator()( Args&&... args ) const { return make_expr_call (std::ref(*this),args...);}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<F1>,Args...>::type
  operator()( Args&&... args ) { return  make_expr_call (std::ref(*this),args...);}

  template<typename Fnt> friend void triqs_clef_auto_assign (F1 & x, Fnt f) { x.v++; std::cerr<< " called triqs_clef_auto_assign "<< f(100)<<std::endl;}
 friend std::ostream & operator<<(std::ostream & out, F1 const & x) { return out<<"F1";}
};

struct F1b {
 double v; std::shared_ptr<double> v_ptr;

 F1b(double v_): v_ptr (new double(v_)) {v=v_;} // on purpose, I avoid adding a default constructor !  
 F1b (F1b const & x)  { v_ptr  = x.v_ptr; v = x.v; std::cerr<< " F1 COPY "<<std::endl;}
 //F1b (F1b && x)  { v_ptr  = x.v_ptr; v = x.v; std::cerr<< " F1 COPY "<<std::endl;}

 double operator() (double x) const { return 10*x;}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<F1b,Args...>::type
  operator()( Args&&... args ) { return  triqs::clef::make_expr_call (*this,args...);}

  template<typename Fnt> friend void triqs_clef_auto_assign (F1b & x, Fnt f) { (*(x.v_ptr))++; std::cerr<< " called triqs_clef_auto_assign "<< f(100)<<std::endl;}
 friend std::ostream & operator<<(std::ostream & out, F1b const & x) { return out<<"F1b";}

};

struct F2 {

 double v; 
 F2() {v=0;}

 double operator()(double x, double y) const { return 10*x + y;}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<const F2>,Args...>::type
  operator()( Args&&... args ) const { return  make_expr_call (std::ref(*this),args...);}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<F2>,Args...>::type
  operator()( Args&&... args ) { return  make_expr_call (std::ref(*this),args...);}

 template<typename Fnt> friend void triqs_clef_auto_assign (F2 const & x, Fnt f) { std::cerr<< " called F2 triqs_clef_auto_assign "<< f(10,20)<<std::endl;}
 friend std::ostream & operator<<(std::ostream & out, F2 const & x) { return out<<"F2";}
};


using namespace triqs::clef;

triqs::clef::placeholder <1> x_; 
triqs::clef::placeholder <2> y_; 
triqs::clef::placeholder <3> z_; 
namespace tql= triqs::clef;

