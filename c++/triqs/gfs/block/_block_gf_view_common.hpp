// Copyright (c) 2020-2023 Simons Foundation
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
 * @brief Member code shared by triqs::gfs::block_gf and triqs::gfs::block_gf_view.
 */

// This is not a standalone header: it is #included inside the bodies of the block_gf / block_gf_view classes, so it
// cannot carry its own #include directives. The including files must provide, in addition to the block-gf machinery:
//   - <h5/h5.hpp> (via ../../arrays.hpp): h5::group, h5_write/h5_read, read/write_hdf5_format
//   - ../../utility/exceptions.hpp: TRIQS_RUNTIME_ERROR
//   - ../../utility/macros.hpp: EXPECTS
//   - <itertools/itertools.hpp>: itertools::enumerate, itertools::zip
//   - nda::clef (via ../../arrays.hpp): clef::make_expr_call, clef::make_expr_subscript
//   - <iterator> (std::forward_iterator_tag), <string> (std::string), <type_traits>, <vector> (std::vector)

// common to many classes

// ------------- Accessors -----------------------------

/**
 * @brief Direct access to the blocks.
 * @return A reference to the block storage.
 */
data_t &data() { return _glist; }

/**
 * @brief Direct access to the blocks (const overload).
 * @return A const reference to the block storage.
 */
data_t const &data() const { return _glist; }

/**
 * @brief Get the block names.
 * @return A const reference to the block names.
 */
block_names_t const &block_names() const { return _block_names; }

/**
 * @brief Get the matrix size of each block.
 *
 * @details Only valid for matrix-valued blocks (rank-2 target); requires each block to have a square target shape.
 *
 * @return A vector holding the size of each block.
 */
std::vector<int> block_sizes() const {
  static_assert(g_t::target_t::rank == 2);
  auto res = std::vector<int>(_glist.size());
  for (auto [i, g] : itertools::enumerate(_glist)) {
    EXPECTS(g.target_shape()[0] == g.target_shape()[1]);
    res[i] = g.target_shape()[0];
  }
  return res;
}

/**
 * @brief Get the block structure.
 *
 * @details Will fail if the target shape is non-square, because block_sizes() will fail.
 *
 * @return A triqs::gfs::gf_struct_t pairing each block name with its size.
 */
gf_struct_t gf_struct() const {
  gf_struct_t result;
  for (auto [blk_name, size] : itertools::zip(block_names(), block_sizes())) result.emplace_back(blk_name, size);
  return result;
}

/**
 * @brief Get the number of blocks along the first index (block2_gf only).
 * @return The number of blocks along the first index.
 */
int size1() const
  requires(Arity == 2)
{
  return _glist.size();
}

/**
 * @brief Get the number of blocks along the second index (block2_gf only).
 * @return The number of blocks along the second index.
 */
int size2() const
  requires(Arity == 2)
{
  return _glist.at(0).size();
}

/**
 * @brief Get the total number of blocks.
 * @return The number of blocks (the product of both dimensions for a block2_gf).
 */
int size() const {
  if constexpr (Arity == 1) {
    return _glist.size();
  } else {
    return size1() * size2();
  }
}

private:
// Assign block by block from a right hand side modeling the block-gf concept, copying its block names.
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): rhs is read block-wise (operator[]/()), not forwarded
template <typename RHS> void _assign_impl(RHS &&rhs) {

  if constexpr (Arity == 1) {
    for (int w = 0; w < size(); ++w) _glist[w] = rhs[w];
  } else {
    for (int w = 0; w < size1(); ++w)
      for (int v = 0; v < size2(); ++v) _glist[w][v] = rhs(w, v);
  }
  _block_names = rhs.block_names();
}

public:
// ------------- All the call operators without lazy arguments -----------------------------

// First, a simple () returns a view, like for an array...
/// Make a const view of *this.
const_view_type operator()() const { return *this; }

/// Make a view of *this if it is non const.
view_type operator()() { return *this; }

/**
 * @brief Access the n-th block of a one-index block Green's function.
 * @param n Block index.
 * @return The n-th block.
 */
decltype(auto) operator()(int n) const
  requires(Arity == 1)
{
  return _glist[n];
}

/**
 * @brief Access block (n1, n2) of a two-index block Green's function (const overload).
 * @param n1 First block index.
 * @param n2 Second block index.
 * @return The block at (n1, n2).
 */
decltype(auto) operator()(int n1, int n2) const
  requires(Arity == 2)
{
  return _glist[n1][n2];
}

