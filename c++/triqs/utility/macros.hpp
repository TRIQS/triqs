// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell
#pragma once

#include <triqs/utility/first_include.hpp>
#include <triqs/utility/stack_trace.hpp>
#include <boost/utility/enable_if.hpp>
#include <type_traits>
#include <iostream>

#include <nda/macros.hpp>


#define DECL_AND_RETURN(...)                                                                                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

#define TRIQS_CATCH_AND_ABORT                                                                                                                        \
  catch (std::exception const &e) {                                                                                                                  \
    std::cout << e.what() << std::endl;                                                                                                              \
    return 1;                                                                                                                                        \
  }

namespace triqs {
  template <typename T> struct remove_cv_ref : std::remove_cv<typename std::remove_reference<T>::type> {};

  /// Tag the views
  struct is_view_tag {};
  template <typename T> struct is_view : std::is_base_of<is_view_tag, T> {};

} // namespace triqs

#define TYPE_ENABLE_IF(Type, ...) typename boost::enable_if<__VA_ARGS__, Type>::type
#define TYPE_ENABLE_IFC(Type, ...) typename boost::enable_if_c<__VA_ARGS__, Type>::type
#define TYPE_DISABLE_IF(Type, ...) typename boost::disable_if<__VA_ARGS__, Type>::type
#define TYPE_DISABLE_IFC(Type, ...) typename boost::disable_if_c<__VA_ARGS__, Type>::type

#define TRIQS_DEPRECATED(Message) __attribute__((deprecated(AS_STRING(Message))))
#define TRIQS_PRINT(X) std::cerr << AS_STRING(X) << " = " << X << "      at " << __FILE__ << ":" << __LINE__ << '\n'

#define FORCEINLINE __inline__ __attribute__((always_inline))

#define TERMINATE(X)                                                                                                                                 \
  std::cerr << "Terminating at " << __FILE__ << ":" << __LINE__ << "\n";                                                                             \
  std::cerr << X;                                                                                                                                    \
  std::terminate();                                                                                                                                  \
  }

