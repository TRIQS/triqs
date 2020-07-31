// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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

#ifndef TRIQS_ARRAYS_MAKE_IMMUTABLE_ARRAY_H
#define TRIQS_ARRAYS_MAKE_IMMUTABLE_ARRAY_H
#include "./array.hpp"
#include "../utility/tuple_tools.hpp"

namespace triqs {
  namespace arrays {

    template <typename Expr, int... ph> class immutable_array_expr_impl : TRIQS_CONCEPT_TAG_NAME(ImmutableArray) {
      template <int I> struct _si { typedef size_t type; };

      public:
      immutable_array_expr_impl(Expr e_, clef::pair<ph, range>... p)
         : f(clef::make_function(e_, clef::placeholder<ph>()...)), dom_(make_shape(p.rhs.size()...)){};
      typedef typename triqs::clef::result_of::make_function<Expr, clef::placeholder<ph>...>::type function_type;
      typedef typename std::result_of<function_type(typename _si<ph>::type...)>::type value_type;
      typedef indexmaps::cuboid::domain_t<sizeof...(ph)> domain_type;
      domain_type domain() const { return dom_; }
      template <typename... Args> value_type operator()(Args const &... args) const { return f(args...); }
      friend std::ostream &operator<<(std::ostream &out, immutable_array_expr_impl const &x) {
        return out << " immutable_array on domain : " << x.domain();
      }

      protected:
      function_type f;
      domain_type dom_;
    };

    /**
  * \brief Makes a lazy immutable (cuboid) array from a simple expression and a set of range...
  * \param expr The lazy expression
  * \param i_=R i_ is a placeholder, R a range. The i_=R produce a pair of i_ and R , which is the parameter.
  * \return A lazy object implementing the ImmutableArray concept with the domain built from the ranges.
  */
    template <typename Expr, int... ph> immutable_array_expr_impl<Expr, ph...> make_immutable_array(Expr const &expr, clef::pair<ph, range>... p) {
      return immutable_array_expr_impl<Expr, ph...>(expr, p...);
    }

    // if ones prefers to use a function...

    template <typename Function, typename... Ranges> class immutable_array_fun_impl : TRIQS_CONCEPT_TAG_NAME(ImmutableArray) {
      template <int I> struct _si { typedef size_t type; };

      public:
      immutable_array_fun_impl(Function f, Ranges... r) : f_(std::move(f)), dom_(make_shape(r.size()...)){};
      //typedef typename std::function<Function(typename Ranges::index_t...)>::result_type value_type;
      typedef decltype(triqs::tuple::apply(std::declval<Function>(), std::make_tuple(typename Ranges::index_t()...))) value_type;
      typedef indexmaps::cuboid::domain_t<sizeof...(Ranges)> domain_type;
      domain_type domain() const { return dom_; }
      template <typename... Args> value_type operator()(Args const &... args) const { return f_(args...); }
      friend std::ostream &operator<<(std::ostream &out, immutable_array_fun_impl const &x) {
        return out << " immutable_array on domain : " << x.domain();
      }

      protected:
      Function f_;
      domain_type dom_;
    };

    template <typename Function, typename... Ranges> immutable_array_fun_impl<Function, Ranges...> make_immutable_array(Function f, Ranges... r) {
      return immutable_array_fun_impl<Function, Ranges...>(std::move(f), r...);
    }

  } // namespace arrays
} // namespace triqs
#endif
