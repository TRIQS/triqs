
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

#ifndef TRIQS_ARRAYS_EXCEPTIONS_H 
#define TRIQS_ARRAYS_EXCEPTIONS_H

#include <triqs/utility/stack_trace.hpp>
#include <exception>
#include <string>
#include <sstream>

namespace triqs { namespace arrays {  

 class key_error : public std::exception {
  std::string acc;
  public:
  key_error() throw() :std::exception() {}
  virtual ~key_error() throw() {}
  template<typename T> key_error & operator  <<( T const & x) { std::stringstream f; f<<acc<<x; acc = f.str(); return *this;}
  virtual const char* what() const throw() { return acc.c_str();}
 };

}}

#define TRIQS_ARRAYS_KEY_ERROR throw triqs::arrays::key_error()<<"triqs::arrays key_error at "<<__FILE__<< " : "<<__LINE__<<"\n\n Trace is :\n\n"<<triqs::utility::stack_trace()<<"\n"

#endif


