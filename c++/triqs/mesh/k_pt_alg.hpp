#pragma once

namespace triqs::mesh {

  // -------------------------------------------------------------------------------------------
  //                             unary expressions
  // -------------------------------------------------------------------------------------------

  template <char OP, typename L> struct k_expr_unary {
    static_assert(OP == '-', "Internal error");
    L l;
    uint64_t mesh_hash = l.mesh_hash;

    auto value() const { return -l.value(); }
    auto get_idx() const { return -l.idx; }
  };

  // -------------------------------------------------------------------------------------------
  //                             binary expressions
  // -------------------------------------------------------------------------------------------
  // OP : '+', '-', ...
  // L is an index or an int.
  template <char OP, typename L, typename R> struct k_expr {
    L l;
    R r;
    uint64_t mesh_hash = r.mesh_hash;

    template <typename L1, typename R1> k_expr(L1 &&l1, R1 &&r1) : l{std::forward<L1>(l1)}, r{std::forward<R1>(r1)} {}

    auto value() const {
      if constexpr (OP == '+')
        return l.value() + r.value();
      else if constexpr (OP == '-')
        return l.value() - r.value();
      else
        return l * r.value(); // last case : OP="*"
    }

    auto get_idx() const {
#ifdef TRIQS_DEBUG
      // check the mesh hash if we have 2 meshes.
      if constexpr (requires { l.mesh_hash; }) {
	// FIXME : EXPECT Or throw ? 
	//EXPECTS(l.mesh_hash == r.mesh_hash);
        if (l.mesh_hash != r.mesh_hash) throw std::runtime_error("Mesh mismatch in brzone mesh_point arithmetic");
      }
#endif
      if constexpr (OP == '+')
        return l.get_idx() + r.get_idx();
      else if constexpr (OP == '-')
        return l.get_idx() - r.get_idx();
      else
        return l * r.get_idx(); // last case : OP="*"
    }
  };

  // -------------------------------------------------------------------------------------------
  //                                 Operator overload
  // -------------------------------------------------------------------------------------------

  template <typename T> constexpr bool is_bz_k_expr                                        = false;
  template <char OP, typename L, typename R> constexpr bool is_bz_k_expr<k_expr<OP, L, R>> = true;
  template <char OP, typename L> constexpr bool is_bz_k_expr<k_expr_unary<OP, L>>          = true;

  template <typename T>
  concept BzMeshPoint = is_bz_k_expr<std::decay_t<T>>;

  template <BzMeshPoint A> k_expr_unary<'-', A> operator-(A &&a) { return {std::forward<A>(a)}; }

  template <BzMeshPoint L, BzMeshPoint R> k_expr<'+', L, R> operator+(L &&l, R &&r) { return {std::forward<L>(l), std::forward<R>(r)}; }

  template <BzMeshPoint L, BzMeshPoint R> k_expr<'-', L, R> operator-(L &&l, R &&r) { return {std::forward<L>(l), std::forward<R>(r)}; }

  template <BzMeshPoint R> k_expr<'*', long, R> operator*(long l, R &&r) { return {l, std::forward<R>(r)}; }

} // namespace triqs::mesh
