#pragma once
#include <Python.h>
#include "structmember.h"
#include <string>
#include <complex>
#include <vector>
#include <triqs/utility/exceptions.hpp>

#pragma clang diagnostic ignored "-Wdeprecated-writable-strings"
#pragma GCC diagnostic ignored "-Wwrite-strings"

// I can use the trace in triqs::exception
#define CATCH_AND_RETURN(MESS,RET)\
 catch(triqs::keyboard_interrupt const & e) {\
 PyErr_SetString(PyExc_KeyboardInterrupt, e.what());\
 return RET; }\
 catch(triqs::exception const & e) {\
 auto err = std::string("Error " MESS "\nC++ error was : \n") + e.what();\
 PyErr_SetString(PyExc_RuntimeError, err.c_str());\
 return RET; }\
 catch(std::exception const & e) {\
 auto err = std::string("Error " MESS "\nC++ error was : \n") + e.what();\
 PyErr_SetString(PyExc_RuntimeError, err.c_str());\
 return RET; }\
 catch(...) { PyErr_SetString(PyExc_RuntimeError,"Unknown error " MESS ); return RET; }\

namespace triqs { namespace py_tools { 

//--------------------- pyref -----------------------------

// a little class to hold an owned ref, make sure it is decref at destruction
class pyref {
 PyObject * ob = NULL;
 public:
 pyref() = default;
 pyref(PyObject *new_ref) : ob(new_ref){}
 ~pyref() { Py_XDECREF(ob);}
 operator PyObject *() const { return ob;}
 PyObject * new_ref() const { Py_XINCREF(ob); return ob;}
 explicit operator bool() const { return (ob!=NULL);}
 bool is_null() const { return ob==NULL;}
 bool is_None() const { return ob==Py_None;}
 pyref attr(const char * s) { return (ob ? PyObject_GetAttrString(ob,s) : NULL);} // NULL : pass the error in chain call x.attr().attr()....
 pyref operator()(PyObject * a1) { return (ob ? PyObject_CallFunctionObjArgs(ob,a1,NULL) : NULL);} // NULL : pass the error in chain call x.attr().attr()....
 pyref operator()(PyObject * a1,PyObject * a2) { return (ob ? PyObject_CallFunctionObjArgs(ob,a1,a2,NULL) : NULL);} // NULL : pass the error in chain call x.attr().attr()....
 pyref(pyref const&p) {ob = p.ob; Py_XINCREF(ob);}
 pyref(pyref && p){ ob = p.ob; p.ob=NULL;}
 pyref& operator =(pyref const&) = delete;
 pyref& operator =(pyref &&p) {ob = p.ob; p.ob=NULL; return *this;}
 static pyref module(std::string const &module_name) { return PyImport_ImportModule(module_name.c_str()); }
 static pyref string(std::string const &s) { return PyString_FromString(s.c_str());} 
};

 inline pyref borrowed(PyObject * ob) { Py_XINCREF(ob); return {ob};}

//---------------------  py_converters -----------------------------

// default version for a wrapped type. To be specialized later.
// py2c behaviour is undefined is is_convertible return false
template<typename T> struct py_converter;
 //{ 
 //  static PyObject * c2py(T const & x);
 //  static T & py2c(PyObject * ob);
 //  static bool is_convertible(PyObject * ob, bool raise_exception);
 //}

// We only use these functions in the code, not converter
// TODO : Does c2py return NULL in failure ? Or is it undefined...
template <typename T> static PyObject *convert_to_python(T &&x) {
 return py_converter<typename std::decay<T>::type>::c2py(std::forward<T>(x));
}
template<typename T> static auto convert_from_python(PyObject * ob) -> decltype(py_converter<T>::py2c(ob)) { return py_converter<T>::py2c(ob);}
template <typename T> static bool convertible_from_python(PyObject *ob, bool raise_exception) {
 return py_converter<T>::is_convertible(ob, raise_exception);
}

// details
template <bool B> struct _bool {};
template <typename T> struct _is_pointer : _bool<false> {};
template <typename T> struct _is_pointer<T *> : _bool<true> {};
template <> struct _is_pointer<PyObject  *> : _bool<false> {}; // yes, false, it is a special case...

// adapter needed for parsing with PyArg_ParseTupleAndKeywords later in the functions
template <typename T> static int converter_for_parser_(PyObject *ob, T *p, _bool<false>) {
 if (!py_converter<T>::is_convertible(ob, true)) return 0;
 *p = std::move(convert_from_python<T>(ob)); // non wrapped types are converted to values, they can be moved !
 return 1;
}
template <typename T> static int converter_for_parser_(PyObject *ob, T **p, _bool<true>) {
 if (!convertible_from_python<T>(ob)) return 0;
 *p = &(convert_from_python<T>(ob));
 return 1;
}
template <typename T> static int converter_for_parser(PyObject *ob, T *p) {
 return converter_for_parser_(ob, p, _is_pointer<T>());
}

// -----------------------------------
//    Tools for the implementation of reduce (V2)
// -----------------------------------

