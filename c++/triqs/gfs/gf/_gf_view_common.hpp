// Copyright (c) 2019-2023 Simons Foundation
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

/**
 * @file
 * @brief Member code shared by triqs::gfs::gf, triqs::gfs::gf_view and triqs::gfs::gf_const_view.
 */

// This is not a standalone header: it is #included inside the bodies of the gf / gf_view / gf_const_view classes, so it
// cannot carry its own #include directives. The including files must provide, in addition to the gf machinery:
//   - <h5/h5.hpp> (via ../../arrays.hpp): h5::group, h5_write/h5_read, read/write_hdf5_format
//   - ../../utility/exceptions.hpp: TRIQS_RUNTIME_ERROR
//   - ../../utility/macros.hpp: EXPECTS_WITH_MESSAGE
//   - <numeric> (std::accumulate), <ostream> (std::ostream), <string> (std::string), <tuple> (std::tuple, std::apply)

// common to many classes

// ------------- All the call operators arguments -----------------------------

/**
 * @brief Evaluate the Green's function at the given arguments.
 *
 * @details With no argument, returns a (const) view of the whole Green's function. Otherwise, the number of arguments
 * must equal the arity of the mesh; if any argument is a lazy CLEF expression a lazy call expression is returned,
 * else the Green's function is evaluated using the mesh's interpolation scheme (triqs::gfs::gf_evaluator).
 *
 * @tparam Self Deduced (explicit object parameter) type of the Green's function.
 * @tparam Args Types of the evaluation arguments (one per mesh dimension).
 * @return A view (no argument), a lazy expression (lazy arguments) or the interpolated value.
 */
#if __cpp_explicit_this_parameter >= 202110L
template <typename Self, typename... Args> decltype(auto) operator()(this Self &&self, Args &&...args) {
#else
template <typename Self, typename... Args> static decltype(auto) call_impl(Self &&self, Args &&...args) {
#endif
  if constexpr (sizeof...(Args) == 0) {
    if constexpr (std::is_const_v<std::remove_reference_t<Self>>)
      return const_view_type{std::forward<Self>(self)};
    else
      return view_type{std::forward<Self>(self)};
  } else {
    static_assert((sizeof...(Args) == n_variables<mesh_t>), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>)) // any argument is lazy ?
      return clef::make_expr_call(std::forward<Self>(self), std::forward<Args>(args)...);
    else
      return gf_evaluator<mesh_t>{}(std::forward<Self>(self), std::forward<Args>(args)...);
  }
}
#if __cpp_explicit_this_parameter < 202110L
/// Const lvalue overload of the call operator (delegates to call_impl()).
template <typename... Args> decltype(auto) operator()(Args &&...args) const & { return call_impl(*this, std::forward<Args>(args)...); }
/// Lvalue overload of the call operator (delegates to call_impl()).
template <typename... Args> decltype(auto) operator()(Args &&...args) & { return call_impl(*this, std::forward<Args>(args)...); }
/// Rvalue overload of the call operator (delegates to call_impl()).
template <typename... Args> decltype(auto) operator()(Args &&...args) && { return call_impl(std::move(*this), std::forward<Args>(args)...); }
#endif

// ------------- [] operator -----------------------------

/// Whether bound checking of the data array is disabled (controlled by `NDA_ENFORCE_BOUNDCHECK`).
#ifdef NDA_ENFORCE_BOUNDCHECK
static constexpr bool has_no_boundcheck = false;
#else
static constexpr bool has_no_boundcheck = true;
#endif

/**
 * @brief Access the Green's function at a mesh point/index passed as a tuple (const overload).
 *
 * @details Flattens the tuple into the variadic `operator[]`. Ensures backward compatibility and easy use when the
 * arguments are stored in a tuple.
 *
 * @tparam T Types of the tuple elements (one per mesh dimension).
 * @param tu Tuple of mesh points/indices.
 * @return The value (or sub-view) at the given point.
 */
template <typename... T>
decltype(auto) operator[](std::tuple<T...> const &tu) const noexcept(has_no_boundcheck)
  requires(sizeof...(T) == arity)
{
  return std::apply([this](auto &...x) -> decltype(auto) { return this->operator[](x...); }, tu);
}

/// Non-const overload of the tuple `operator[]` (see the const overload).
template <typename... T>
decltype(auto) operator[](std::tuple<T...> const &tu) noexcept(has_no_boundcheck)
  requires(sizeof...(T) == arity)
{
  return std::apply([this](auto &...x) -> decltype(auto) { return this->operator[](x...); }, tu);
}

private:
// local helper functions for the _subscript_impl below
template <Mesh MT, typename X> static auto call_to_data_index(MT const &m, X const &x) {
  if constexpr (std::is_same_v<X, all_t>)
    return all_t{};
  else if constexpr (std::is_same_v<X, typename MT::mesh_point_t>) {
    static_assert(std::is_same_v<X, typename MT::mesh_point_t>, "Incompatible mesh_point type passed to a gf via [] operator.");
    EXPECTS_WITH_MESSAGE(m.mesh_hash() == x.mesh_hash(),
                         "Passing to a gf a mesh point of incompatible mesh (but correct type), e.g. different beta, or other parameter.");
    return x.data_index();
  } else
    return m.to_data_index(x);
}

