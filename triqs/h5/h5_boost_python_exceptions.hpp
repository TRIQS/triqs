
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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

#ifndef TRIQS_H5_EXCEPTIONS_H 
#define TRIQS_H5_EXCEPTIONS_H

#include <Python.h>
#include <boost/python.hpp>
#include <H5Cpp.h>
#include <sstream>

namespace triqs { namespace utility {

 // transcription of H5 error
 inline void translatorH5Error(H5::Exception const& x) {
  std::stringstream f; f<< " H5 exception\n"<<x.getCDetailMsg()<<"\n in function "<< x.getCFuncName()<<"\n";
  PyErr_SetString(PyExc_RuntimeError, f.str().c_str() ); 
 };

 void register_h5_exception() { boost::python::register_exception_translator<H5::Exception>(translatorH5Error);}

}}

#endif

