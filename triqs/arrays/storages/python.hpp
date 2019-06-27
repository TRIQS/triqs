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

#include <Python.h>
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

    // We create a new shared handle -> increase refcount
    Py_INCREF(obj);

    PyArrayObject *arr = (PyArrayObject *)(obj);

    handle<T, 'S'> r{
       (T *)PyArray_DATA(arr),    // r._data
       size_t(PyArray_SIZE(arr)), // r._size
       obj,                       // r._foreign_handle
       (void *)&py_decref         // r._foreign_decref
    };
    return r;
  }

  // ------------------  delete_pycapsule  ----------------------------------------------------

  // Properly delete the handle<T, 'S'> in a PyCapsule
  template <typename T> static void delete_pycapsule(PyObject *capsule) {
    handle<T, 'S'> *handle = static_cast<nda::mem::handle<T, 'S'> *>(PyCapsule_GetPointer(capsule, "guard"));
    //std::cerr << "decapsulate : "<< handle->id << "  "<< handle->data << "  nrefs" << handle->nref() << "\n";
    delete handle;
  }

  // ------------------  make_pycapsule,   ----------------------------------------------------

  // Make a pycapsule out of the shared handle to return to Python
  template <typename T> PyObject *make_pycapsule(handle<T, 'R'>  const &h) {
    void *keep = new handle<T, 'S'>{h}; // a new reference
    return PyCapsule_New(keep, "guard", &delete_pycapsule<T>);
  }

  template <typename T> PyObject *make_pycapsule(handle<T, 'B'> const &h) {
    if (h.parent() == nullptr) throw std::runtime_error("Can not return to python a view on something else than an nda::array");
    void *keep = new handle<T, 'S'>{*h.parent()}; // a new reference
    return PyCapsule_New(keep, "guard", &delete_pycapsule<T>);
  }

} // namespace nda::mem
