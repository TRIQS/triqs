/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by H. Hafermann, O. Parcollet
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
#ifndef TRIQS_UTILITY_OPAQUE_OBJECT_H5_H
#define TRIQS_UTILITY_OPAQUE_OBJECT_H5_H
#include <triqs/utility/first_include.hpp>
#include <string>
#include <complex>
#include <memory>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/serialization/map.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/serialization.hpp>
#include <triqs/arrays.hpp>
#include <triqs/python_tools/array_interface.hpp>

namespace triqs { namespace utility {

 template<typename T> 
 std::ostream & operator<<(std::ostream & out, std::vector<T> const & v) { 
  out << "["; int c = 0; for (auto const & x : v) out << ( c++==0 ? ", " :" ") << x;
  return out << "]";
 }

 // All the predefined cast of _object
#define TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST (int)(long)(long long)(unsigned int)(unsigned long)(unsigned long long)(double)(bool)(std::string)

 // a trait to compute the type actually stored in the opaque object.
 // T except for integers, which are all stored as long 
 template<typename T> struct _object_collapse_type_impl : std::conditional<std::is_integral<T>::value, long, T > {};

 // We store the array/matrix/vector as array_c (a view repackaged as a value) since : 
 // in python : it allows to extract a view
 // in C++ : it allows to extract a value (array) with a copy, or a view (array_view) without a copy
 template<class T, int R, ull_t Opt, ull_t To> struct _object_collapse_type_impl< arrays::array<T,R,Opt,To>> { typedef arrays::array_c<T,R,Opt,To> type;};
 template<class T, ull_t Opt, ull_t To>        struct _object_collapse_type_impl< arrays::matrix<T,Opt,To>>  { typedef arrays::array_c<T,2,Opt,To> type;};
 template<class T, ull_t Opt>                  struct _object_collapse_type_impl< arrays::vector<T,Opt>>     { typedef arrays::array_c<T,1,Opt> type;};

 //same thing for the view
 template<class T, int R, ull_t Opt, ull_t To> struct _object_collapse_type_impl< arrays::array_view<T,R,Opt,To>> { typedef arrays::array_c<T,R,Opt,To> type;};
 template<class T, ull_t Opt, ull_t To>        struct _object_collapse_type_impl< arrays::matrix_view<T,Opt,To>>  { typedef arrays::array_c<T,2,Opt,To> type;};
 template<class T, ull_t Opt>                  struct _object_collapse_type_impl< arrays::vector_view<T,Opt>>     { typedef arrays::array_c<T,1,Opt> type;};

 // uncomment after merge qview branch 
 //template<class T, int R, ull_t Opt, ull_t To> struct _object_collapse_type_impl< arrays::array_qview<T,R,Opt,To>> { typedef arrays::array_c<T,R,Opt,To> type;};
 //template<class T, ull_t Opt, ull_t To>        struct _object_collapse_type_impl< arrays::matrix_qview<T,Opt,To>>  { typedef arrays::array_c<T,2,Opt,To> type;};
 //template<class T, ull_t Opt>                  struct _object_collapse_type_impl< arrays::vector_qview<T,Opt>>     { typedef arrays::array_c<T,1,Opt> type;};
 
 template<typename T> struct _object_collapse_type: _object_collapse_type_impl <typename std::decay<T>::type> {};

 // Since I used array_c to store in the dict, I need to make it h5 read/writable.
 // write is ok, because it is a view. Read : we can not use the view (no resize)
 // so I first build the value (an array) and rebind the array_c to it.
 template<class T, int R, ull_t Opt, ull_t To>
 void h5_read(h5::group const &gr, std::string const &Name, arrays::array_c<T,R,Opt,To> & a) {
  arrays::array<T,R,Opt,To> v; h5_read(gr,Name,v); a=v();// rebinds !
 }

 // --------------- the opaque object ---------------------------------------
 // _object is a value : it makes deep copies in particular ...
 class _object {

  std::shared_ptr<void> p;          // the object handled by the class
  size_t type_code_;                // id of the type, implementation dependent...
  std::function<_object()> clone_;  // clones the object : will be used to make copy of parameters
  std::function<void(h5::group const &, std::string const &)> h5_w; // the function to write in h5
  std::function<std::string()> serialize_; // for boost serialization ...
  std::function<void(std::ostream&)> print_;
  std::string name_;