 // auxiliary object to reduce the object into a tuple
 class reductor {
  std::vector<PyObject *> elem;
  PyObject *as_tuple() {
   int l = elem.size();
   PyObject *tup = PyTuple_New(l);
   for (int pos = 0; pos < l; ++pos) PyTuple_SetItem(tup, pos, elem[pos]);
   return tup;
  }
  public:
  template <typename T> reductor & operator&(T &x) { elem.push_back(convert_to_python(x)); return *this;}
  template<typename T> 
  PyObject * apply_to(T & x) { x.serialize(*this,0); return as_tuple();}
 };

 // inverse : auxiliary object to reconstruct the object from the tuple ...
 class reconstructor {
  PyObject * tup; // borrowed ref
  int pos=0, pos_max = 0;
  public:
  reconstructor(PyObject *borrowed_ref) : tup(borrowed_ref) { pos_max = PyTuple_Size(tup)-1;}
  template <typename T> reconstructor &operator&(T &x) {
   if (pos > pos_max) TRIQS_RUNTIME_ERROR << " Tuple too short in reconstruction";
   x = convert_from_python<T>(PyTuple_GetItem(tup, pos++));
   return *this;
  }
 };

 // no protection for convertion !
 template <typename T> struct py_converter_from_reductor {
 template<typename U> static PyObject *c2py(U && x) { return reductor{}.apply_to(x); }
 static T py2c(PyObject *ob) { 
  T res;
  auto r = reconstructor{ob};
  res.serialize(r, 0);
  return res;
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) { return true;}
};

// -----------------------------------
//       basic types 
// -----------------------------------

// PyObject * 
template <> struct py_converter<PyObject *> {
 static PyObject *c2py(PyObject *ob) { return ob; }
 static PyObject *py2c(PyObject *ob) { return ob; }
 static bool is_convertible(PyObject *ob, bool raise_exception) { return true;}
};

// --- bool 
template <> struct py_converter<bool> {
 static PyObject *c2py(bool b) {
  if (b)
   Py_RETURN_TRUE;
  else
   Py_RETURN_FALSE;
 }
 static bool py2c(PyObject *ob) { return ob == Py_True; }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyBool_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to bool");}
  return false;
 }
};

// --- long

template <> struct py_converter<long> {
 static PyObject *c2py(long i) { return PyInt_FromLong(i); }
 static long py2c(PyObject *ob) { return PyInt_AsLong(ob); }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to long");}
  return false;
 }
};

template <> struct py_converter<int> : py_converter<long> {};
template <> struct py_converter<size_t> : py_converter<long> {};

// --- double

template <> struct py_converter<double> {
 static PyObject *c2py(double x) { return PyFloat_FromDouble(x); }
 static double py2c(PyObject *ob) { return PyFloat_AsDouble(ob); }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyFloat_Check(ob) || PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to double");}
  return false;
 }
};

// --- complex

template <> struct py_converter<std::complex<double>> {
 static PyObject *c2py(std::complex<double> x) { return PyComplex_FromDoubles(x.real(), x.imag()); }
 static std::complex<double> py2c(PyObject *ob) {
  if (PyComplex_Check(ob)) {
    auto r = PyComplex_AsCComplex(ob);
    return {r.real, r.imag};
  }
  return PyFloat_AsDouble(ob);
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyComplex_Check(ob) || PyFloat_Check(ob) || PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to complex");}
  return false;
 }
};

// --- string 

template <> struct py_converter<std::string> {
 static PyObject *c2py(std::string const &x) { return PyString_FromString(x.c_str()); }
 static std::string py2c(PyObject *ob) { return PyString_AsString(ob); }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyString_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to string");}
  return false;
 }
};