// ------------------------------------------
// General implementation for any set of arguments.
// https://godbolt.org/z/sbqYv3oeE
template <typename Self, typename... Arg>
static decltype(auto) _subscript_impl(Self &&self, Arg &&...arg)
  requires(sizeof...(Arg) == arity)
{
  if constexpr ((clef::is_any_lazy<Arg> or ... or false))
    return clef::make_expr_subscript(std::forward<Self>(self), std::forward<Arg>(arg)...);
  else {
    // Count the number of all_t
    static constexpr auto mesh_filter = std::array<int, sizeof...(Arg)>{std::is_same_v<range::all_t, std::decay_t<Arg>>...};
    static constexpr auto n_all       = std::accumulate(begin(mesh_filter), end(mesh_filter), 0);

    decltype(auto) new_data = [&self,
                               &arg...]<size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) { // the trailing ->decltype(auto) is crucial
      return data_t::template call<(target_t::is_matrix and n_all == 0 ? 'M' : 'A'), false>(
         std::forward<Self>(self)._data, this_t::call_to_data_index(get_mesh<Is>(self), std::forward<Arg>(arg))..., ellipsis{});
    }(std::make_index_sequence<arity>{}); //keep arity here. we could authorize to pass additional integers for direct access here ??

    if constexpr (n_all == 0)
      return new_data;
    else {
      // mesh is a tuple of meshes
      auto new_mesh = detail::filter_mesh<detail::compute_position<n_all>(mesh_filter)>(self.mesh());
      using self_t  = std::remove_reference_t<Self>;
      if constexpr (self_t::is_const or std::is_const_v<self_t>)
        return gf_const_view<decltype(new_mesh), typename self_t::target_t>{std::move(new_mesh), new_data};
      else
        return gf_view<decltype(new_mesh), typename self_t::target_t>{std::move(new_mesh), new_data};
    }
  }
}

public:
// ------------------------------------------

/**
 * @brief Access the Green's function at a mesh point/index (const lvalue overload).
 *
 * @details Unlike `operator()`, this accesses the stored value directly (no interpolation). Passing `all_t` for one or
 * more dimensions returns a (const) view sliced over the remaining mesh, and lazy arguments return a lazy subscript
 * expression.
 *
 * @tparam Arg Types of the access arguments (one per mesh dimension).
 * @param arg Mesh points, indices or `all_t` selectors.
 * @return The stored value, a sliced view, or a lazy expression.
 */
template <typename... Arg>
decltype(auto) operator[](Arg &&...arg) const & noexcept(has_no_boundcheck)
  requires(sizeof...(Arg) == arity)
{
  return _subscript_impl(*this, std::forward<Arg>(arg)...);
}

/// Lvalue overload of the variadic `operator[]` (see the const lvalue overload).
template <typename... Arg>
decltype(auto) operator[](Arg &&...arg) & noexcept(has_no_boundcheck)
  requires(sizeof...(Arg) == arity)
{
  return _subscript_impl(*this, std::forward<Arg>(arg)...);
}

/// Rvalue overload of the variadic `operator[]` (see the const lvalue overload).
template <typename... Arg>
decltype(auto) operator[](Arg &&...arg) && noexcept(has_no_boundcheck)
  requires(sizeof...(Arg) == arity)
{
  return _subscript_impl(std::move(*this), std::forward<Arg>(arg)...);
}

/**
 * @brief Access a single-variable Green's function by a mesh index.
 *
 * @param index Mesh index.
 * @return The stored value at the corresponding mesh point.
 */
decltype(auto) operator[](typename mesh_t::index_t const &index) noexcept(has_no_boundcheck)
  requires(arity == 1)
{
  return this_t::operator[]<typename mesh_t::index_t const &>(index);
}

//----------------------------- HDF5 -----------------------------

/**
 * @brief Get the HDF5 format tag of a Green's function.
 * @return The string `"Gf"`.
 */
[[nodiscard]] static std::string hdf5_format() { return "Gf"; }

// Grants the HDF5 read/write traits class access to the private members.
friend struct gf_h5_rw<M, Target>;

/**
 * @brief Write a Green's function to HDF5.
 *
 * @param fg `h5::group` to write into.
 * @param subgroup_name Name of the subgroup to create.
 * @param g Green's function to write.
 */
friend void h5_write(h5::group fg, std::string const &subgroup_name, this_t const &g) {
  auto gr = fg.create_group(subgroup_name);
  write_hdf5_format(gr, g);
  gf_h5_rw<M, Target>::write(gr, g);
}

/**
 * @brief Read a Green's function from HDF5.
 *
 * @details Throws a triqs::runtime_error if the HDF5 format tag does not correspond to a Green's function.
 *
 * @param fg `h5::group` to read from.
 * @param subgroup_name Name of the subgroup to read.
 * @param g Green's function to read into.
 */
friend void h5_read(h5::group fg, std::string const &subgroup_name, this_t &g) {
  auto gr       = fg.open_group(subgroup_name);
  auto tag_file = read_hdf5_format(gr);
  if (!(tag_file[0] == 'G' and tag_file[1] == 'f'))
    TRIQS_RUNTIME_ERROR << "h5_read : For a Green function, the type tag should be Gf (or Gfxxxx for old archive) "
                        << " while I found " << tag_file;
  gf_h5_rw<M, Target>::read(gr, g);
}

//----------------------------- print  -----------------------------

/**
 * @brief Write a Green's function to an output stream.
 * @param out `std::ostream` output stream.
 * @return Reference to the output stream.
 */
friend std::ostream &operator<<(std::ostream &out, this_t const &) { return out << "this_t"; }
