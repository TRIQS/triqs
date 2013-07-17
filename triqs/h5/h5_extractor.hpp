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
#ifndef TRIQS_H5_H5_EXTRACTOR_GROUP_H
#define TRIQS_H5_H5_EXTRACTOR_GROUP_H
#include "./group.hpp"
#include <triqs/utility/view_tools.hpp>

namespace triqs { namespace h5 {
 
 /** 
  * \brief Extractor for cython usage only
  */
 template<typename T> 
  struct h5_extractor { 
   typename view_type_if_exists_else_type<T>::type operator() (group fg, std::string const & subgroup_name) const { 
    typename non_view_type_if_exists_else_type<T>::type r;
    h5_read(fg,subgroup_name,r);
    return r;
   }
  };

}}
#endif