  public :

  template<typename T> static constexpr size_t type_code () { return typeid(T).hash_code();}
  template<typename T> static std::string make_type_name () { return get_triqs_hdf5_data_scheme(typename _object_collapse_type<T>::type());} 
  //template<typename T> static std::string make_type_name () { return demangle(typeid(T).name());}

  _object() {_init(); };

  // pass a const & or a && (in which case a move will be used, provided that T has a move constructor)
  template<typename T>
   _object ( T && obj, std::string const & name){ delegate_constructor( new typename std::remove_cv<typename std::remove_reference<T>::type>::type(std::forward<T>(obj)), name); }

  private:
  template<typename T> void delegate_constructor( T * obj, std::string const & name) {
   p = std::shared_ptr<void> (obj);
   type_code_ = type_code<T>();
   name_ = name;
   clone_ =  [obj,name]() { return _object( *obj, name);} ;
   //clone_     = [this,obj]() { return _object::factory( *obj);} ; //clone_ =  [obj]() { return _object( *obj, "");} ;
   //h5_w       = [obj](h5::group const &F, std::string const &Name)->void { h5_write(F,Name, *obj);};
   h5_w       = h5::make_h5_write(obj);// either call h5_write or synthetize one into a string using boost serialization
   //static_assert(h5::has_h5_write<T>::value, "oops");
   serialize_ = [obj](){ return triqs::serialize(*obj);};
   print_     = [obj](std::ostream & out ){out << *obj;};
   // CHECK if std::bind would lead to less code bloat ??
   _init();
  }

  public:

  _object(_object const & x ) { *this = x; _init(); }
  _object(_object && c)       { *this= std::move(c); _init();}

  friend void swap (_object & a, _object & b) {
#define SWAP(A)   swap(a.A,b.A)
   SWAP(p); std::SWAP(type_code_); SWAP(clone_); SWAP(h5_w); SWAP(serialize_); SWAP(print_); SWAP(name_);
#undef SWAP
  }

  _object & operator = (_object && c)       { swap(*this, c); return *this; }
  _object & operator = (_object const & x ) { if (x.p) *this = x.clone_(); else *this =  _object(); return *this; }
  _object & operator = (_object & x )       { if (x.p) *this = x.clone_(); else *this =  _object(); return *this; }
  // the last one is needed since otherwise the template for _object & is taken !
  // alternatively disable the template for object ...

  template <typename RHS> _object & operator=(RHS && rhs) {
   typedef typename _object_collapse_type<RHS>::type storage_t;
   *this =  _object (storage_t(std::forward<RHS>(rhs)),this->name()); 
   register_type<storage_t>::invoke();
   //register_type<typename std::remove_reference<RHS>::type>::invoke();
   return *this;
  }

  // special treatment for const char *: fall back to string
  _object &  operator=(const char * rhs) { *this = std::string(rhs); return *this;}

  bool is_empty() const { return bool(p);}

  std::string const & name() const { return name_;}
  void set_name(std::string const & n)  { name_ = n;}
  
  friend bool have_same_type( _object const & a, _object const & b) { return a.type_code_ == b.type_code_;}

  template<typename T> bool has_type() const { return type_code_ == type_code<T>();}

  const void * get_void_ptr() const { return p.get();}
  void * get_void_ptr() { return p.get();}

   // implemented later, since it needs the extract function ...
#define CAST_OPERATOR(r, data, T) operator T () const;
  BOOST_PP_SEQ_FOR_EACH(CAST_OPERATOR, nil , TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST);
#undef CAST_OPERATOR

  friend std::ostream & operator << (std::ostream & out, _object const & p) { if (p.is_empty()) p.print_(out); else out<< "empty"; return out;}
  friend void h5_write ( h5::group F, std::string const & Name, _object const & obj){ obj.h5_w(F,Name); };
  friend void h5_read ( h5::group F, std::string const & Name, _object & obj);
  friend std::string get_triqs_hdf5_data_scheme(_object const&) { return "";}

  std::string type_name() const { return type_code_to_type_name[type_code_];}

