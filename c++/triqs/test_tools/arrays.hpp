// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief GoogleTest helper macros and an HDF5 round-trip utility shared by the TRIQS C++ test suite.
 */

#pragma once

#ifndef TRIQS_DEBUG
#define TRIQS_DEBUG
#endif

#include "../arrays.hpp"

#include <gtest/gtest.h>
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>
#include <nda/nda.hpp>
#include <nda/gtest_tools.hpp>

#include <sstream>
#include <string>

using dcomplex = std::complex<double>;
using triqs::clef::placeholder;

/**
 * @addtogroup triqs-test-tools
 * @{
 */

/**
 * @brief Check that streaming `Y` into an output stream produces the string `X`.
 *
 * @details Streams `Y` into a `std::stringstream` and compares the result to `X` with `EXPECT_EQ`, i.e. a non-fatal
 * GoogleTest assertion that lets the test continue on failure.
 *
 * @param X Expected string.
 * @param Y Object to be streamed via `operator<<`.
 */
#define EXPECT_PRINT(X, Y)                                                                                                                           \
  {                                                                                                                                                  \
    std::stringstream ss;                                                                                                                            \
    ss << (Y);                                                                                                                                       \
    EXPECT_EQ(X, ss.str());                                                                                                                          \
  }

/**
 * @brief Check that streaming `Y` into an output stream produces the string `X`.
 *
 * @details Like @ref EXPECT_PRINT but uses `ASSERT_EQ`, i.e. a fatal GoogleTest assertion that aborts the current test
 * on failure.
 *
 * @param X Expected string.
 * @param Y Object to be streamed via `operator<<`.
 */
#define ASSERT_PRINT(X, Y)                                                                                                                           \
  {                                                                                                                                                  \
    std::stringstream ss;                                                                                                                            \
    ss << (Y);                                                                                                                                       \
    ASSERT_EQ(X, ss.str());                                                                                                                          \
  }

/**
 * @brief Define a `main` function that runs all registered GoogleTest tests.
 *
 * @details Initializes GoogleTest and runs all tests. When TRIQS is built with MPI support, an `mpi::environment` is
 * set up so that MPI-aware tests run under a valid communicator.
 */
#define MAKE_MAIN                                                                                                                                    \
  int main(int argc, char **argv) {                                                                                                                  \
    ::testing::InitGoogleTest(&argc, argv);                                                                                                          \
    if (mpi::has_env) {                                                                                                                              \
      mpi::environment env(argc, argv);                                                                                                              \
      std::cout << "MPI environment detected\n";                                                                                                     \
      return RUN_ALL_TESTS();                                                                                                                        \
    } else                                                                                                                                           \
      return RUN_ALL_TESTS();                                                                                                                        \
  }

/**
 * @brief Write an object to an HDF5 file and read it back.
 *
 * @details Serializes `x` to an HDF5 file, reads it back into a freshly default-constructed object and returns it.
 * Useful for testing that a type's HDF5 read/write round-trip preserves its value.
 *
 * @tparam T Type of the object. Must be default-constructible and h5-serializable.
 * @param x Object to write to file.
 * @param filename Base name of the HDF5 file (the `.h5` extension is appended).
 * @param name Name of the dataset inside the file.
 * @return The object read back from the file.
 */
template <typename T> T rw_h5(T const &x, std::string filename = "ess", std::string name = "x") {
  T y; // must be default constructible
  {
    h5::file file(filename + ".h5", 'w');
    h5_write(file, name, x);
  }
  {
    h5::file file(filename + ".h5", 'r');
    h5_read(file, name, y);
  }
  return y;
}

/** @} */
