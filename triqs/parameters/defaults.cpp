/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by H. Hafermann, O. Parcollet
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
#include "./defaults.hpp"
#include <triqs/utility/formatted_output.hpp>
namespace triqs { namespace utility {
   
 std::vector<std::vector<std::string>> parameter_defaults:: generate_help() const{
     std::vector<std::vector<std::string>> str;
     str.push_back({"parameter:", "status:", "type",  "default value:", "description:"});
     for (auto const &s : object_map){
       std::string key=s.first; std::ostringstream val; val << s.second;
       if(is_required(key)) str.push_back({key, "required", s.second.type_name(), "-", doc(key)});
       else str.push_back({key, "optional", s.second.type_name(), val.str(), doc(key)});
     }
     return str; 
   }

   std::ostream & operator << (std::ostream & out, parameter_defaults const & p) {
    out<< print_formatted(p.generate_help());
    return out;
   }

}}