template <> struct py_converter<const char *> {
 static PyObject *c2py(const char *x) { return PyString_FromString(x); }
 static const char * py2c(PyObject *ob) { return PyString_AsString(ob); }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyString_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to string");}
  return false;
 }
};

// ---  h5 group of h5py into a triqs::h5 group

template <> struct py_converter<triqs::h5::group> {
 
 static PyObject *c2py(std::string const &x) =delete;
 
 static pyref group_type;

 static triqs::h5::group py2c (PyObject * ob) {
  int id = PyInt_AsLong(borrowed(ob).attr("id").attr("id"));
  int cmp = PyObject_RichCompareBool((PyObject *)ob->ob_type, group_type, Py_EQ);
  return triqs::h5::group (id, (cmp==1));
 }
 
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (group_type.is_null()) { 
   group_type = pyref::module("h5py").attr("highlevel").attr("Group");
   if (PyErr_Occurred()) TRIQS_RUNTIME_ERROR << "Can not load h5py module and find the group in it";
  }
  int cmp = PyObject_RichCompareBool((PyObject *)ob->ob_type, group_type, Py_EQ);
  if (cmp<0) {
   if (raise_exception) { 
      PyErr_SetString(PyExc_TypeError, "hd5 : internal : comparison to group type has failed !!");
   }
   return false;
  }
  pyref id_py = borrowed(ob).attr("id").attr("id");
  if ((!id_py) ||(!PyInt_Check((PyObject*)id_py))) {
    if (raise_exception) { 
      PyErr_SetString(PyExc_TypeError, "hd5 : INTERNAL : group id.id is not an int !!");
   }
   return false;
  }
  return true; 
 }
};

// --- vector ---

template <typename T> struct py_converter<std::vector<T>> {
 static PyObject * c2py(std::vector<T> const &v) {
  PyObject * list = PyList_New(0);
  for (auto & x : v) if (PyList_Append(list, py_converter<T>::c2py(x)) == -1) { Py_DECREF(list); return NULL;} // error
  return list;
 }
  static PyObject * c2py(std::vector<T> &v) {
  PyObject * list = PyList_New(0);
  for (auto & x : v) if (PyList_Append(list, py_converter<T>::c2py(x)) == -1) { Py_DECREF(list); return NULL;} // error
  return list;
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!PySequence_Check(ob)) goto _false;
  {
   pyref seq = PySequence_Fast(ob, "expected a sequence");
   int len = PySequence_Size(ob);
   for (int i = 0; i < len; i++) if (!py_converter<T>::is_convertible(PySequence_Fast_GET_ITEM((PyObject*)seq, i),raise_exception)) goto _false; //borrowed ref
   return true;
  }
  _false: 
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to std::vector");}
  return false;
 }
 static std::vector<T> py2c(PyObject * ob) {
  pyref seq = PySequence_Fast(ob, "expected a sequence");
  std::vector<T> res;
  int len = PySequence_Size(ob);
  for (int i = 0; i < len; i++) res.push_back(py_converter<T>::py2c(PySequence_Fast_GET_ITEM((PyObject*)seq, i))); //borrowed ref
  return res;
 }
};

// --- pair ---

template <typename T1, typename T2> struct py_converter<std::pair<T1,T2>> {
 static PyObject * c2py(std::pair<T1,T2> const &p) {
  PyObject * list = PyList_New(0);
  if (PyList_Append(list, py_converter<T1>::c2py(std::get<0>(p))) == -1) { Py_DECREF(list); return NULL;} // error
  if (PyList_Append(list, py_converter<T2>::c2py(std::get<1>(p))) == -1) { Py_DECREF(list); return NULL;} // error
  return list;
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!PySequence_Check(ob)) goto _false;
  {
   pyref seq = PySequence_Fast(ob, "expected a sequence");
   if (!py_converter<T1>::is_convertible(PySequence_Fast_GET_ITEM((PyObject*)seq, 0),raise_exception)) goto _false; //borrowed ref
   if (!py_converter<T2>::is_convertible(PySequence_Fast_GET_ITEM((PyObject*)seq, 1),raise_exception)) goto _false; //borrowed ref
   return true;
  }
  _false: 
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to std::pair");}
  return false;
 }

 static std::pair<T1,T2> py2c(PyObject * ob) {
  pyref seq = PySequence_Fast(ob, "expected a sequence");
  return std::make_pair(
    py_converter<T1>::py2c(PySequence_Fast_GET_ITEM((PyObject*)seq, 0)),
    py_converter<T2>::py2c(PySequence_Fast_GET_ITEM((PyObject*)seq, 1))
    );
 }
};

