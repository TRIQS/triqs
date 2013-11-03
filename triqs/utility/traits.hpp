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
#pragma once

namespace triqs { 

 // MPL traits
 template<typename ... T> struct _or; 
 template<typename T0, typename ... T> struct _or<T0,T...> : std::integral_constant<bool,T0::value || _or<T...>::value>{}; 
 template<> struct _or<> : std::false_type{};

 template<typename ... T> struct _and; 
 template<typename T0, typename ... T> struct _and<T0,T...> : std::integral_constant<bool,T0::value && _and<T...>::value>{}; 
 template<> struct _and<> : std::true_type{};

 // helpers
 template<bool B> using bool_constant = std::integral_constant<bool,B>;

}