/**
 * @brief Access block (n1, n2) of a two-index block Green's function.
 * @param n1 First block index.
 * @param n2 Second block index.
 * @return The block at (n1, n2).
 */
decltype(auto) operator()(int n1, int n2)
  requires(Arity == 2)
{
  return _glist[n1][n2];
}

// ------------- Call with lazy arguments -----------------------------

/**
 * @brief Make a lazy CLEF call expression (lvalue overload).
 * @tparam Args Types of the lazy CLEF arguments.
 * @return A lazy call expression.
 */
template <typename... Args>
auto operator()(Args &&...args) &
  requires(nda::clef::is_clef_expression<Args...>)
{
  return clef::make_expr_call(*this, std::forward<Args>(args)...);
}

/// Const lvalue overload of the lazy CLEF call (see the lvalue overload).
template <typename... Args>
auto operator()(Args &&...args) const &
  requires(nda::clef::is_clef_expression<Args...>)
{
  return clef::make_expr_call(*this, std::forward<Args>(args)...);
}

/// Rvalue overload of the lazy CLEF call (see the lvalue overload).
template <typename... Args>
auto operator()(Args &&...args) &&
  requires(nda::clef::is_clef_expression<Args...>)
{
  return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
}

// ------------- All the [] operators without lazy arguments -----------------------------

/**
 * @brief Access the n-th block of a one-index block Green's function (const overload).
 * @param n Block index.
 * @return The n-th block.
 */
decltype(auto) operator[](int n) const
  requires(Arity == 1)
{
  return _glist[n];
}

/**
 * @brief Access the n-th block of a one-index block Green's function.
 * @param n Block index.
 * @return The n-th block.
 */
decltype(auto) operator[](int n)
  requires(Arity == 1)
{
  return _glist[n];
}

// ------------- [] with lazy arguments -----------------------------

/**
 * @brief Make a lazy CLEF subscript expression (const lvalue overload).
 * @tparam Arg Type of the lazy CLEF argument.
 * @param arg Lazy CLEF argument.
 * @return A lazy subscript expression.
 */