  // ----- Boost serialisation
  template<class Archive>
   void save(Archive & ar, const unsigned int version) const {
    std::string s = serialize_();
    ar << boost::serialization::make_nvp("type_name", type_code_to_type_name[type_code_]);
    ar << boost::serialization::make_nvp("seria_str", s);
   }
  template<class Archive>
   void load(Archive & ar, const unsigned int version) {
    std::string s, tnn;
    ar >> boost::serialization::make_nvp("type_name", tnn);
    ar >> boost::serialization::make_nvp("seria_str", s);
    auto it = type_name_to_type_code.find(tnn);
    if (it== type_name_to_type_code.end())
     TRIQS_RUNTIME_ERROR << " Can not deserialize the type "<< tnn<< "\n Did you forget to register it ?";
    *this = code_to_deserialization_fnts[it->second](s);
   }
  BOOST_SERIALIZATION_SPLIT_MEMBER();

  // ----- Deserialization (boost, h5) table : type_code -> deserialization function
  // init will register the most common types.
  static std::map<size_t, std::function<_object(std::string const &)>>                      code_to_deserialization_fnts;
  static std::map<size_t, std::function<_object(h5::group const &, std::string const &)>>   code_to_h5_read_fnts;
  static std::map<hid_t, size_t >                                                           h5_type_to_c_equivalence;
  static std::map<std::pair<size_t,int>, size_t >                                           code_element_rank_to_code_array;
  static std::map<std::string, size_t >                                                     h5_scheme_to_code;
  static std::map<size_t, std::string >                                                     type_code_to_type_name;
  static std::map<std::string, size_t >                                                     type_name_to_type_code;

  static bool initialized;
  static void _init();
  static bool is_initialised(size_t code) { return type_code_to_type_name.find(code) != type_code_to_type_name.end();}

  template <typename T> struct register_type;

  private: // native type
  template <typename T>
   static void register_native_type() {
    if (register_type<T>::invoke()) return;
    h5_type_to_c_equivalence[h5::h5_type_from_C(T()).getId()] = type_code<T>();
   }

 };// object class

 // --------------------- type registering  ---------------------------------

 template <typename T> struct _object::register_type<const T> : _object::register_type<T>{};

 template <typename T> struct _object::register_type {
  static bool invoke() { // returns true if already registered
   size_t code = type_code<T>();
   if (is_initialised(code)) return true;
   type_code_to_type_name[code] = make_type_name<T>();
   type_name_to_type_code[make_type_name<T>()]= code;
   code_to_deserialization_fnts[code] = [](std::string const &s) { return _object( triqs::deserialize<T>(s),"");};
   code_to_h5_read_fnts[code] = [](h5::group const &f,std::string const &s) ->_object { T n; h5::make_h5_read(&n)(f,s); return _object(std::move(n),"");};
   //code_to_h5_read_fnts[code] = [](h5::group const &f,std::string const &s) ->_object { T n; h5_read(f,s,n); return _object(std::move(n),true);};
   auto h5_scheme = get_triqs_hdf5_data_scheme(T());
   if (h5_scheme != "") h5_scheme_to_code[h5_scheme] = code;
   //std::cerr  << " registering " << type_code_to_type_name[code] << "h5 scheme "<< h5_scheme<< std::endl ;
   return false;
  }
 };

 // special case for array, we need to fill one more table
 template<typename T, int R> struct _object::register_type<arrays::array_c<T,R>>{
  static bool invoke() {
   typedef arrays::array_c<T,R> A;
   typedef arrays::array<T,R> Aa;
   if (is_initialised(type_code<A>())) return true;
   type_code_to_type_name[type_code<A>()] = make_type_name<A>();
   type_name_to_type_code[make_type_name<A>()]= type_code<A>();
   code_to_deserialization_fnts[type_code<A>()] = [](std::string const &s) { return _object( triqs::deserialize<A>(s),"");};
    //code_to_h5_read_fnts[type_code<A>()] = [](h5::group const &f,std::string const &s) ->_object { A n; h5_read(f,s,n); return _object(std::move(n),true);};
   code_to_h5_read_fnts[type_code<A>()] = [](h5::group const &f,std::string const &s) ->_object { Aa a; h5::make_h5_read(&a)(f,s); return _object(A(a),"");};
   code_element_rank_to_code_array[std::make_pair(type_code<T>(), R)] = type_code<A>();
   auto h5_scheme = get_triqs_hdf5_data_scheme(Aa());
   if (h5_scheme != "") h5_scheme_to_code[h5_scheme] = type_code<A>();
   return false;
  }
 };

