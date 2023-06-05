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
#include <functional>
#include <memory>
#include <map>

namespace triqs {
  namespace mc_tools {

    // mini concept checking : does T have void operator()() ?
    template <typename T, typename Enable = void> struct is_callable : std::false_type {};
    template <typename T> struct is_callable<T, decltype(std::declval<T>()())> : std::true_type {};

    //--------------------------------------------------------------------

    class measure_aux {

      std::shared_ptr<void> impl_;
      std::function<void()> call_;

      public:
      template <typename MeasureAuxType> measure_aux(std::shared_ptr<MeasureAuxType> p) {
        static_assert(is_callable<MeasureAuxType>::value, "This measure_aux is not callable");
        impl_ = p;
        call_ = [p]() mutable { (*p)(); }; // without the mutable, the operator () of the lambda is const, hence p ...
      }

      void operator()() { call_(); }
    };

  } // namespace mc_tools
} // namespace triqs
