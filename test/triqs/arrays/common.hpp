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
#ifndef COMMON_TEST_ARRAY_H
#define COMMON_TEST_ARRAY_H
#include <triqs/arrays.hpp>
#include <triqs/arrays/asserts.hpp>
#include <sstream>
#include <iostream>

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl;
#define TEST_ERR(X) std::cerr << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl;

#define TEST_ASSERT(X)                                                                                                                               \
  if (!bool(X)) {                                                                                                                                    \
    std::stringstream fs;                                                                                                                            \
    fs << "Failed Assertion line " << __LINE__ << " of file " << __FILE__ << " :\n" << BOOST_PP_STRINGIZE((X));                                      \
    std::cout << fs.str() << std::endl;                                                                                                              \
    TRIQS_RUNTIME_ERROR << fs.str();                                                                                                                 \
  }

#define AS_STRING(X) AS_STRING2(X)
#define AS_STRING2(X) #X

#endif