// --- mini_vector<T,N>---
// via std::vector
template <typename T, int N> struct py_converter<triqs::utility::mini_vector<T,N>> {
 using conv = py_converter<std::vector<T>>;
 static PyObject * c2py(triqs::utility::mini_vector<T,N> const &v) {
  return conv::c2py( v.to_vector());
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  return conv::is_convertible(ob,raise_exception);
 }
 static triqs::utility::mini_vector<T,N> py2c(PyObject * ob) {
  return conv::py2c(ob);
 }
};

// --- array 

template <typename ArrayType> struct py_converter_array {
 static PyObject *c2py(ArrayType const &x) { return x.to_python(); }
 static ArrayType py2c(PyObject *ob) {
  return ArrayType (ob);
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  try {
   py2c(ob);
   return true;
  }
  catch (...) {
   if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to array/matrix/vector");}
   return false;
  }
 }
};

template <typename T, int R> struct py_converter<triqs::arrays::array_view<T, R>> : py_converter_array<triqs::arrays::array_view<T, R>> {};
template <typename T> struct py_converter<triqs::arrays::matrix_view<T>> : py_converter_array<triqs::arrays::matrix_view<T>> {};
template <typename T> struct py_converter<triqs::arrays::vector_view<T>> : py_converter_array<triqs::arrays::vector_view<T>> {};

template <typename T, int R> struct py_converter<triqs::arrays::array_const_view<T, R>> : py_converter_array<triqs::arrays::array_const_view<T, R>> {};
template <typename T> struct py_converter<triqs::arrays::matrix_const_view<T>> : py_converter_array<triqs::arrays::matrix_const_view<T>> {};
template <typename T> struct py_converter<triqs::arrays::vector_const_view<T>> : py_converter_array<triqs::arrays::vector_const_view<T>> {};

template <typename T, int R> struct py_converter<triqs::arrays::array<T, R>> : py_converter_array<triqs::arrays::array<T, R>> {};
template <typename T> struct py_converter<triqs::arrays::matrix<T>> : py_converter_array<triqs::arrays::matrix<T>> {};
template <typename T> struct py_converter<triqs::arrays::vector<T>> : py_converter_array<triqs::arrays::vector<T>> {};

// --- range

// convert from python slice and int (interpreted are slice(i,i+1,1))
template <> struct py_converter<triqs::arrays::range> {
 static PyObject *c2py(triqs::arrays::range const &r) {
  return PySlice_New(convert_to_python(r.first()), convert_to_python(r.last()), convert_to_python(r.step()));
 }
 static triqs::arrays::range py2c(PyObject *ob) {
  if (PyInt_Check(ob)) {
    int i = PyInt_AsLong(ob); 
    return {i,i+1,1};
  }
  int len = 4; // no clue what this len is ??
  Py_ssize_t start, stop, step, slicelength;
  if (PySlice_GetIndicesEx((PySliceObject *)ob, len, &start, &stop, &step, &slicelength) < 0) return {};
  return {start, stop, step};
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PySlice_Check(ob) || PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to python slice");}
  return false;
 }
};

// --- nothing  <--> None ----
#ifdef TRIQS_GF_INCLUDED

template<> struct py_converter<triqs::gfs::nothing> {
 static PyObject *c2py(triqs::gfs::nothing g) { Py_RETURN_NONE;}
 static bool is_convertible(PyObject *ob, bool raise_exception) { 
  if (ob ==Py_None) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to triqs::gfs::nothing : can only convert None");}
  return false;
 }
 static triqs::gfs::nothing py2c(PyObject *ob) { return {}; }
};


template<> struct py_converter<triqs::gfs::indices_2> : py_converter_from_reductor<triqs::gfs::indices_2>{};
template<bool B> struct py_converter<triqs::gfs::matsubara_domain<B>> : py_converter_from_reductor<triqs::gfs::matsubara_domain<B>>{};
template<> struct py_converter<triqs::gfs::R_domain> : py_converter_from_reductor<triqs::gfs::R_domain>{};