 // --------------------- arithmetic operations are deleted for _object  ---------------------------------

#define DELETE_OP(op)\
 template <typename LHS, typename RHS> \
 typename std::enable_if< std::is_same<LHS,_object>::value || std::is_same<RHS,_object>::value>::type\
 operator op( LHS const &, RHS const &) = delete;
 DELETE_OP(+); DELETE_OP(-); DELETE_OP(*); DELETE_OP(/);
#undef DELETE_OP

 // --------------------- extraction with strict type checking for python ---------------------------------

  template<typename T> struct extract_strict  {
  typedef typename _object_collapse_type<T>::type T_stored;
  static bool is_possible (_object const &obj) { return obj.has_type<T_stored>(); }
  static T invoke(_object const &obj) {
   if (! is_possible(obj))
    TRIQS_RUNTIME_ERROR<<"extraction of "<< obj.name() << "impossible : type mismatch. Got a "<<obj.type_name()<< " while I am supposed to extract a "<<_object::make_type_name<T>();
   return T(* static_cast<const T_stored*>(obj.get_void_ptr()));
  }
 };

 // --------------------- extraction with more relaxed type checking for C++ ---------------------------------

 template<typename T> T extract(_object const &obj);

 template<typename T> T lex_cast_from_string(_object const &obj) {
  std::string s = extract<std::string>(obj);
  try { return boost::lexical_cast<T>(s); }
  catch(boost::bad_lexical_cast &) { TRIQS_RUNTIME_ERROR << " extraction : can not read the string "<<s <<" into a "<< _object::make_type_name<T>(); }
 }

 template<typename T> struct lexical_cast_make_sense : std::is_arithmetic<T>{};
 //template<typename T> constexpr bool lexical_cast_make_sense() { return std::is_arithmetic<T>::value; }

 template<typename T> T extract1(_object const &obj, std::false_type) {
  typedef typename _object_collapse_type<T>::type coll_t;
  if (! obj.has_type<coll_t>())
   TRIQS_RUNTIME_ERROR<<"extraction of "<< obj.name() << " impossible : type mismatch. Got "<<obj.type_name()<< ", while I am supposed to extract a "<<_object::make_type_name<T>();
  return * static_cast<const coll_t*>(obj.get_void_ptr());
 }

 template<typename T> T extract1(_object const &obj, std::true_type) {
  // if T is not a string, and object contains a string, attempt lexical_cast
  if ((!std::is_same<T,std::string>::value) && (obj.has_type<std::string>())) { return lex_cast_from_string<T>(obj); }
  return extract1<T>(obj, std::false_type()); 
 }

 template<typename T> T extract(_object const &obj) { 
   return extract1<T>(obj, std::integral_constant<bool,lexical_cast_make_sense<T>::value>());
 }

 template<> // specialize for double since we can make int -> double conversion
  inline double extract(_object const & obj) {
   if (obj.has_type<double>()) return * static_cast<const double*>(obj.get_void_ptr());
   if (obj.has_type<std::string>()) {return lex_cast_from_string<double>(obj); }
#define TRANSFORM_TYPE(T) if (obj.has_type<T>()) return extract<T>(obj)
   TRANSFORM_TYPE(int);
   //TRANSFORM_TYPE(unsigned int);
   TRANSFORM_TYPE(long);
   //TRANSFORM_TYPE(unsigned long);
   TRANSFORM_TYPE(short);
   //TRANSFORM_TYPE(unsigned short);
   TRANSFORM_TYPE(long long);
   //TRANSFORM_TYPE(unsigned long long);
   //TRANSFORM_TYPE(float);
#undef TRANSFORM_TYPE
   TRIQS_RUNTIME_ERROR<<"extraction of "<< obj.name() << " impossible : type mismatch. Got "<<obj.type_name()<< ", while I am supposed to extract a double";
  }

// template<> // special case to size_t
//  inline size_t extract(_object const & obj) { return extract<long>(obj);}

 // --------------- _object cast op implementation ---------------------------------------

#define CAST_OPERATOR(r, data, T) inline _object::operator T () const{ return extract<T>(*this);}
 BOOST_PP_SEQ_FOR_EACH(CAST_OPERATOR, nil , TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST);
#undef CAST_OPERATOR

}}
#endif
