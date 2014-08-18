#include "./_field.hpp"
#include <boost/lexical_cast.hpp>

namespace triqs {
namespace params {

 std::map<std::type_index, std::string> _field::type_names;

 std::string _field::type_name(std::type_index i) {
  try {
   return type_names.at(i);
  }
  catch (...) {
   return i.name();
  }
 }

 //-----------------------------------------------------------------------

 void h5_write(h5::group g, std::string const &name, _field const &obj) {
  obj.p->h5_write_(g, name);
 };

 //-----------------------------------------------------------------------

 void h5_read(h5::group g, std::string const &name, _field &obj) {
  obj.p->h5_read_(g, name);
  obj.modified = true;
 };

 template <typename T> T lex_cast_from_string(_field const &obj) {
  std::string s = extract<std::string>(obj);
  try {
   return boost::lexical_cast<T>(s);
  }
  catch (boost::bad_lexical_cast &) {
   TRIQS_RUNTIME_ERROR << " extraction : cannot read the string " << s << " into a " << typeid(T).name();
  }
 }

 //-----------------------------------------------------
 template <> // specialize for double since we can make int -> double conversion
 double extract(_field const &obj) {
  if (obj.has_type<double>()) return *static_cast<const double *>(obj.get());
  if (obj.has_type<std::string>()) {
   return lex_cast_from_string<double>(obj);
  }
  try {
   if (obj.has_type<long>()) return extract<long>(obj);
  }
  catch(...){}
  TRIQS_RUNTIME_ERROR << "extraction of " << obj.name() << " impossible : type mismatch. Got " << obj.type_name()
                      << ", while I am supposed to extract a double";
 }

 //-----------------------------------------------------
 template <> // specialize for double since we can make int -> double conversion
 long extract(_field const &obj) {
  if (obj.has_type<long>()) return *static_cast<const long *>(obj.get());
  if (obj.has_type<std::string>()) {
   return lex_cast_from_string<long>(obj);
  }
  TRIQS_RUNTIME_ERROR << "extraction of " << obj.name() << " impossible : type mismatch. Got " << obj.type_name()
                      << ", while I am supposed to extract a double";
 }

// --------------- _field cast op implementation ---------------------------------------

#define CAST_OPERATOR(r, data, T)                                                                                                \
 _field::operator T() const { return extract<T>(*this); }
 BOOST_PP_SEQ_FOR_EACH(CAST_OPERATOR, nil, TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST);
#undef CAST_OPERATOR
}
}

