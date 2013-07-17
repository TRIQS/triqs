/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by  O. Parcollet
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
#ifndef TRIQS_PYTHON_EXTRACTOR_H
#define TRIQS_PYTHON_EXTRACTOR_H
#include <triqs/utility/first_include.hpp>

namespace triqs { namespace utility { namespace python { 

 template<typename T> 
 class extractor { 
  T const * _c;
  public : 
  extractor (PyObject * x)  {
   if (!PyCObject_Check(x)) TRIQS_RUNTIME_ERROR<< " Internal error : extractor contructed from a wrong type !!!!!";
    _c = static_cast<T*>(PyCObject_AsVoidPtr(x));
  }
  T const & operator() () const { return *_c;}
 };

}}}
#endif
