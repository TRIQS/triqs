// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once

#include "triqs/utility/macros.hpp"
#include "triqs/utility/exceptions.hpp"

#include <cpp2py/pyref.hpp>

#include <Python.h>
#include <numpy/arrayobject.h>

namespace triqs {
  namespace arrays {
    namespace numpy_interface {

      using utility::mini_vector;

      inline std::string object_to_string(PyObject *p) {
        return "";
        // FIXME
        // if (!PyString_Check(p)) TRIQS_RUNTIME_ERROR<<" Internal error, expected a python string .....";
        // return PyString_AsString(p);
      }

      template <class T> struct numpy_to_C_type;
      template <class T> struct numpy_to_C_type<T const> : numpy_to_C_type<T> {};
#define CONVERT(C, P)                                                                                                                                \
  template <> struct numpy_to_C_type<C> {                                                                                                            \
    enum { arraytype = P };                                                                                                                          \
    static const char *name() { return AS_STRING(C); }                                                                                               \
  }
      CONVERT(bool, NPY_BOOL);
      CONVERT(char, NPY_STRING);
      CONVERT(signed char, NPY_BYTE);
      CONVERT(unsigned char, NPY_UBYTE);
      CONVERT(short, NPY_SHORT);
      CONVERT(unsigned short, NPY_USHORT);
      CONVERT(int, NPY_INT);
      CONVERT(unsigned int, NPY_UINT);
      CONVERT(long, NPY_LONG);
      CONVERT(unsigned long, NPY_ULONG);
      CONVERT(long long, NPY_LONGLONG);
      CONVERT(unsigned long long, NPY_ULONGLONG);
      CONVERT(float, NPY_FLOAT);
      CONVERT(double, NPY_DOUBLE);
      CONVERT(long double, NPY_LONGDOUBLE);
      CONVERT(std::complex<float>, NPY_CFLOAT);
      CONVERT(std::complex<double>, NPY_CDOUBLE);
      CONVERT(std::complex<long double>, NPY_CLONGDOUBLE);
#undef CONVERT

      // ----------------------   Impl. functions

      // Return X is an array of given rank and elementtype
      bool numpy_convertible_to_view_impl(PyObject *X, std::string const &type_name, int elementsType, int rank);

      // Extracts
      std::pair<cpp2py::pyref, std::string> numpy_extractor_impl(PyObject *X, bool enforce_copy, std::string const &type_name, int elementsType,
                                                                 int rank, size_t *lengths, std::ptrdiff_t *strides, size_t size_of_ValueType);

      // ---------------------

      template <typename T, int Rank> struct numpy_extractor {
        using numpy_t = numpy_to_C_type<T>;

        numpy_extractor() = default;

        // is X convertible to array_view<T, Rank> ?
        bool is_convertible_to_view(PyObject *X) const { return numpy_convertible_to_view_impl(X, numpy_t::name(), numpy_t::arraytype, Rank); }

        // true if ok
        bool extract(PyObject *X, bool enforce_copy) {
          std::tie(numpy_obj, error) =
             numpy_extractor_impl(X, enforce_copy, numpy_t::name(), numpy_t::arraytype, Rank, &lengths[0], &strides[0], sizeof(T));
          return bool(numpy_obj);
        }

        cpp2py::pyref numpy_obj;
        std::string error = " ";
        mini_vector<size_t, Rank> lengths;
        mini_vector<std::ptrdiff_t, Rank> strides;
      };

    } // namespace numpy_interface
  }   // namespace arrays
} // namespace triqs
