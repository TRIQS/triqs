/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <triqs/utility/first_include.hpp>
#ifdef TRIQS_WITH_PYTHON_SUPPORT
#define TRIQS_LIB_CPP
#include "./py_stream.hpp"
#include "../utility/exceptions.hpp"

#pragma clang diagnostic ignored "-Wdeprecated-writable-strings"
#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace triqs {

using py_tools::pyref;

py_stream::py_stream() {
 if (!Py_IsInitialized()) TRIQS_RUNTIME_ERROR << "Construction of a py_stream before the interpreter is initialized !!";
 sys = pyref::module("sys");
 sys_out = sys.attr("stdout");
}

void py_stream::_write(const char* s) { pyref res = PyObject_CallMethod(sys_out, "write", "s", s); }
}

#endif

