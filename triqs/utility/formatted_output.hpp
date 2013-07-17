/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by H. Hafermann
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
#ifndef TRIQS_FORMATTED_OUTPUT_H
#define TRIQS_FORMATTED_OUTPUT_H

#include<string>
#include<vector>
#include<iostream>
#include<iomanip>
#include<sstream>

namespace triqs { namespace utility { 

 inline std::string print_formatted(std::vector<std::vector<std::string>> const &out){
   std::vector<std::size_t> max_len;
   std::ostringstream str;

   for(auto const &s : out){
       max_len.resize(out[0].size(),0);
       for(std::size_t i=0; i<s.size(); ++i)
         if(s[i].length()>max_len[i]) max_len[i]=s[i].length();
   }
   for(auto const &s : out){
     for(std::size_t i=0; i<s.size(); ++i)
       str << std::left << std::setw(max_len[i]+2) << s[i];
     str << std::endl;
   }

   return str.str();
 }
 
}}
#endif



