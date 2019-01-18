#pragma once
#include <functional>

namespace cpp2py {

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

  // details
  template <bool B> struct _bool {};
  template <typename T> struct _is_pointer : _bool<false> {};
  template <typename T> struct _is_pointer<T *> : _bool<true> {};
  template <> struct _is_pointer<PyObject *> : _bool<false> {}; // yes, false, it is a special case...

  // adapter needed for parsing with PyArg_ParseTupleAndKeywords later in the functions
  template <typename T> static int converter_for_parser_fnt_(PyObject *ob, T *p, _bool<false>) {
    if (!py_converter<T>::is_convertible(ob, true)) return 0;
    *p = std::move(convert_from_python<T>(ob)); // non wrapped types are converted to values, they can be moved !
    return 1;
  }
  template <typename T> static int converter_for_parser_fnt_(PyObject *ob, T **p, _bool<true>) {
    if (!convertible_from_python<T>(ob)) return 0;
    *p = &(convert_from_python<T>(ob));
    return 1;
  }
  template <typename T> static int converter_for_parser_fnt(PyObject *ob, T *p) { return converter_for_parser_fnt_(ob, p, _is_pointer<T>()); }

  template <typename R, typename... T> struct py_converter<std::function<R(T...)>> {

    static_assert(sizeof...(T) < 5, "More than 5 variables not implemented");
    typedef struct {
      PyObject_HEAD std::function<R(T...)> *_c;
    } std_function;

    static PyObject *std_function_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
      std_function *self;
      self = (std_function *)type->tp_alloc(type, 0);
      if (self != NULL) { self->_c = new std::function<R(T...)>{}; }
      return (PyObject *)self;
    }

    static void std_function_dealloc(std_function *self) {
      delete self->_c;
      self->ob_type->tp_free((PyObject *)self);
    }

    // technical details to implement the __call function of the wrapping python object, cf below
    // we are using the unpack trick of the apply proposal for the C++ standard : cf XXXX
    //
    // specialise the convertion of the return type in the void case
    template <typename RR, typename TU, int... Is>
    static PyObject *_call_and_treat_return(nop<RR>, std_function *pyf, TU const &tu, index_seq<Is...>) {
      return py_converter<RR>::c2py(pyf->_c->operator()(std::get<Is>(tu)...));
    }
    template <typename TU, int... Is> static PyObject *_call_and_treat_return(nop<void>, std_function *pyf, TU const &tu, index_seq<Is...>) {
      pyf->_c->operator()(std::get<Is>(tu)...);
      Py_RETURN_NONE;
    }

    using arg_tuple_t = std::tuple<T...>;
    using _int_max    = _int<sizeof...(T) - 1>;

    template <typename... U> static int _parse(_int<-1>, PyObject *args, arg_tuple_t &tu, U... u) {
      const char *format = "O&O&O&O&O&"; // change 5 for more arguments.
      static_assert(sizeof...(T) <= 5, "More than 5 not implement. Easy to do ...");
      return PyArg_ParseTuple(args, format + 2 * (5 - sizeof...(T)), u...);
    }
    template <int N, typename... U> static int _parse(_int<N>, PyObject *args, arg_tuple_t &tu, U... u) {
      return _parse(_int<N - 1>(), args, tu, converter_for_parser_fnt<typename std::tuple_element<N, typename std::decay<arg_tuple_t>::type>::type>,
                    &std::get<N>(tu), u...);
    }

    // the call function object ...
    // TODO : ADD THE REF AND POINTERS  in x ??
    static PyObject *std_function_call(PyObject *self, PyObject *args, PyObject *kwds) {
      arg_tuple_t x;
      if (!_parse(_int_max(), args, x)) return NULL;
      try {
        return _call_and_treat_return(nop<R>(), (std_function *)self, x, make_index_seq<sizeof...(T)>());
      }
      CATCH_AND_RETURN("calling C++ std::function ", NULL);
    }

    static PyTypeObject get_type() {
      return {
         PyObject_HEAD_INIT(NULL) 0,          /*ob_size*/
         "std_function",                      /*tp_name*/
         sizeof(std_function),                /*tp_basicsize*/
         0,                                   /*tp_itemsize*/
         (destructor)std_function_dealloc,    /*tp_dealloc*/
         0,                                   /*tp_print*/
         0,                                   /*tp_getattr*/
         0,                                   /*tp_setattr*/
         0,                                   /*tp_compare*/
         0,                                   /*tp_repr*/
         0,                                   /*tp_as_number*/
         0,                                   /*tp_as_sequence*/
         0,                                   /*tp_as_mapping*/
         0,                                   /*tp_hash */
         std_function_call,                   /*tp_call*/
         0,                                   /*tp_str*/
         0,                                   /*tp_getattro*/
         0,                                   /*tp_setattro*/
         0,                                   /*tp_as_buffer*/
         Py_TPFLAGS_DEFAULT,                  /*tp_flags*/
         "Internal wrapper of std::function", /* tp_doc */
         0,                                   /* tp_traverse */
         0,                                   /* tp_clear */
         0,                                   /* tp_richcompare */
         0,                                   /* tp_weaklistoffset */
         0,                                   /* tp_iter */
         0,                                   /* tp_iternext */
         0,                                   /* tp_methods */
         0,                                   /* tp_members */
         0,                                   /* tp_getset */
         0,                                   /* tp_base */
         0,                                   /* tp_dict */
         0,                                   /* tp_descr_get */
         0,                                   /* tp_descr_set */
         0,                                   /* tp_dictoffset */
         0,                                   /* tp_init */
         0,                                   /* tp_alloc */
         std_function_new,                    /* tp_new */
      };
    }

    static void ensure_type_ready(PyTypeObject &Type, bool &ready) {
      if (!ready) {
        Type = get_type();
        if (PyType_Ready(&Type) < 0) std::cerr << " Warning : ensure_type_ready has failed in function-lambda C++/Python converter " << std::endl;
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
      if (self != NULL) { self->_c = new std::function<R(T...)>{std::forward<U>(x)}; }
      return (PyObject *)self;
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyCallable_Check(ob)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::function a non callable object"); }
      return false;
    }

    static std::function<R(T...)> py2c(PyObject *ob) {
      static PyTypeObject Type;
      static bool ready = false;
      ensure_type_ready(Type, ready);
      // If we convert a wrapped std::function, just extract it.
      if (PyObject_TypeCheck(ob, &Type)) { return *(((std_function *)ob)->_c); }
      // otherwise, we build a new std::function around the python function
      pyref py_fnt = borrowed(ob);
      auto l       = [py_fnt](T... x) mutable -> R { // py_fnt is a pyref, it will keep the ref and manage the ref counting...
        pyref ret  = PyObject_CallFunctionObjArgs(py_fnt, (PyObject *)pyref(convert_to_python(x))..., NULL);
        return py_converter<R>::py2c(ret);
      };
      return l;
    }
  };

} // namespace cpp2py