// Converter for Block gf
template <typename... T> struct py_converter<triqs::gfs::gf_view<triqs::gfs::block_index, triqs::gfs::gf<T...>>> {

 using gf_type = triqs::gfs::gf<T...>;
 using gf_view_type = triqs::gfs::gf_view<T...>;
 using c_type = triqs::gfs::gf_view<triqs::gfs::block_index, gf_type>;

 static PyObject *c2py(c_type g) {
  // rm the view_proxy
  std::vector<gf_view_type> vg; 
  vg.reserve(g.data().size());
  for (auto const & x : g.data()) vg.push_back(x);
  pyref v_gf = convert_to_python(vg);
  pyref v_names = convert_to_python(g.mesh().domain().names());
  pyref cls = pyref::module("pytriqs.gf.local").attr("BlockGf");
  if (cls.is_null()) TRIQS_RUNTIME_ERROR <<"Can not find the pytriqs.gf.local.BlockGf";
  pyref kw = PyDict_New();
  PyDict_SetItemString(kw, "name_list", v_names);
  PyDict_SetItemString(kw, "block_list", v_gf);
  pyref empty_tuple = PyTuple_New(0);
  return PyObject_Call(cls, empty_tuple, kw);
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  pyref cls = pyref::module("pytriqs.gf.local").attr("BlockGf");
  if (cls.is_null()) TRIQS_RUNTIME_ERROR <<"Can not find the pytriqs.gf.local.BlockGf";
  int i = PyObject_IsInstance(ob, cls);
  if (i == -1) { // an error has occurred
   i = 0;
   if (!raise_exception) PyErr_Clear();
  }
  if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a BlockGf");
  return i;
 }

 static c_type py2c(PyObject *ob) {
  pyref x = borrowed(ob);
  pyref names = x.attr("_BlockGf__indices");
  pyref gfs = x.attr("_BlockGf__GFlist");
  return make_block_gf_view_from_vector(convert_from_python<std::vector<std::string>>(names),
                                        convert_from_python<std::vector<gf_view_type>>(gfs));
 }
};

// Converter for scalar_valued gf : reinterpreted as 1x1 matrix
template <typename Variable, typename Opt> struct py_converter<triqs::gfs::gf_view<Variable, triqs::gfs::scalar_valued, Opt>>{

 using conv = py_converter<triqs::gfs::gf_view<Variable, triqs::gfs::matrix_valued, Opt>>;
 using c_t = triqs::gfs::gf_view<Variable, triqs::gfs::scalar_valued, Opt>;

 static PyObject *c2py(c_t g) {
  return conv::c2py(reinterpret_scalar_valued_gf_as_matrix_valued(g));
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!conv::is_convertible(ob,raise_exception)) return false;
  auto g = conv::py2c(ob); // matrix view
  if (get_target_shape(g) == triqs::arrays::make_shape(1,1)) return true;
  if (raise_exception) PyErr_SetString(PyExc_RuntimeError,"The green function is not of dimension 1x1 : can not be reinterpreted as a scalar_valued Green function");
  return false;
 }

 static c_t py2c(PyObject *ob) {
  return slice_target_to_scalar(conv::py2c(ob),0,0);
 }
};

#endif

// ---- function ----

// a few useful meta tricks
template <int N> struct _int {};
template <int... N> struct index_seq {};
template <typename U> struct nop {};

template <int N> struct _make_index_seq;
template <int N> using make_index_seq = typename _make_index_seq<N>::type;
template <> struct _make_index_seq<0> { using type = index_seq<>; };
template <> struct _make_index_seq<1> { using type = index_seq<0>; };
template <> struct _make_index_seq<2> { using type = index_seq<0, 1>; };
template <> struct _make_index_seq<3> { using type = index_seq<0, 1, 2>; };
template <> struct _make_index_seq<4> { using type = index_seq<0, 1, 2, 3>; };
template <> struct _make_index_seq<5> { using type = index_seq<0, 1, 2, 3, 4>; };

template <int N> struct make_format { static const char * value;};

