#pragma once
#include "utils.hpp"
namespace triqs::mesh {

  // -------------------------------------------------------------------------------------------
  //                             unary expressions
  // -------------------------------------------------------------------------------------------

  template <char OP, typename L> struct k_expr_unary {
    static_assert(OP == '-', "Internal error");
    L l;
    uint64_t mesh_hash = l.mesh_hash();

    auto value() const { return -l.value(); }
    auto index() const { return -l.index(); }
  };

  // -------------------------------------------------------------------------------------------
  //                             binary expressions
  // -------------------------------------------------------------------------------------------
  // OP : '+', '-', ...
  // L is an index or an int.
  template <char OP, typename L, typename R> struct k_expr {
    L l;
    R r;

    template <typename L1, typename R1> k_expr(L1 &&l1, R1 &&r1) : l{std::forward<L1>(l1)}, r{std::forward<R1>(r1)} {}

    /// The value of the k-expression
    [[nodiscard]] auto value() const {
      if constexpr (OP == '+')
        return make_regular(l.value() + r.value());
      else if constexpr (OP == '-')
        return make_regular(l.value() - r.value());
      else
        return make_regular(l * r.value()); // last case : OP="*"
    }

    /// The value of the index
    [[nodiscard]] auto index() const {
      // check the mesh hash if we have 2 meshes.
      if constexpr (requires { l.mesh_hash(); }) EXPECTS(l.mesh_hash() == r.mesh_hash());
      if constexpr (OP == '+')
        return l.index() + r.index();
      else if constexpr (OP == '-')
        return l.index() - r.index();
      else
        return l * r.index(); // last case : OP="*"
    }

    /// The Hash for the associated mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const { return r.mesh_hash(); };
  };

  // -- Trait --

  template <typename T> constexpr bool is_k_expr                                        = false;
  template <char OP, typename L, typename R> constexpr bool is_k_expr<k_expr<OP, L, R>> = true;
  template <char OP, typename L> constexpr bool is_k_expr<k_expr_unary<OP, L>>          = true;

} // namespace triqs::mesh
