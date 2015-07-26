/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by I. Krivenko
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
#include "test_tools.hpp"

#include <string>
#include <queue>
#include <array>
#include <ostream>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <triqs/utility/variant.hpp>
#include <triqs/utility/variant_serialize.hpp>

struct my_struct {
  std::string name;
  int value;
  int id;

  my_struct(std::string const& name = "", double value = 0) :
    name(name), value(value), id(next_id) {
    messages.push("my_struct[" + std::to_string(id) + "]: constructor");
    next_id++;
  }
  my_struct(my_struct const& other) :
    name(other.name), value(other.value), id(next_id) {
    messages.push("my_struct[" + std::to_string(id) + "]: copy-constructor");
    next_id++;
  }
  ~my_struct() {
    messages.push("my_struct[" + std::to_string(id) + "]: destructor");
  }

  my_struct & operator=(my_struct const& other) {
    messages.push("my_struct[" + std::to_string(id) + "]: operator=()");
    name = other.name;
    value = other.value;
    return *this;
  }

  bool operator==(my_struct const& other) const {
    return name == other.name && value == other.value;
  }

  bool operator<(my_struct const& other) const {
    return value < other.value;
  }

  friend std::ostream & operator<<(std::ostream & os, my_struct const& ms) {
    return os << "{" << ms.name << " => " << ms.value << "}";
  }

  // Boost.Serialization
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & name & value;
  }

  static int next_id;
  static std::queue<std::string> messages;
};
int my_struct::next_id = 0;
std::queue<std::string> my_struct::messages;

using my_variant = triqs::utility::variant<int,std::string,my_struct>;

#define EXPECT_MY_STRUCT_MESSAGE(MSG) \
ASSERT_TRUE(my_struct::messages.size()>0); \
EXPECT_EQ(my_struct::messages.front(),MSG); \
my_struct::messages.pop();

struct test_visitor {
  std::ostream & os;
  test_visitor(std::ostream & os) : os(os) {}
  void operator()(int i) { os << "int " << i << ":"; }
  void operator()(std::string s) { os << "string '" << s << "':"; }
  void operator()(my_struct const& ms) { os << "my_struct " << ms << ":"; }
};

TEST(Variant,Basic) {
  {
  my_variant v_int(2);
  my_variant v_string("text");
  my_variant v_my_struct1(my_struct{"g",9});
  EXPECT_MY_STRUCT_MESSAGE("my_struct[0]: constructor");
  EXPECT_MY_STRUCT_MESSAGE("my_struct[1]: copy-constructor");
  EXPECT_MY_STRUCT_MESSAGE("my_struct[0]: destructor");

  my_variant v_my_struct2(v_my_struct1);
  EXPECT_MY_STRUCT_MESSAGE("my_struct[2]: copy-constructor");

  my_variant v_my_struct3(my_struct{"x",2});
  EXPECT_MY_STRUCT_MESSAGE("my_struct[3]: constructor");
  EXPECT_MY_STRUCT_MESSAGE("my_struct[4]: copy-constructor");
  EXPECT_MY_STRUCT_MESSAGE("my_struct[3]: destructor");

  v_my_struct1 = v_my_struct3;
  EXPECT_MY_STRUCT_MESSAGE("my_struct[1]: operator=()");

  v_my_struct1 = v_string;
  EXPECT_MY_STRUCT_MESSAGE("my_struct[1]: destructor");

  EXPECT_TRUE(v_my_struct1 == v_string);
  EXPECT_FALSE(v_int < v_int);

  std::stringstream ss;
  ss << v_int << ":";
  ss << v_string << ":";
  ss << v_my_struct1 << ":";
  ss << v_my_struct2 << ":";
  ss << v_my_struct3 << ":";
  EXPECT_EQ(ss.str(),"2:text:text:{g => 9}:{x => 2}:");

  std::stringstream ss2;
  apply_visitor(test_visitor(ss2), v_int);
  apply_visitor(test_visitor(ss2), v_string);
  apply_visitor(test_visitor(ss2), v_my_struct1);
  apply_visitor(test_visitor(ss2), v_my_struct2);
  apply_visitor(test_visitor(ss2), v_my_struct3);
  EXPECT_EQ(ss2.str(),"int 2:string 'text':string 'text':my_struct {g => 9}:my_struct {x => 2}:");
  }
  EXPECT_MY_STRUCT_MESSAGE("my_struct[4]: destructor");
  EXPECT_MY_STRUCT_MESSAGE("my_struct[2]: destructor");
}

TEST(Variant,Serialization) {
  std::array<my_variant,3> in {my_variant(7), my_variant("text_in"), my_struct("G",101)};
  std::array<my_variant,3> out {my_variant(8), my_variant("text_out"), my_struct("g",202)};

  std::array<std::array<int,3>,6> permutations {{{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}}};

  for(auto p : permutations){
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << in[0] << in[1] << in[2];

    boost::archive::text_iarchive ia(ss);
    ia >> out[p[0]] >> out[p[1]] >> out[p[2]];

    EXPECT_EQ(out[p[0]],7);
    EXPECT_EQ(out[p[1]],"text_in");
    EXPECT_EQ(out[p[2]],my_struct("G",101));
  }

}

TEST(Variant,Casts) {
  my_variant v_int(2);
  my_variant v_string("text");
  my_variant v_my_struct(my_struct{"g",9});

  int i;
  std::string s;
  my_struct ms;
  EXPECT_NO_THROW(i = v_int);
  EXPECT_NO_THROW(s = v_string);
  EXPECT_NO_THROW(ms = v_my_struct);
  EXPECT_EQ(i,2);
  EXPECT_EQ(s,"text");
  EXPECT_EQ(ms,(my_struct{"g",9}));
  EXPECT_THROW(i = v_string,triqs::exception);
  EXPECT_THROW(s = v_my_struct,triqs::exception);
  EXPECT_THROW(ms = v_int,triqs::exception);
}

MAKE_MAIN;