template <typename R, typename... T> struct py_converter<std::function<R(T...)>> {

 static_assert(sizeof...(T) < 5, "More than 5 variables not implemented");
  typedef struct {
    PyObject_HEAD 
    std::function<R(T...)> *_c;
  } std_function;

 static PyObject* std_function_new (PyTypeObject *type, PyObject *args, PyObject *kwds) {
  std_function *self;
  self = (std_function *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->_c = new std::function<R(T...)>{}; 
  }
  return (PyObject *)self;
 }

 static void std_function_dealloc(std_function* self) {
   delete self->_c;
   self->ob_type->tp_free((PyObject*)self);
 }

 // technical details to implement the __call function of the wrapping python object, cf below
 // we are using the unpack trick of the apply proposal for the C++ standard : cf XXXX
 //
 // specialise the convertion of the return type in the void case
 template <typename RR, typename TU, int... Is>
 static PyObject *_call_and_treat_return(nop<RR>, std_function *pyf, TU const &tu, index_seq<Is...>) {
  return py_converter<RR>::c2py(pyf->_c->operator()(std::get<Is>(tu)...));
 }
 template <typename TU, int... Is> 
 static PyObject *_call_and_treat_return(nop<void>, std_function *pyf, TU const &tu, index_seq<Is...>) {
  pyf->_c->operator()(std::get<Is>(tu)...);
  Py_RETURN_NONE;
 }

 using arg_tuple_t = std::tuple<T...>;
 using _int_max = _int<sizeof...(T) - 1>;

 template <typename... U> static int _parse(_int<-1>, PyObject *args, arg_tuple_t &tu, U... u) {
  return PyArg_ParseTuple(args, make_format<sizeof...(T)>::value, u...);
 }
 template <int N, typename... U> static int _parse(_int<N>, PyObject *args, arg_tuple_t &tu, U... u) {
  return _parse(_int<N - 1>(), args, tu,
                converter_for_parser<typename std::tuple_element<N, typename std::decay<arg_tuple_t>::type>::type>,
                &std::get<N>(tu), u...);
 }

 // the call function object ...
 // TODO : ADD THE REF AND POINTERS  in x ??
 static PyObject *std_function_call(PyObject *self, PyObject *args, PyObject *kwds) {
  arg_tuple_t x; 
  if (!_parse(_int_max(), args,  x)) return NULL;
   try {
    return _call_and_treat_return(nop<R>(), (std_function *)self, x, make_index_seq<sizeof...(T)>());
   }
   CATCH_AND_RETURN("calling C++ std::function ", NULL);
 }

 static PyTypeObject get_type() { return {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "std_function",             /*tp_name*/
    sizeof(std_function),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)std_function_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    std_function_call,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT , /*tp_flags*/
    "Internal wrapper of std::function",                /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,             /* tp_methods */
    0,              /* tp_members */
    0,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    std_function_new,                 /* tp_new */
 };}

 static void ensure_type_ready(PyTypeObject &Type, bool &ready) {
  if (!ready) {
   Type = get_type();
   if (PyType_Ready(&Type) < 0) std::cout << " aie ie " << std::endl;
   ready = true;
  }
 }

 // U can be anything, typically a lambda
 template <typename U> static PyObject *c2py(U &&x) {
   std_function *self;
   static PyTypeObject Type;
   static bool ready = false;
   ensure_type_ready(Type, ready); 
   self = (std_function *)Type.tp_alloc(&Type, 0);
   if (self != NULL) {
     self->_c = new std::function<R(T...)>{ std::forward<U>(x) };
   }
   return (PyObject *)self;
 }
 
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyCallable_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Can not convert to std::function a non callable object");}
  return false;
 }

 static std::function<R(T...)> py2c(PyObject * ob) {
  static PyTypeObject Type;
  static bool ready = false;
  ensure_type_ready(Type, ready);
  // If we convert a wrapped std::function, just extract it. 
  if (PyObject_TypeCheck(ob, &Type)) { return *(((std_function *)ob)->_c);}
  // otherwise, we build a new std::function around the python function
  pyref py_fnt = borrowed(ob);
  auto l = [py_fnt](T... x) mutable -> R { // py_fnt is a pyref, it will keep the ref and manage the ref counting... 
   pyref ret  = PyObject_CallFunctionObjArgs(py_fnt,  (PyObject*)pyref(convert_to_python(x))...,NULL);
   return py_converter<R>::py2c(ret);
  };
  return l;
 }
};
}}
