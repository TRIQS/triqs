/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include "../impl/common.hpp"
#include "../impl/traits.hpp"
#include "../../utility/mini_vector.hpp"
#include "../../utility/typeid_name.hpp"
#include "./numpy_extractor.hpp"

namespace triqs {
  namespace arrays {
    namespace numpy_interface {

      static const char *error_msg =
         "   Error from the Python to C++ converter of array/matrix/vector.\n   I am asked to take a *view* of Python numpy array.\n   However, it is impossible (a deep copy would be necessary) for the following reason : \n";

      // --------------------------------------------------------------------

      bool numpy_convertible_to_view_impl(PyObject *X, std::string const &type_name, int elementsType, int rank) {

        bool err = (X == NULL) or (_import_array() != 0) or (!PyArray_Check(X));
        if (err) return false;
        // now I am sure X is an array
        PyArrayObject *arr = (PyArrayObject *)X;
        err                = err or (elementsType != PyArray_TYPE(arr));
#ifdef PYTHON_NUMPY_VERSION_LT_17
        err = err or (arr->nd != rank);
#else
        err = err or (PyArray_NDIM(arr) != rank);
#endif
        return !err;
      }

      // --------------------------------------------------------------------
      // In case numpy_convertible_to_view return false, build the error message. Separated for speed reason (in Python convertion, we need the convertible function for overload
      // but we want the string just in case there is actually an error)

      static std::string numpy_view_convertion_get_error_impl(PyObject *X, std::string const &type_name, int elementsType, int rank) {

        if (!PyArray_Check(X)) return std::string{error_msg} + "   Indeed the object was not even an array !\n";

        if (elementsType != PyArray_TYPE((PyArrayObject *)X)) {
          PyObject *p             = PyObject_GetAttrString(X, "dtype");
          std::string actual_type = "";
          if (p) {
            PyObject *q = PyObject_GetAttrString(p, "name");
            if (q && PyUnicode_Check(q)) actual_type = PyUnicode_AsUTF8(q);
            Py_XDECREF(q);
          }
          Py_XDECREF(p);
          return std::string{error_msg} + "   Type mismatch of the elements.\n   The expected type by C+ is " + utility::demangle(type_name)
             + " while I receive an array of type " + actual_type + "\n";
        }
        PyArrayObject *arr = (PyArrayObject *)X;
#ifdef PYTHON_NUMPY_VERSION_LT_17
        if (arr->nd != rank)
          return std::string{error_msg} + "   Rank mismatch . numpy array is of rank " + std::to_string(arr->nd) + "while you ask for rank "
             + std::to_string(rank) + ". \n";
#else
        if (PyArray_NDIM(arr) != rank)
          return std::string{error_msg} + "   Rank mismatch . numpy array is of rank " + std::to_string(PyArray_NDIM(arr)) + "while you ask for rank "
             + std::to_string(rank) + ". \n";
#endif
        return "Internal Error : unknown error in numpy view convertion";
      }

      // --------------------------------------------------------------------

      std::pair<cpp2py::pyref, std::string> numpy_extractor_impl(PyObject *X, bool enforce_copy, std::string const &type_name, int elementsType,
                                                                 int rank, size_t *lengths, std::ptrdiff_t *strides, size_t size_of_ValueType) {

        cpp2py::pyref numpy_obj;

        if (X == NULL) return {NULL, "numpy interface : the python object is NULL !\n"};
        if (_import_array() != 0) return {NULL, "Internal Error in importing numpy\n"};

        if (!enforce_copy) {
          if (!numpy_convertible_to_view_impl(X, type_name, elementsType, rank))
            return {NULL, numpy_view_convertion_get_error_impl(X, type_name, elementsType, rank)};
          numpy_obj = cpp2py::borrowed(X);
        } else {
          // From X, we ask the numpy library to make a numpy, and of the correct type.
          // This handles automatically the cases where :
          //   - we have list, or list of list/tuple
          //   - the numpy type is not the one we want.
          //   - adjust the dimension if needed
          // If X is an array :
          //   - if Order is same, don't change it
          //   - else impose it (may provoque a copy).
          // if X is not array :
          //   - Order = FortranOrder or SameOrder - > Fortran order otherwise C

          //bool ForceCast = false;// Unless FORCECAST is present in flags, this call will generate an error if the data type cannot be safely obtained from the object.
          int flags = 0; //(ForceCast ? NPY_FORCECAST : 0) ;// do NOT force a copy | (make_copy ?  NPY_ENSURECOPY : 0);
                         //if (!(PyArray_Check(X) ))
          //flags |= ( IndexMapType::traversal_order == indexmaps::mem_layout::c_order(rank) ? NPY_C_CONTIGUOUS : NPY_F_CONTIGUOUS); //impose mem order
#ifdef PYTHON_NUMPY_VERSION_LT_17
          flags |= (NPY_C_CONTIGUOUS); //impose mem order
          flags |= (NPY_ENSURECOPY);
#else
          flags |= (NPY_ARRAY_C_CONTIGUOUS); // impose mem order
          flags |= (NPY_ARRAY_ENSURECOPY);
#endif
          numpy_obj = PyArray_FromAny(X, PyArray_DescrFromType(elementsType), rank, rank, flags, NULL); // new ref

          // do several checks
          if (!bool(numpy_obj)) { // The convertion of X to a numpy has failed !
            if (PyErr_Occurred()) {
              //PyErr_Print();
              PyErr_Clear();
            }
            return {NULL, "numpy interface : the python object  is not convertible to a numpy. "};
          }
          //assert (PyArray_Check(( PyObject *)numpy_obj)); assert((numpy_obj->ob_refcnt==1) || ((numpy_obj ==X)));

          PyArrayObject *arr_obj;
          arr_obj = (PyArrayObject *)((PyObject *)numpy_obj);
#ifdef PYTHON_NUMPY_VERSION_LT_17
          if (arr_obj->nd != rank) return {NULL, "numpy interface : internal error : dimensions do not match"};
          if (arr_obj->descr->type_num != elementsType)
            return {NULL,
                    "numpy interface : internal error : incorrect type of element :" + std::to_string(arr_obj->descr->type_num) + " vs "
                       + std::to_string(elementsType)};
#else
          if (PyArray_NDIM(arr_obj) != rank) return {NULL, "numpy interface : internal error : dimensions do not match"};
          if (PyArray_DESCR(arr_obj)->type_num != elementsType)
            return {NULL,
                    "numpy interface : internal error : incorrect type of element :" + std::to_string(PyArray_DESCR(arr_obj)->type_num) + " vs "
                       + std::to_string(elementsType)};
#endif
        }

        // extract strides and lengths
        PyArrayObject *arr_obj;
        arr_obj = (PyArrayObject *)((PyObject *)numpy_obj);
#ifdef PYTHON_NUMPY_VERSION_LT_17
        const size_t dim = arr_obj->nd; // we know that dim == rank
        for (size_t i = 0; i < dim; ++i) {
          lengths[i] = size_t(arr_obj->dimensions[i]);
          strides[i] = std::ptrdiff_t(arr_obj->strides[i]) / size_of_ValueType;
        }
#else
        const size_t dim = PyArray_NDIM(arr_obj); // we know that dim == rank
        for (size_t i = 0; i < dim; ++i) {
          lengths[i] = size_t(PyArray_DIMS(arr_obj)[i]);
          strides[i] = std::ptrdiff_t(PyArray_STRIDES(arr_obj)[i]) / size_of_ValueType;
        }
#endif

        return {numpy_obj, ""};
      }
    } // namespace numpy_interface
  }   // namespace arrays
} // namespace triqs
