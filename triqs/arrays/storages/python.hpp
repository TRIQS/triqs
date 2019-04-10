/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet
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

// SHOULD ONLY BE INCLUDED in a python module.
#ifndef PYTHON_NUMPY_VERSION_LT_17
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include "Python.h"
#include <numpy/arrayobject.h>
#include <cpp2py/pyref.hpp>


namespace nda::mem {

  // -------------  make_handle ------------

  static void py_decref(void *x) { Py_DECREF((PyObject *)x); }

  // Take a handle on a numpy. numpy is a borrowed Python ref.
  // implemented only in Python module, not in triqs cpp
  template <typename T> handle<T, 'S'> make_handle(PyObject *obj) {

    _import_array();

    if (obj == NULL) throw std::runtime_error(" Can not build an mem_blk_handle from a NULL PyObject *");
    if (!PyArray_Check(obj)) throw std::runtime_error("Internal error : ref_counter construct from pyo : obj is not an array");
    Py_INCREF(obj); // assume borrowed
    PyArrayObject *arr = (PyArrayObject *)(obj);

    handle<T, 'S'> r; // empty
    r.data        = (T *)PyArray_DATA(arr);
    r.size        = size_t(PyArray_SIZE(arr));
    r.id          = globals::rtable.get();
    r.sptr        = obj;
    r.release_fnt = (void*)&py_decref;
    return r;
  }

  // ------------------  make_pycapsule  ----------------------------------------------------
  // make a pycapsule out of the shared handle to return to Python

  template <typename T> static void delete_pycapsule(PyObject *capsule) {
    handle<T, 'S'> *handle = static_cast<nda::mem::handle<T, 'S'> *>(PyCapsule_GetPointer(capsule, "guard"));
    //std::cerr << "decapsulate : "<< handle->id << "  "<< handle->data << "  nrefs" << handle->nref() << "\n";
    delete handle;
  }

  template <typename T> PyObject *make_pycapsule(handle<T, 'S'> h) {
    //std::cerr << "capsulate : "<< h.id << "  "<<h.data <<  "nrefs" << h.nref() << "\n";
    void *keep = new handle<T, 'S'>{std::move(h)}; // a new reference
    return PyCapsule_New(keep, "guard", &delete_pycapsule<T>);
  }

  template <typename T> PyObject *make_pycapsule(handle<T, 'R'> const &h) { return make_pycapsule(handle<T, 'S'>{h}); }

  template <typename T> PyObject *make_pycapsule(handle<T, 'B'> const &h) = delete; // Can not return a borrowed view to Python

} // namespace nda::mem
