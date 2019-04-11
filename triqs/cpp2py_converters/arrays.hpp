#pragma once

// in the generated wrapper, we only "import_array" if this macro is defined.
#define TRIQS_IMPORTED_CONVERTERS_ARRAYS

namespace cpp2py {

  // is_view
  template <typename T, int R> struct is_view<triqs::arrays::array_view<T, R>> : std::true_type {};
  template <typename T> struct is_view<triqs::arrays::matrix_view<T>> : std::true_type {};
  template <typename T> struct is_view<triqs::arrays::vector_view<T>> : std::true_type {};

  template <typename T, int R> struct is_view<triqs::arrays::array_const_view<T, R>> : std::true_type {};
  template <typename T> struct is_view<triqs::arrays::matrix_const_view<T>> : std::true_type {};
  template <typename T> struct is_view<triqs::arrays::vector_const_view<T>> : std::true_type {};

  // FIXME : std::array instead
  // --- mini_vector<T,N>---
  // via std::vector
  template <typename T, int N> struct py_converter<triqs::utility::mini_vector<T, N>> {
    using conv = py_converter<std::vector<T>>;
    static PyObject *c2py(triqs::utility::mini_vector<T, N> const &v) { return conv::c2py(v.to_vector()); }
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv::is_convertible(ob, raise_exception); }
    static triqs::utility::mini_vector<T, N> py2c(PyObject *ob) { return conv::py2c(ob); }
  };

  // -----------------------------------
  // array
  // -----------------------------------

  inline static void import_numpy() {
    static bool init = false;
    if (!init) {
      _import_array();
      // std::cerr << "importing array"<<std::endl;
      init = true;
    }
  }

  template <typename ArrayType> struct py_converter_array_impl {
    static ArrayType py2c(PyObject *ob) {
      import_numpy();
      return ArrayType(ob);
    }
    static bool is_convertible(PyObject *ob, bool raise_exception) {
      import_numpy();
      triqs::arrays::numpy_interface::numpy_extractor<typename ArrayType::value_type, ArrayType::indexmap_type::rank> E;
      // FIXME C++17 Add if constexpr
      if (is_view<ArrayType>::value and not raise_exception) { // quick decision, no need to build all the error strings
        return E.is_convertible_to_view(ob);
      } else {                                               // is a regular type or we want the error message
        bool ok = E.extract(ob, !is_view<ArrayType>::value); // if not a view, enforce_copy
        if (!ok and raise_exception) {
          std::string mess = "Cannot convert to array/matrix/vector : the error was : \n" + E.error;
          PyErr_SetString(PyExc_TypeError, mess.c_str());
        }
        return ok;
      }
    }
  };

  template <typename ArrayType> struct py_converter_array_cvt : py_converter_array_impl<ArrayType> {
    static PyObject *c2py(ArrayType const &x) = delete; // Can not convert a C++ const_view into python, it violates const correctness
  };

  template <typename ArrayType> struct py_converter_array : py_converter_array_impl<ArrayType> {
    static PyObject *c2py(ArrayType const &x) {
      import_numpy();
      return x.to_python();
    }
  };

  template <typename T, int R> struct py_converter<triqs::arrays::array_view<T, R>> : py_converter_array<triqs::arrays::array_view<T, R>> {};
  template <typename T> struct py_converter<triqs::arrays::matrix_view<T>> : py_converter_array<triqs::arrays::matrix_view<T>> {};
  template <typename T> struct py_converter<triqs::arrays::vector_view<T>> : py_converter_array<triqs::arrays::vector_view<T>> {};

  template <typename T, int R>
  struct py_converter<triqs::arrays::array_const_view<T, R>> : py_converter_array<triqs::arrays::array_const_view<T, R>> {};
  template <typename T> struct py_converter<triqs::arrays::matrix_const_view<T>> : py_converter_array<triqs::arrays::matrix_const_view<T>> {};
  template <typename T> struct py_converter<triqs::arrays::vector_const_view<T>> : py_converter_array<triqs::arrays::vector_const_view<T>> {};

  // FIXME : better, but there is ONE error in lattice_tools !
  //struct py_converter<triqs::arrays::array_const_view<T, R>> : py_converter_array_cvt<triqs::arrays::array_const_view<T, R>> {};
  //template <typename T> struct py_converter<triqs::arrays::matrix_const_view<T>> : py_converter_array_cvt<triqs::arrays::matrix_const_view<T>> {};
  //template <typename T> struct py_converter<triqs::arrays::vector_const_view<T>> : py_converter_array_cvt<triqs::arrays::vector_const_view<T>> {};

  template <typename T, int R> struct py_converter<triqs::arrays::array<T, R>> : py_converter_array<triqs::arrays::array<T, R>> {};
  template <typename T> struct py_converter<triqs::arrays::matrix<T>> : py_converter_array<triqs::arrays::matrix<T>> {};
  template <typename T> struct py_converter<triqs::arrays::vector<T>> : py_converter_array<triqs::arrays::vector<T>> {};

  // -----------------------------------
  // range
  // -----------------------------------

  // range can not be directly converted from slice (slice is more complex)
  // convert from python slice and int (interpreted are slice(i,i+1,1))
  itertools::range range_from_slice(PyObject *ob, long len) {
    if (PyInt_Check(ob)) {
      long i = PyInt_AsLong(ob);
      if ((i < -len) || (i >= len)) CPP2PY_RUNTIME_ERROR << "Integer index out of range : expected [0," << len << "], got " << i;
      if (i < 0) i += len;
      // std::cerr  << " range int "<< i << std::endl;
      return {i, i + 1, 1};
    }
    Py_ssize_t start, stop, step, slicelength;
    if (!PySlice_Check(ob) || (PySlice_GetIndicesEx((PySliceObject *)ob, len, &start, &stop, &step, &slicelength) < 0))
      CPP2PY_RUNTIME_ERROR << "Can not converted the slice to C++";
    // std::cerr  << "range ( "<< start << " "<< stop << " " << step<<std::endl;
    return {start, stop, step};
  }

  template <> struct py_converter<itertools::range> {
    static PyObject *c2py(itertools::range const &r) {
      return PySlice_New(convert_to_python(r.first()), convert_to_python(r.last()), convert_to_python(r.step()));
    }
    static itertools::range py2c(PyObject *ob)                 = delete;
    static bool is_convertible(PyObject *ob, bool raise_exception) = delete;
  };
} // namespace cpp2py