template <typename Arg>
auto operator[](Arg &&arg) const &
  requires(nda::clef::is_clef_expression<Arg>)
{
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

/// Lvalue overload of the lazy CLEF subscript (see the const lvalue overload).
template <typename Arg>
auto operator[](Arg &&arg) &
  requires(nda::clef::is_clef_expression<Arg>)
{
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

/// Rvalue overload of the lazy CLEF subscript (see the const lvalue overload).
template <typename Arg>
auto operator[](Arg &&arg) &&
  requires(nda::clef::is_clef_expression<Arg>)
{
  return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
}

//----------------------------- HDF5 -----------------------------

/**
 * @brief Get the HDF5 format tag of a block Green's function.
 * @return The string `"BlockGf"` (`"Block2Gf"` for a block2_gf).
 */
[[nodiscard]] static std::string hdf5_format() {
  if constexpr (Arity == 1)
    return "BlockGf";
  else
    return "Block2Gf";
}

/**
 * @brief Write a block Green's function to HDF5.
 *
 * @param fg `h5::group` to write into.
 * @param subgroup_name Name of the subgroup to create.
 * @param g Block Green's function to write.
 */
friend void h5_write(h5::group fg, std::string const &subgroup_name, this_t const &g) {
  auto gr = fg.create_group(subgroup_name);
  write_hdf5_format(gr, g);

  if constexpr (Arity == 1) {
    h5_write(gr, "block_names", g.block_names());
    for (int i = 0; i < g.size(); ++i) h5_write(gr, g.block_names()[i], g.data()[i]);
  } else {
    h5_write(gr, "block_names1", g.block_names()[0]);
    h5_write(gr, "block_names2", g.block_names()[1]);
    for (int i = 0; i < g.size1(); ++i)
      for (int j = 0; j < g.size2(); ++j) h5_write(gr, g.block_names()[0][i] + "_" + g.block_names()[1][j], g._glist[i][j]);
  }
}

/**
 * @brief Read a block Green's function from HDF5.
 *
 * @details Throws a triqs::runtime_error if the HDF5 format tag does not correspond to the expected block format.
 *
 * @param fg `h5::group` to read from.
 * @param subgroup_name Name of the subgroup to read.
 * @param g Block Green's function to read into.
 */
friend void h5_read(h5::group fg, std::string const &subgroup_name, this_t &g) {
  auto gr = fg.open_group(subgroup_name);
  // Check the attribute or throw
  auto tag_file     = read_hdf5_format(gr);
  auto tag_expected = this_t::hdf5_format();
  if (tag_file != tag_expected)
    TRIQS_RUNTIME_ERROR << "h5_read : mismatch of the Format tag in the h5 group : found " << tag_file << " while I expected " << tag_expected;
  if constexpr (Arity == 1) {

    auto block_names = h5::h5_read<std::vector<std::string>>(gr, "block_names");
    int s            = block_names.size();
    g._glist.resize(s);
    g._block_names = block_names;
    for (int i = 0; i < s; ++i) h5_read(gr, block_names[i], g._glist[i]);
  } else {
    auto block_names1 = h5::h5_read<std::vector<std::string>>(gr, "block_names1");
    auto block_names2 = h5::h5_read<std::vector<std::string>>(gr, "block_names2");
    auto block_names  = std::vector<std::vector<std::string>>{block_names1, block_names2};
    int s0            = block_names[0].size();
    int s1            = block_names[1].size();
    g._glist.resize(s0);
    g._block_names = block_names;
    for (int i = 0; i < s0; ++i) {
      g._glist[i].resize(s1);
      for (int j = 0; j < s1; ++j) h5_read(gr, block_names[0][i] + "_" + block_names[1][j], g._glist[i][j]);
    }
  }
}

// -------------------------------  iterator  --------------------------------------------------

/**
 * @brief Forward iterator over the blocks of a block Green's function.
 *
 * @details Iterates over the blocks in row-major order (for a block2_gf, block (i, j) comes before (i, j+1)).
 *
 * @tparam is_const Whether the iterator yields const blocks.
 */
template <bool is_const> class iterator_impl {
  std::conditional_t<is_const, const this_t *, this_t *> bgf = NULL;
  int n;

  public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = g_t;
  using difference_type   = std::ptrdiff_t;
  using pointer           = std::conditional_t<is_const, const g_t *, g_t *>;
  using reference         = std::conditional_t<is_const, g_t const &, g_t &>;
  using block_gf_ref      = std::conditional_t<is_const, this_t const &, this_t &>;

  /// Construct a past-the-end (singular) iterator.
  iterator_impl() = default;

  /// Construct an iterator over a block Green's function, optionally positioned at the end.
  iterator_impl(block_gf_ref _bgf, bool at_end = false) : bgf(&_bgf), n(at_end ? bgf->size() : 0) {}

  /// Construct an iterator over a block Green's function positioned at block `_n`.
  iterator_impl(block_gf_ref _bgf, int _n) : bgf(&_bgf), n(_n) {}

  /// Convert a non-const iterator to a const iterator.
  operator iterator_impl<true>() const { return iterator_impl<true>(*bgf, n); }

  /// Dereference to the current block.
  reference operator*() {
    if constexpr (Arity == 1) {
      return (*bgf)[n];
    } else {
      return (*bgf)(n / bgf->size2(), n % bgf->size2());
    }
  }

  /// Dereference to the current block (const overload).
  reference operator*() const {
    if constexpr (Arity == 1) {
      return (*bgf)[n];
    } else {
      return (*bgf)(n / bgf->size2(), n % bgf->size2());
    }
  }

  /// Member access to the current block.
  reference operator->() { return operator*(); }

  /// Pre-increment to the next block.
  iterator_impl &operator++() {
    ++n;
    return *this;
  }

  /// Post-increment to the next block.
  iterator_impl operator++(int) {
    auto it = *this;
    ++n;
    return it;
  }

  /// Equality comparison (same block Green's function and same position).
  bool operator==(iterator_impl const &other) const { return ((bgf == other.bgf) && (n == other.n)); }

  /// Inequality comparison.
  bool operator!=(iterator_impl const &other) const { return (!operator==(other)); }
};

/// Mutable block iterator type.
using iterator = iterator_impl<false>;

/// Const block iterator type.
using const_iterator = iterator_impl<true>;

//------------

/// Get an iterator to the first block.
iterator begin() { return {*this, false}; }

/// Get a const iterator to the first block.
const_iterator begin() const { return {*this, false}; }

/// Get an iterator past the last block.
iterator end() { return {*this, true}; }

/// Get a const iterator past the last block.
const_iterator end() const { return {*this, true}; }

/// Get a const iterator to the first block.
auto cbegin() { return const_view_type(*this).begin(); }

/// Get a const iterator past the last block.
auto cend() { return const_view_type(*this).end(); }
