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
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once
#include <triqs/utility/first_include.hpp>
#include <h5/h5.hpp>
#include <string>
namespace triqs {
  namespace mc_tools {

    // mini concept checking
    template <typename MCSignType, typename T, typename = MCSignType> struct has_attempt : std::false_type {};
    template <typename MCSignType, typename T>
    struct has_attempt<MCSignType, T, decltype(MCSignType(std::declval<T>().attempt()))> : std::true_type {};

    template <typename MCSignType, typename T, typename = MCSignType> struct has_accept : std::false_type {};
    template <typename MCSignType, typename T> struct has_accept<MCSignType, T, decltype(std::declval<T>().accept())> : std::true_type {};

    template <typename T, typename = void> struct has_reject : std::false_type {};
    template <typename T> struct has_reject<T, decltype(std::declval<T>().reject())> : std::true_type {};

    template <typename MCSignType, typename T, typename = void> struct has_accumulate : std::false_type {};
    template <typename MCSignType, typename T>
    struct has_accumulate<MCSignType, T, decltype(std::declval<T>().accumulate(MCSignType()))> : std::true_type {};

    template <typename T, typename = void> struct has_collect_result : std::false_type {};
    template <typename T>
    struct has_collect_result<T, decltype(std::declval<T>().collect_results(std::declval<mpi::communicator>()))> : std::true_type {};

    // ----------------- h5 detection -----------------------
    using h5_rw_lambda_t = std::function<void(h5::group, std::string const &)>;

#ifdef TRIQS_HAS_CONCEPTS
    template <typename T> concept bool H5ReadWritable = requires(T x) {
      {h5_read(h5::group{},std::string{}, (T&)x
      }
      ->void;
      {h5_write(h5::group{},std::string{}, (T const&)x
      }
      ->void;
    };

    h5_rw_lambda_t make_h5_read(H5ReadWritable *p) {
      return [p](h5::group F, std::string const &Name) { h5_read(F, Name, *p) };
    }
    h5_rw_lambda_t make_h5_read(auto *p) { // generic case
      return [p](h5::group F, std::string const &Name) {};
    }
    h5_rw_lambda_t make_h5_write(H5ReadWritable *p) {
      return [p](h5::group F, std::string const &Name) { h5_write(F, Name, *p) };
    }
    h5_rw_lambda_t make_h5_write(auto *p) { // generic case
      return [p](h5::group F, std::string const &Name) {};
    }

#else

    // Workaround lack of concepts implementation
    template <typename T, typename = void> struct make_h5_impl {
      static h5_rw_lambda_t read(T *p) { return {}; }
      static h5_rw_lambda_t write(T const *p) { return {}; }
    };

    template <typename T> struct make_h5_impl<T, decltype(h5_read(std::declval<h5::group>(), std::string(), std::declval<T &>()))> {
      static h5_rw_lambda_t read(T *p) {
        return [p](h5::group F, std::string const &Name) { h5_read(F, Name, *p); };
      }
      static h5_rw_lambda_t write(T const *p) {
        return [p](h5::group F, std::string const &Name) { h5_write(F, Name, *p); };
      }
    };
    template <typename T> h5_rw_lambda_t make_h5_read(T *p) { return make_h5_impl<T>::read(p); }
    template <typename T> h5_rw_lambda_t make_h5_write(T const *p) { return make_h5_impl<T>::write(p); }

#endif

    // ----------------- collect_statistics detection -----------------------
    using collect_statistics_rw_lambda_t = std::function<void(mpi::communicator)>;

#ifdef TRIQS_HAS_CONCEPTS
    template <typename T> concept bool HasCollectResults = requires(T x) {
      { x.collect_statistics(mpi::communicator{})->void; };

      std::function<void(mpi::communicator)> make_collect_statistics(HasCollectResults * p) {
        return [p](mpi::communicator c) { p->collect_statistics(c); };
      }
      std::function<void(mpi::communicator)> make_collect_statistics(auto *p) { // generic case
        return {};
      }

#else

    template <typename T, typename = void> struct synth_collect_statistics {
      static std::function<void(mpi::communicator)> invoke(T *p) { return {}; }
    };
    template <typename T> struct synth_collect_statistics<T, decltype(std::declval<T>().collect_statistics(mpi::communicator()))> {
      static std::function<void(mpi::communicator)> invoke(T *p) {
        return [p](mpi::communicator c) { p->collect_statistics(c); };
      }
    };
    template <typename T> std::function<void(mpi::communicator)> make_collect_statistics(T *p) { return synth_collect_statistics<T>::invoke(p); }

#endif

      // move_construtible is not in gcc 4.6 std lib
      //template <class T> struct is_move_constructible : std::is_constructible<T, typename std::add_rvalue_reference<T>::type> {};
    } // namespace mc_tools
  }   // namespace mc_tools
