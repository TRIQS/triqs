
namespace triqs::gfs {
  /**
   * Makes a copy of the array in matrix, whose first dimension is the n-th dimension of a
   * and the second dimension are the flattening of the other dimensions, in the original order
   *
   * @param a : array
   * @param n : the dimension to preserve.
   *
   * @return : a matrix, copy of the data
   * */
  template <typename T, int R> array<T, 2> flatten_2d(array_const_view<T, R> a, int n) {

    a.rebind(rotate_index_view(a, n));    // Swap relevant dim to front. The view is passed by value, we modify it.
    long nrows = first_dim(a);            // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols = a.size() / nrows;        // # columns of the result. Everything but n-th dim.
    array<T, 2> mat(first_dim(a), ncols); // result

    auto a_0 = a(0, ellipsis()); // FIXME for_each should take only the lengths ...
    for (long n : range(first_dim(a))) {
      if constexpr (R == 1)
        mat(n, 0) = a(n);
      else
        foreach (a_0, [&a, &mat, n, c = 0ll ](auto &&... i) mutable { mat(n, c++) = a(n, i...); })
          ;
    }
    return std::move(mat);
  }

  //template <typename T, int R> matrix<T> flatten_2d(array<T, R> const &a, int n) { return flatten_2d(a(), n); }

  //-------------------------------------

  template <int N, typename... Ms, typename Target> auto flatten_gf_2d(gf_const_view<cartesian_product<Ms...>, Target> g) {
    return gf{std::get<N>(g.mesh()), flatten_2d(g.data(), N), {}};
  }

  template <int N, typename Var, typename Target> gf<Var, tensor_valued<1>> flatten_gf_2d(gf_const_view<Var, Target> g) {
    static_assert(N == 0, "Internal error");
    return {g.mesh(), flatten_2d(g.data(), 0), {}};
  }

} // namespace triqs::gfs
