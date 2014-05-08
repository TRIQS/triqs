#include <iostream>
#include <vector>
#include <ostream>
#include <triqs/arrays.hpp>
//#include <triqs/h5.hpp>

namespace triqs { namespace py_tools { 
 class reductor;
 class reconstructor;
}}

struct A {
  private : 
   double pp =90;
  public:

  /// Doc of i member 
  int i = 1;
  /// Doc of x,y
  double x = 3, y = 89;

  A() = default;
  A(A&&) = default;
  A(A const &) = delete; // to prevent using copy
  A &operator =(A&&) = default;
  A &operator =(A const &) = default;

  std::vector<int> data = { 1, 2, 3, 4, 5, 6 };

  using const_iterator = std::vector<int>::const_iterator;
  const_iterator begin() const { return data.begin(); }
  const_iterator end() const { return data.end(); }

  /// The nice doc of m1 ....
  double m1(int u, double y) const {
    std::cout << " calling m1 " << u << " " << y << std::endl;
    return x + 2 * u + 3 * y;
  }


  double m1(int u) const {
    std::cout << " calling m1 one arg " << u << std::endl;
    return x + 2 * u;
  }

  /// m2 has also some doc !
   double m2(double u) const {
    std::cout << " calling m2 one arg " << u << std::endl;
    return x + 20 * u;
  }

  double &operator[](int i) { return y; }

  A operator+(A const &y) {
    A a;
    a.x = x + y.x;
    return a;
  }
  A operator-(A const &y) {
    A a;
    a.x = x - y.x;
    return a;
  }
  A operator*(A const &y) {
    A a;
    a.x = x * y.x;
    return a;
  }
  friend A operator*(A const &x, double s) {
    A a;
    a.x = x.x * s;
    return a;
  }
  friend A operator*(double s, A const &x) {
    A a;
    a.x = x.x * s;
    return a;
  }
  A operator/(A const &y) {
    A a;
    a.x = x / y.x;
    return a;
  }
  A operator/(double y) {
    A a;
    a.x = x / y;
    return a;
  }

  int _get_i() const { return i; }
  void _set_i(int j) { i = j; }

  int operator()(int u) {
    std::cout << " Calling with u = " << u << std::endl;
    return u + i;
  }

  friend void h5_write(int gr, A const &a) {
    std::cout << " mimic h5 writing" << std::endl;
  }

  /// Write into HDF5
  friend void h5_write(triqs::h5::group fg, std::string subgroup_name, A const &a) {
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr,"x",a.x);
  }

  /// Read from HDF5
  friend void h5_read(triqs::h5::group fg, std::string subgroup_name, A & a) { 
   auto gr = fg.open_group(subgroup_name);
   //TRIQS_RUNTIME_ERROR << "A nasty error in h5read ....";
   h5_read(gr,"x",a.x);
  }
  
  friend std::ostream &operator<<(std::ostream &out, A const &a) {
    return out << " I am an A with x= " << a.x;
  }

  friend class triqs::py_tools::reductor;
  friend class triqs::py_tools::reconstructor;
  /// boost serialisation
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & i & x;
  }
};

/// some function using A
void print_a(A const & a) { 
    std::cout << "my a is " << a.x << std::endl;
}

/// function with an error
void print_err(A const &a) { 
    TRIQS_RUNTIME_ERROR << "hum does not look good" << a;
}

/// DOC make_vector...
std::vector<int> make_vector(int size) {
  std::vector<int> r(size, 0);
  for (int i = 0; i < size; ++i)
    r[i] = i;
  return r;
}

std::vector<std::vector<int> > make_vector2(int size) {
  return { make_vector(size), make_vector(size + 1) };
}

std::vector<int> vector_x2(std::vector<int> const &v) {
  auto r = v;
  for (auto &x : r)
    x *= 2;
  return r;
}

using triqs::arrays::matrix;
using triqs::arrays::array;
using triqs::arrays::range;
using namespace triqs::arrays;

void iter_on_range(range r){ 
 foreach(r, [](int i) { std::cout  << i << std::endl;});
}

matrix<double> make_matrix(int size) { return make_unit_matrix<double>(size); }

void print_matrix(matrix<double> const &M) {
  // std::cout  << M <<std::endl;
}


std::function<int(int,int)>  make_fnt_ii() {
 return [](int i, int j) { return i + 2*j;}; 
}

std::function<int(int,int,double)>  make_fnt_iid() {
 return [](int i, int j, double a) { return a+ i + 2*j;}; 
}


void use_fnt_ii(std::function<int(int,int)> f) { 
 std::cout  << "use_fnt ii \n"<< f(1,2) << std::endl;
}

void use_fnt_iid(std::function<int(int,int,double)> f) { 
 std::cout  << "use_fnt iid \n"<< f(1,2,4.5) << std::endl;
}


std::function<void(int,int)>  make_fnt_void() {
 return [](int i, int j) { std::cout  << " I am a C++ lambda : "<<i <<" "<< j << std::endl;}; 
}
