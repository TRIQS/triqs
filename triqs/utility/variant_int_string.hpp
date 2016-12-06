/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <triqs/utility/exceptions.hpp>
#include <string>
#include <boost/serialization/utility.hpp>

namespace triqs {
namespace utility {

 /**
  * A simple int-string variant
  */
 class variant_int_string {
  enum type_t {
   Int,
   String
  };
  type_t type;

  union {
   int as_int;
   std::string as_string;
  };

  void _clean() {
   using std::string;
   if (type == String) as_string.~string();
  }

  void _copy_from(variant_int_string const &x) {
   type = x.type;
   if (type == String)
    new (&as_string) std::string(x.as_string);
   else
    new (&as_int) int(x.as_int);
  }

  public:
  /// The variant will be an int
  variant_int_string(int i = 0) : type(Int) { new (&as_int) int(i); }

  /// The variant will be a string
  variant_int_string(std::string const &s) : type(String) { new (&as_string) std::string(s); }
  variant_int_string(const char * c) : variant_int_string(std::string(c)) {}

  ~variant_int_string() { _clean(); }

  variant_int_string(variant_int_string const &x) { _copy_from(x); }

  variant_int_string &operator=(variant_int_string const &x) {
   if (type == x.type) {
    if (type == String)
     as_string = x.as_string;
    else
     as_int = x.as_int;
   } else {
    _clean();
    _copy_from(x);
   }
   return *this;
  }

  // serialization
  BOOST_SERIALIZATION_SPLIT_MEMBER();

  template <typename Archive> void save(Archive &ar, const unsigned int version) const {
   if (type == Int)
    ar << type << as_int;
   else
    ar << type << as_string;
  }

  template <typename Archive> void load(Archive &ar, const unsigned int version) {
   type_t ty;
   ar >> ty;
   int i;
   std::string s;
   switch (ty) {
    case(Int) :
     ar >> i;
     *this = variant_int_string(i);
     break;
    case(String) :
     ar >> s;
     *this = variant_int_string(s);
     break;
    default:
     TRIQS_RUNTIME_ERROR << "Deserialize error of variant_int_string";
   }
  }

  /** Apply the callable object f on v.
   * f must be callable with an int or a string (generic or overload).
   */
  template <typename F> friend auto apply_visitor(F &&f, variant_int_string const &v) -> decltype(f(0)) {
   if (v.type == String)
    return f(v.as_string);
   else
    return f(v.as_int);
  }

  ///
  friend bool operator<(variant_int_string const &x, variant_int_string const &y) {
   if (x.type != y.type) TRIQS_RUNTIME_ERROR << "Comparing two variant of different types";
   if (x.type == Int) return (x.as_int < y.as_int);
   return (x.as_string < y.as_string);
  }

  ///
  friend bool operator==(variant_int_string const &x, variant_int_string const &y) {
   if (x.type != y.type) return false;
   if (x.type == Int) return (x.as_int == y.as_int);
   return (x.as_string == y.as_string);
  }

  friend std::ostream & operator << (std::ostream & out, variant_int_string const &x) {
   apply_visitor([&out](auto const &u) mutable { out << u;}, x);
   return out;
  }
 };

 inline bool operator!=(variant_int_string const &x, variant_int_string const &y) { return !(operator==(x, y)); }
 inline bool operator>(variant_int_string const &x, variant_int_string const &y) { return !(operator<(x, y)); }
 inline std::string to_string(variant_int_string const &x) {
  std::stringstream fs;
  fs << x;
  return fs.str();
 }
}
}
