/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by H. Hafermann, O. Parcollet
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
#include <triqs/arrays.hpp>
#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include <triqs/python_tools/wrapper_tools.hpp>
#endif
#include <string>
#include <complex>
#include <memory>
#include <map>
#include <typeindex>
#include <boost/preprocessor/seq/for_each.hpp>
// serialization will use hdf5 with a failsafe to boost for old hdf5 libs.
#include <triqs/utility/serialization.hpp>

namespace triqs {
namespace params {

 using triqs::get_triqs_hdf5_data_scheme; // bring it into name resolution

 template <typename T> std::ostream &operator<<(std::ostream &out, std::vector<T> const &v) {
  out << "[";
  int c = 0;
  for (auto const &x : v) out << (c++ ? ", " : "") << x;
  return out << "]";
 }

// All the predefined cast of _field
#define TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST                                                                                 \
 (int)(long)(long long)(unsigned int)(unsigned long)(unsigned long long)(double)(bool)(std::string)

 // the type actually stored
 template <typename T, typename Enable = void> struct storage_t_impl : std::conditional<std::is_integral<T>::value, long, T>{};
 template <typename T> using storage_t = typename storage_t_impl<std::c14::decay_t<T>>::type;

 template <typename A> struct storage_t_impl<A, std::c14::enable_if_t<arrays::is_amv_value_or_view_class<A>::value>> {
  using type = arrays::array<typename A::value_type, A::rank>;
 };
 template<> struct storage_t_impl<const char *> {using type = std::string;};
 template<> struct storage_t_impl<char *> {using type = std::string;};

 // --------------- the field object ---------------------------------------

 class _field {
  friend class parameters;

  struct _data {
   virtual const void *get() const = 0;
   virtual _data *clone() const = 0;
   virtual void h5_write_(h5::group, std::string const &) const = 0;
   virtual void h5_read_(h5::group, std::string const &) = 0;
   virtual std::string serialize() const = 0;
   virtual void deserialize(std::string const &) = 0;
   virtual std::ostream &print(std::ostream &out) const = 0;
#ifdef TRIQS_WITH_PYTHON_SUPPORT
   virtual bool from_python_convertible(PyObject *) const = 0;
   virtual void set_from_python(PyObject *) = 0;
   virtual PyObject *to_python() const = 0;
#endif
  };

  template <typename T> struct _data_impl : _data {
   T x;

   _data_impl(T obj) : x(std::move(obj)) {}
   _data_impl(_data_impl &&c) = default;
   _data_impl(_data_impl const &)  = default;

   virtual const void *get() const override { return &x; }
   virtual _data *clone() const override { return new _data_impl(*this); }
   virtual void h5_write_(h5::group f, std::string const &name) const override { h5_write(f, name, x); }
   virtual void h5_read_(h5::group f, std::string const &name) override { h5_read(f, name, x); }
   virtual std::string serialize() const override { return triqs::serialize(x); }
   virtual void deserialize(std::string const &s) override {
    x = triqs::deserialize<T>(s);
   };
   virtual std::ostream &print(std::ostream &out) const override { return out << x; }
#ifdef TRIQS_WITH_PYTHON_SUPPORT
   virtual bool from_python_convertible(PyObject *ob) const override {
    return py_tools::py_converter<T>::is_convertible(ob, true);
   }
   virtual void set_from_python(PyObject *ob) override { x = py_tools::py_converter<T>::py2c(ob); }
   virtual PyObject *to_python() const override { return py_tools::py_converter<T>::c2py(x); }
#endif
  };

  std::type_index index;
  std::unique_ptr<_data> p; //invariant : is never null
  static std::map<std::type_index, std::string> type_names;
  std::string name_; // for_error_messages
  bool modified = false;
  bool no_default_value = false;

  // only parameters will construct _field
  template <typename T>
  _field(T obj, std::string n, bool without_default_value)
     : index(typeid(storage_t<T>)), p(new _data_impl<storage_t<T>>{std::move(obj)}), name_(n), no_default_value(without_default_value) {
   type_names.insert({index, get_triqs_hdf5_data_scheme(storage_t<T>{})});
  }

  std::string type_name() const { return type_name(index); }
  static std::string type_name(std::type_index i);

  const void *get() const { return (p ? p->get() : nullptr); }
  template <typename T> friend T extract(_field const &ob);
  template <typename T> bool has_type() const { return index == typeid(T); }

