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
#include <triqs/utility/first_include.hpp>
#include "./typeid_name.hpp"
#include <sstream>

#ifdef __GNUC__
#include <cxxabi.h> 
#endif

#include <boost/algorithm/string/erase.hpp>

namespace triqs { namespace utility { 

 std::string demangle(const char * name) { 
  std::stringstream fs;
#ifdef __GNUC__
  int status;
  char * demangled = abi::__cxa_demangle(name, NULL, NULL, &status);
  if (!status) { 
   std::string res(demangled);
   boost::erase_all(res,", boost::tuples::null_type");   
   boost::erase_all(res,", -1");   
   boost::erase_all(res,", void");   
   fs<< res; 
   free(demangled); 
  } else fs<< name; 
  //fs<<abi::__cxa_demangle(typeid(A).name(), 0, 0, &status);
#else 
  fs<<name;
#endif
  return fs.str();
 }

 std::string demangle(std::string const & name) { return demangle (name.c_str());}

 std::string get_name(std::type_info const &info) { return demangle(info.name());}

}}
