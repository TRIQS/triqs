/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include "block_gf.hpp"

namespace triqs::gfs {

  template <typename Mesh, typename Target, int Arity, bool IsConst> class block_gf_view : is_view_tag, TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {
    using this_t = block_gf_view; // for common code

    public:
    static constexpr bool is_view  = true;
    static constexpr bool is_const = IsConst;
    static constexpr int arity     = Arity;

    using mesh_t   = Mesh;
    using target_t = Target;

    using regular_type      = block_gf<Mesh, Target, Arity>;
    using mutable_view_type = block_gf_view<Mesh, Target, Arity>;
    using view_type         = block_gf_view<Mesh, Target, Arity, false>;
    using const_view_type   = block_gf_view<Mesh, Target, Arity, true>;

    /// The associated real type
    using real_t = block_gf_view<Mesh, typename Target::real_t, Arity, IsConst>;

    // FIXME : Const as template
    using g_t           = std::conditional_t<IsConst, gf_const_view<Mesh, Target>, gf_view<Mesh, Target>>;
    using data_t        = std::conditional_t<Arity == 1, std::vector<g_t>, std::vector<std::vector<g_t>>>;
    using block_names_t = std::conditional_t<Arity == 1, std::vector<std::string>, std::vector<std::vector<std::string>>>;

    std::string name;

    private:
    block_names_t _block_names;
    data_t _glist;

    // ---------------  Constructors --------------------

    struct impl_tag {};
    template <typename G> block_gf_view(impl_tag, G &&x) : _block_names(x.block_names()), _glist(factory<data_t>(x.data())), name(x.name) {}

    public:
    /// Copy constructor
    block_gf_view(block_gf_view const &x) = default;

    /// Move constructor
    block_gf_view(block_gf_view &&) = default;

    /// Construct from block_names and list of gf
    block_gf_view(block_names_t b, data_t d) : _block_names(std::move(b)), _glist(std::move(d)) {
      if constexpr (Arity == 1) {
        if (_glist.size() != _block_names.size())
          TRIQS_RUNTIME_ERROR << "block_gf(vector<string>, vector<gf>) : the two vectors do not have the same size !";
      } else {
        if (_glist.size() != _block_names[0].size())
          TRIQS_RUNTIME_ERROR << "block2_gf(vector<vector<string>>, vector<vector<gf>>) : Outer vectors have different sizes !";
        if (_glist.size() != 0)
          if (_glist[0].size() != _block_names[1].size())
            TRIQS_RUNTIME_ERROR << "block2_gf(vector<vector<string>>, vector<vector<gf>>) : Inner vectors have different sizes !";
      }
    }

    // ---------------  Constructors --------------------

    block_gf_view() = default;

    block_gf_view(block_gf<Mesh, Target, Arity> const &g) requires(IsConst) : block_gf_view(impl_tag{}, g) {}
    
    block_gf_view(block_gf<Mesh, Target, Arity> &g) requires(!IsConst) : block_gf_view(impl_tag{}, g) {}

    block_gf_view(block_gf<Mesh, Target, Arity> &&g) noexcept : block_gf_view(impl_tag{}, std::move(g)) {}

    block_gf_view(block_gf_view<Mesh, Target, Arity, !IsConst> const &g) requires(IsConst) : block_gf_view(impl_tag{}, g) {}

    /// ---------------  Operator = --------------------

    /// Copy the data, without resizing the view.
    block_gf_view &operator=(block_gf_view const &rhs) requires(not IsConst) {
      _assign_impl(rhs);
      return *this;
    }

    /**
     *  RHS can be anything with .block_names() and [n] -> gf or a scalar
     */
    template <typename RHS> block_gf_view &operator=(RHS const &rhs) requires(not IsConst) {
      if constexpr (not nda::is_scalar_v<RHS>) {
        if (!(size() == rhs.size())) TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible size" << size() << " vs " << rhs.size();
        _assign_impl(rhs);
      } else {
        if constexpr (Arity == 1) {
          for (auto &y : _glist) y = rhs;
        } else {
          for (auto &x : _glist)
            for (auto &y : x) y = rhs;
        }
      }
      return *this;
    }

    /**
    * Assignment operator overload specific for lazy_transform objects
    *
    * @param rhs The lazy object returned e.g. by fourier(my_block_gf)
    */
    template <typename L, typename G> block_gf_view &operator=(lazy_transform_t<L, G> const &rhs) requires(not IsConst) {
      if constexpr (Arity == 1) {
        for (int i = 0; i < rhs.value.size(); ++i) (*this)[i] = rhs.lambda(rhs.value[i]);
      } else {

        for (int i = 0; i < rhs.value.size1(); ++i)
          for (int j = 0; j < rhs.value.size2(); ++j) (*this)(i, j) = rhs.lambda(rhs.value(i, j));
      }
      return *this;
    }

    /**
    * Assignment operator overload specific for mpi::lazy objects (keep before general assignment)
    * @param l The lazy object returned by reduce
    */
    block_gf_view &operator=(mpi::lazy<mpi::tag::reduce, const_view_type> l) requires(not IsConst) {
      if (l.rhs.size() != this->size())
        TRIQS_RUNTIME_ERROR << "mpi reduction of block_gf : size of RHS is incompatible with the size of the view to be assigned to";
      _block_names = l.rhs.block_names();
      if constexpr (Arity == 1) {
        for (int i = 0; i < size(); ++i) _glist[i] = mpi::reduce(l.rhs.data()[i], l.c, l.root, l.all, l.op);
      } else {

        for (int i = 0; i < size1(); ++i)
          for (int j = 0; j < size2(); ++j) _glist[i][j] = mpi::reduce(l.rhs.data()[i][j], l.c, l.root, l.all, l.op);
      }
      // here we need to enumerate the vector, the mpi::reduce produce a vector<gf>, NOT a gf_view,
      // we can not overload the = of vector for better API.
      return *this;
    }

    // ---------------  Rebind --------------------
    /// Rebind
    void rebind(block_gf_view x) noexcept {
      _block_names = x._block_names;
      _glist       = data_t{x._glist}; // copy of vector<vector<gf_view>>, makes new views on the gf of x
      name         = x.name;
    }
    void rebind(block_gf_view<Mesh, Target, Arity, !IsConst> const &X) noexcept requires(IsConst) { rebind(block_gf_view{X}); }
    void rebind(block_gf<Mesh, Target, Arity> const &X) noexcept requires(IsConst) { rebind(block_gf_view{X}); }
    void rebind(block_gf<Mesh, Target, Arity> &X) noexcept { rebind(block_gf_view{X}); }

    public:
    //----------------------------- print  -----------------------------
    friend std::ostream &operator<<(std::ostream &out, block_gf_view const &x) { return out << "block_gf_view"; }

    // Common code for gf, gf_view, gf_const_view
#include "./_block_gf_view_common.hpp"
  };

} // namespace triqs::gfs

/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
  template <typename Mesh, typename Target, int Arity, bool IsConst>
  void swap(triqs::gfs::block_gf_view<Mesh, Target, Arity, IsConst> &a, triqs::gfs::block_gf_view<Mesh, Target, Arity, IsConst> &b) = delete;
} // namespace std
