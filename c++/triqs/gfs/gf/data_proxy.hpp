// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
#include <utility>
#include <triqs/arrays.hpp>
#include <triqs/utility/view_tools.hpp>

namespace triqs {
  namespace gfs {

    namespace details {
      template <typename Target> struct _data_proxy;

      //---------------------------- tensor valued ---------------------------------
      template <int R> struct _data_proxy<tensor_valued<R>> {

        template <typename S, typename X> FORCEINLINE static auto invoke(S &data, X const &x) { return arrays::make_array_proxy(data, x); }

        // one var
        //template <typename S> static auto invoke(S& data, long i) { return arrays::make_array_proxy(data, i); }

        // multi var
        //template <typename S, typename... Int> static auto invoke(S& data, std::tuple<Int...> const& i) {
        // return arrays::make_array_proxy(data, i);
        //}
      };

      //---------------------------- matrix valued ---------------------------------
      template <> struct _data_proxy<matrix_valued> {

        template <typename S, typename X> FORCEINLINE static auto invoke(S &data, X const &x) { return arrays::make_matrix_proxy(data, x); }

        // one var
        //template <typename S> static auto invoke(S& data, long i) { return arrays::make_matrix_proxy(data, i); }

        // multi var
        //template <typename S, typename... Int> static auto invoke(S& data, std::tuple<Int...> const& i) {
        // return arrays::make_matrix_proxy(data, i);
        //}
      };
      //---------------------------- scalar valued ---------------------------------
      template <> struct _data_proxy<scalar_valued> {

        // one var
        template <typename S> static decltype(auto) invoke(S &data, long i) { return data(i); }

        // FIXME : WHY is this necessary in this case ? can we simplify ?
        // multi var
        // using the standard technique from tuple::apply with a sequence
        template <typename S, typename Tu, size_t... Is> static decltype(auto) _impl(S &data, Tu const &tu, std::index_sequence<Is...>) {
          return data(std::get<Is>(tu)...);
        }
        template <typename S, typename... Int> static decltype(auto) invoke(S &data, std::tuple<Int...> const &tu) {
          return _impl(data, tu, std::index_sequence_for<Int...>{});
        }
      };

      template <int R> struct _data_proxy<tensor_real_valued<R>> : _data_proxy<tensor_valued<R>> {};
      template <> struct _data_proxy<matrix_real_valued> : _data_proxy<matrix_valued> {};
      template <> struct _data_proxy<scalar_real_valued> : _data_proxy<scalar_valued> {};
    } // namespace details
  }   // namespace gfs
} // namespace triqs
