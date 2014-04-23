
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

#ifndef TRIQS_EXCEPTIONS_H 
#define TRIQS_EXCEPTIONS_H

#include "./stack_trace.hpp"
#include <exception>
#include <string>
#include <sstream>

namespace triqs { 

 class exception : public std::exception {
  std::string acc, trace;
  public:
  exception() throw() :std::exception() { trace = utility::stack_trace();}
  virtual ~exception() throw() {}
  template<typename T> exception & operator  <<( T const & x) { std::stringstream f; f<<acc<<x; acc = f.str(); return *this;}
  exception & operator  <<( const char * mess ) { (*this) << std::string(mess); return *this;}// to limit code size
  virtual const char* what() const throw() { return acc.c_str();}
 };
 
 class runtime_error : public exception {
  public:
  runtime_error() throw() : exception() {}
  virtual ~runtime_error() throw() {}
  template<typename T> runtime_error & operator  <<( T && x) { exception::operator<<(x); return *this; }
 };

}

#define TRIQS_ERROR(CLASS,NAME) throw CLASS()<<" Triqs "<<NAME<<" at "<<__FILE__<< " : "<<__LINE__<<"\n\n"
#define TRIQS_RUNTIME_ERROR TRIQS_ERROR(triqs::runtime_error,"runtime error")

#endif