  public:
#ifdef TRIQS_SERIALIZATION_WITH_HDF5_IMPOSSIBLE
  _field() : index(typeid(void)) {} // BREAKS invariant : only used for BOOST serialization failsafe mode.
#endif
  _field(_field &&c) = default;
  _field(_field const &x) : index(x.index), p(x.p ? x.p->clone() : nullptr), name_(x.name_), modified(x.modified), no_default_value(x.no_default_value) {} // regular type

  _field &operator=(_field &&c) = default;
  _field &operator=(_field const &x) { return operator=(_field(x)); }

  template <typename RHS> _field &operator=(RHS rhs) { // pass by value (sink)
   modified = true;
   using S_t = storage_t<RHS>;
   if (index != typeid(S_t)) {
    TRIQS_RUNTIME_ERROR << "Field "<< name_<<" is of type "<< type_name(index)<<". I can not put a " << type_name(typeid(S_t)) << " into it.";
   }
   p.reset(new _data_impl<S_t>{std::move(rhs)});
   return *this;
  }

  // rewrite a few cases for practical convenience ...
  _field &operator=(int rhs) { // a special case where we can correct : int -> double
   if (index == typeid(double)) return operator=(double(rhs));
   return operator=(long(rhs));// beware infinite loop! Works because int != long ... Clean this ...
  }
  _field &operator=(const char *rhs) { return operator=(std::string(rhs)); }

  // In the case where the data is itself a parameters (used to implement subgroups)
  // we suplement _field with a few parameters methods
  // Implemented after parameters (we need the type).
  // Strict check type at runtime.
  template <typename... T> _field &add_field(T &&... x);
  _field &operator[](const char * key);
  _field const &operator[](const char * key) const;
  friend bool is_parameter(_field const & f);
  _field& add_group(std::string const& key, std::string const& doc);

  /// Name of the field
  std::string const &name() const { return name_; }

  /// Has the field been modified
  bool is_modified() const { return modified; }

  /// Is a modification required for this field
  bool is_modification_required() const { return no_default_value && (!modified); }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  /// Convertions python <-> C++
  bool from_python_convertible(PyObject *ob) const { return p->from_python_convertible(ob); }
  void set_from_python(PyObject *ob) {
   p->set_from_python(ob);
   modified = true;
  }
  PyObject *to_python() const { return p->to_python(); }
#endif

// implemented later, since it needs the extract function ...
#define CAST_OPERATOR(r, data, T) operator T() const;
  BOOST_PP_SEQ_FOR_EACH(CAST_OPERATOR, nil, TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST);
#undef CAST_OPERATOR

#ifdef TRIQS_SERIALIZATION_WITH_HDF5_IMPOSSIBLE
  // ----- Boost serialisation

  template <class Archive> void save(Archive &ar, const unsigned int version) const {
   std::string s = p->serialize();
   ar << TRIQS_MAKE_NVP("seria_str", s);
  }
  template <class Archive> void load(Archive &ar, const unsigned int version) {
   std::string s;
   ar >> TRIQS_MAKE_NVP("seria_str", s);
   p->deserialize(s);
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

  friend std::ostream &operator<<(std::ostream &out, _field const &ob) { return ob.p->print(out); }

  // ---------------------  hdf5  ---------------------------------
  friend std::string get_triqs_hdf5_data_scheme(_field const &) { return ""; }
  friend void h5_write(h5::group F, std::string const &Name, _field const &);
  friend void h5_read(h5::group F, std::string const &Name, _field &);

 }; // object class

// --------------------- arithmetic operations are deleted for _field  ---------------------------------

#define DELETE_OP(op)                                                                                                            \
 template <typename LHS, typename RHS>                                                                                           \
 std::c14::enable_if_t<std::is_same<LHS, _field>::value || std::is_same<RHS, _field>::value> operator op(LHS const &,            \
                                                                                                         RHS const &) = delete;
 DELETE_OP(+);
 DELETE_OP(-);
 DELETE_OP(*);
 DELETE_OP(/ );
#undef DELETE_OP

 // --------------------- extraction with type checking for C++ ---------------------------------

 template <typename T> T extract(_field const &ob) {
  if (!ob.has_type<storage_t<T>>())
   TRIQS_RUNTIME_ERROR << "extraction of " << ob.name() << " impossible : type mismatch. I have received a " << ob.type_name()
                       << ", while I am supposed to extract a " << _field::type_name(typeid(T));
  return *static_cast<const storage_t<T> *>(ob.get());
 }

 template <> double extract(_field const &obj); // special treatment for basic types
 template <> long extract(_field const &obj);
}
}
