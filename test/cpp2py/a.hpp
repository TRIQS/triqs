#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <ostream>
#include <triqs/utility/variant.hpp>
#include <triqs/arrays.hpp>
#include <triqs/utility/signal_handler.hpp>

namespace triqs {
  namespace py_tools {
    class reductor;
    class reconstructor;
  } // namespace py_tools
} // namespace triqs

struct A {
  private:
  double pp = 90;

  public:
  /// Doc of i member
  int i = 1;
  /// Doc of x,y
  double x = 3, y = 89;

  A()          = default;
  A(A &&)      = default;
  A(A const &) = delete; // to prevent using copy
  A &operator=(A &&) = default;
  A &operator=(A const &) = default;

  std::vector<int> data = {1, 2, 3, 4, 5, 6};

  using const_iterator = std::vector<int>::const_iterator;
  const_iterator begin() const { return data.begin(); }
  const_iterator end() const { return data.end(); }

  /// The nice doc of m1 ....
  double m1(int u, double y) const {
    std::cout << " calling m1 " << u << " " << y << std::endl;
    return x + 2 * u + 3 * y;
  }

  static int sm(int i) { return i * 2; }

  int count = 0;

  void long_fnt() {

    for (int u = 0; u < 101; u += 10) {
      sleep(1);
      if (triqs::signal_handler::received(true)) TRIQS_KEYBOARD_INTERRUPT;
      count = u;
      std::cout << " inner count " << count << std::endl;
    }
    std::cout << " completed" << std::endl;
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

  ///
  static std::string hdf5_scheme() { return "Ac"; }

  /// Write into HDF5
  friend void h5_write(triqs::h5::group fg, std::string subgroup_name, A const &a) {
    auto gr = fg.create_group(subgroup_name);
    gr.write_hdf5_scheme(a);
    h5_write(gr, "x", a.x);
  }

  /// Read from HDF5
  friend void h5_read(triqs::h5::group fg, std::string subgroup_name, A &a) {
    auto gr = fg.open_group(subgroup_name);
    //TRIQS_RUNTIME_ERROR << "A nasty error in h5read ....";
    h5_read(gr, "x", a.x);
  }

  friend std::ostream &operator<<(std::ostream &out, A const &a) { return out << " I am an A with x= " << a.x; }

  friend class triqs::py_tools::reductor;
  friend class triqs::py_tools::reconstructor;
  /// boost serialisation
  template <typename Archive> void serialize(Archive &ar, const unsigned int version) { ar &i &x; }
};

/// some function using A
inline void print_a(A const &a) { std::cout << "my a is " << a.x << std::endl; }

/// function with an error
inline void print_err(A const &a) { TRIQS_RUNTIME_ERROR << "hum does not look good" << a; }

/// DOC make_vector...
inline std::vector<int> make_vector(int size) {
  std::vector<int> r(size, 0);
  for (int i = 0; i < size; ++i) r[i] = i;
  return r;
}

inline std::vector<std::vector<int>> make_vector2(int size) { return {make_vector(size), make_vector(size + 1)}; }

inline std::vector<int> vector_x2(std::vector<int> const &v) {
  auto r = v;
  for (auto &x : r) x *= 2;
  return r;
}

using triqs::arrays::array;
using triqs::arrays::matrix;
using triqs::arrays::range;
using namespace triqs::arrays;

inline void iter_on_range(range r) {
  foreach (r, [](int i) { std::cout << i << std::endl; })
    ;
}

inline matrix<double> make_matrix(int size) { return make_unit_matrix<double>(size); }

inline void print_matrix(matrix<double> const &M) {
  // std::cout  << M <<std::endl;
}

inline std::map<std::string, std::vector<int>> map_to_mapvec(std::map<std::string, int> const &m) {
  std::map<std::string, std::vector<int>> mm;
  for (auto const &x : m) mm.emplace(x.first, std::vector<int>{x.second, 3, 5});
  return mm;
}

inline std::set<int> set_to_set(std::set<std::string> const &s) {
  std::set<int> ss;
  for (auto const &x : s) ss.insert(x.size());
  return ss;
}

inline std::tuple<> tuple_to_tuple_0(std::tuple<> const &t) { return std::make_tuple(); }

inline std::tuple<int> tuple_to_tuple_1(std::tuple<int> const &t) { return std::make_tuple(2 * std::get<0>(t)); }

inline std::tuple<int, double> tuple_to_tuple_2(std::tuple<int, double> const &t) { return std::make_tuple(2 * std::get<0>(t), 1 / std::get<1>(t)); }

inline std::tuple<int, double, std::string> tuple_to_tuple_3(std::tuple<int, double, std::string> const &t) {
  return std::make_tuple(2 * std::get<0>(t), 1 / std::get<1>(t), "*" + std::get<2>(t) + "*");
}

struct test_variant_visitor {
  int operator()(int i) { return 3 * i; }
  int operator()(std::string const &s) { return s.size(); }
  int operator()(std::pair<std::string, double> const &p) { return int(p.second); }
};

inline std::variant<int, std::string, std::pair<std::string, double>>
variant_to_variant(std::variant<int, std::string, std::pair<std::string, double>> const &v) {
  std::variant<int, std::string, std::pair<std::string, double>> res(visit(test_variant_visitor(), v));
  return res;
}

inline std::function<int(int, int)> make_fnt_ii() {
  return [](int i, int j) { return i + 2 * j; };
}

inline std::function<int(int, int, double)> make_fnt_iid() {
  return [](int i, int j, double a) { return a + i + 2 * j; };
}

inline void use_fnt_ii(std::function<int(int, int)> f) { std::cout << "use_fnt ii \n" << f(1, 2) << std::endl; }

inline void use_fnt_iid(std::function<int(int, int, double)> f) { std::cout << "use_fnt iid \n" << f(1, 2, 4.5) << std::endl; }

inline std::function<void(int, int)> make_fnt_void() {
  return [](int i, int j) { std::cout << " I am a C++ lambda : " << i << " " << j << std::endl; };
}
