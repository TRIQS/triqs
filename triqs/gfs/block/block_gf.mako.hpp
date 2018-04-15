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

#include "./../../hilbert_space/fundamental_operator_set.hpp"

namespace triqs {
 namespace gfs {

  using triqs::utility::factory;
  using triqs::mpi::mpi_reduce;

  /*----------------------------------------------------------
   *   Declaration of main types : gf, gf_view, gf_const_view
   *--------------------------------------------------------*/
  template <typename Var, typename Target = matrix_valued> class block_gf;
  template <typename Var, typename Target = matrix_valued> class block_gf_view;
  template <typename Var, typename Target = matrix_valued> class block_gf_const_view;

  template <typename Var, typename Target = matrix_valued> class block2_gf;
  template <typename Var, typename Target = matrix_valued> class block2_gf_view;
  template <typename Var, typename Target = matrix_valued> class block2_gf_const_view;

  /// ---------------------------  traits ---------------------------------

  // Is G a block_gf, block_gf_view, block_gf_const_view
  // is_block_gf_or_view<G> is true iif G is a block_gf or block2_gf
  // is_block_gf_or_view<G,1> is true iff G is a block_gf
  // is_block_gf_or_view<G,2> is true iff G is a block2_gf
  //
  template <typename G, int n> struct _is_block_gf_or_view : std::false_type {};

  template <typename G, int n = 0> using is_block_gf_or_view = _is_block_gf_or_view<std14::decay_t<G>, n>;

  template <typename G>
  struct _is_block_gf_or_view<G, 0>
      : std::integral_constant<bool, is_block_gf_or_view<G, 1>::value || is_block_gf_or_view<G, 2>::value> {};

  template <typename V, typename T> struct _is_block_gf_or_view<block_gf<V, T>, 1> : std::true_type {};
  template <typename V, typename T> struct _is_block_gf_or_view<block_gf_view<V, T>, 1> : std::true_type {};
  template <typename V, typename T> struct _is_block_gf_or_view<block_gf_const_view<V, T>, 1> : std::true_type {};
  template <typename V, typename T> struct _is_block_gf_or_view<block2_gf<V, T>, 2> : std::true_type {};
  template <typename V, typename T> struct _is_block_gf_or_view<block2_gf_view<V, T>, 2> : std::true_type {};
  template <typename V, typename T> struct _is_block_gf_or_view<block2_gf_const_view<V, T>, 2> : std::true_type {};

  // Given a gf G, the corresponding block
  template <typename G> using get_variable_t          = typename std::decay_t<G>::variable_t;
  template <typename G> using get_target_t            = typename std::decay_t<G>::target_t;
  template <typename G> using block_gf_of             = block_gf<get_variable_t<G>, get_target_t<G>>;
  template <typename G> using block_gf_view_of        = block_gf_view<get_variable_t<G>, get_target_t<G>>;
  template <typename G> using block_gf_const_view_of  = block_gf_const_view<get_variable_t<G>, get_target_t<G>>;
  template <typename G> using block2_gf_of            = block2_gf<get_variable_t<G>, get_target_t<G>>;
  template <typename G> using block2_gf_view_of       = block2_gf_view<get_variable_t<G>, get_target_t<G>>;
  template <typename G> using block2_gf_const_view_of = block2_gf_const_view<get_variable_t<G>, get_target_t<G>>;

  // The trait that "marks" the Green function
  TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(BlockGreenFunction);

  // ------------- Helper Types -----------------------------

  template <typename Lambda, typename T> struct lazy_transform_t {
   Lambda lambda;
   T value;
  };

  template <typename Lambda, typename T> lazy_transform_t<Lambda, T> make_lazy_transform(Lambda&& l, T&& x) {
   return {std::forward<Lambda>(l), std::forward<T>(x)};
  }

  /// ---------------------------  details  ---------------------------------

  namespace details {
   inline auto _make_block_names1(int n) {
    std::vector<std::string> r(n);
    for (int i = 0; i < n; ++i)
     r[i]      = std::to_string(i);
    return r;
   }
   inline std::vector<std::vector<std::string>> _make_block_names2(int n, int p) {
    return {_make_block_names1(n), _make_block_names1(p)};
   }
  }

  /// ---------------------------  implementation  ---------------------------------

  /*mako
  %for ROOT in ['block_gf', 'block2_gf'] :
   %for RVC in ['regular', 'view', 'const_view'] :
   <%
      EXT = '_' + RVC if 'view' in RVC else ''
      GF = ROOT + EXT
      GV = ROOT + '_' + (RVC if 'view' in RVC else 'view')
      ISVIEW = str('view' in GF).lower()
      ISCONST = str(RVC == 'const_view').lower()
      ARITY = 2 if '2' in ROOT else 1
   %>
  */

  // ----------------------  MAKO_GF -----------------------------------------
  /**
   * MAKO_GF
   */
  template <typename Var, typename Target> class MAKO_GF : 
   // mako %if 'view' in RVC:
   is_view_tag, 
   // mako %endif
   TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {

   public:
   static constexpr bool is_view  = MAKO_ISVIEW;
   static constexpr bool is_const = MAKO_ISCONST;
   static constexpr int arity     = MAKO_ARITY;

   using variable_t = Var;
   using target_t   = Target;

   using regular_type      = MAKO_ROOT<Var, Target>;
   using mutable_view_type = MAKO_ROOT_view<Var, Target>;
   using view_type         = MAKO_GV<Var, Target>;
   using const_view_type   = MAKO_ROOT_const_view<Var, Target>;

   /// The associated real type
   using real_t = MAKO_GF<Var, typename Target::real_t>;

   using g_t = gfMAKO_EXT<Var, Target>;
   // mako %if ARITY == 1 :
   using data_t        = std::vector<g_t>;
   using block_names_t = std::vector<std::string>;
   // mako %else:
   using data_t        = std::vector<std::vector<g_t>>;
   using block_names_t = std::vector<std::vector<std::string>>;
   // mako %endif

   // ------------- Accessors -----------------------------

   /// Direct access to the data array
   data_t& data() { return _glist; }

   /// Const version
   data_t const& data() const { return _glist; }

   ///
   block_names_t const& block_names() const { return _block_names; }

   // mako %if ARITY == 1 :
   int size() const { return _glist.size(); }
   // mako %else:
   int size1() const { return _glist.size(); }
   int size2() const { return _glist[0].size(); } // FIXME PROTECT
   int size() const { return size1() * size2(); }
   // mako %endif

   // mako %if ARITY == 1 :
   // backwd compat only
   // TO BE REMOVED IN FUTURE RELEASE
   //
   struct __dom {
    MAKO_GF const* _g;
    int size() const { return _g->size(); }
    auto const& names() const { return _g->block_names(); }
   };
   TRIQS_DEPRECATED("g.domain() is deprecated. Replace g.domain().size() by g.size(), and g.domain().names() by g.block_names()")
   __dom domain() const { return {this}; }
   TRIQS_DEPRECATED(
       "g.mesh() is deprecated. Replace for (auto & x : g.mesh()) by \n for (auto &x : range(g.size()) or \n for (int x=0; "
       "x<g.size(); ++x)")
   arrays::range mesh() const { return arrays::range{0, size()}; }
   // mako %endif

   std::string name;

   private:
   block_names_t _block_names;
   data_t _glist;

   // ---------------  Constructors --------------------

   struct impl_tag {};
   template <typename G>
   MAKO_GF(impl_tag, G&& x) : _block_names(x.block_names()), _glist(factory<data_t>(x.data())), name(x.name) {}

   public:
   /// Copy constructor
   MAKO_GF(MAKO_GF const& x) = default;

   /// Move constructor
   MAKO_GF(MAKO_GF&&) = default;

   /// Construct from block_names and list of gf
   MAKO_GF(block_names_t b, data_t d) : _block_names(std::move(b)), _glist(std::move(d)) {
    // mako %if ARITY == 1 :
    if (_glist.size() != _block_names.size())
      TRIQS_RUNTIME_ERROR << "block_gf(vector<string>, vector<gf>) : the two vectors do not have the same size !";
    // mako %else:
    if (_glist.size() != _block_names[0].size())
     TRIQS_RUNTIME_ERROR << "block2_gf(vector<vector<string>>, vector<vector<gf>>) : Outer vectors have different sizes !";
    if (_glist.size() != 0)
     if (_glist[0].size() != _block_names[1].size())
      TRIQS_RUNTIME_ERROR << "block2_gf(vector<vector<string>>, vector<vector<gf>>) : Inner vectors have different sizes !";
    // mako %endif
   }

   // ---------------  Constructors --------------------
   // mako %if RVC == 'regular' :

   /// Construct an empty Green function (with empty array).
   MAKO_GF() = default;

   /// From a block_gf_view of the same kind
   MAKO_GF(view_type const& g) : MAKO_GF(impl_tag{}, g) {}

   /// From a const_gf_view of the same kind
   MAKO_GF(const_view_type const& g) : MAKO_GF(impl_tag{}, g) {}

   /// Construct from anything which models BlockGreenFunction.
   // TODO: We would like to refine this, G should have the same mesh, target, at least ...
   template <typename G> MAKO_GF(G const& x, std14::enable_if_t<BlockGreenFunction<G>::value>* dummy = 0) : MAKO_GF() {
    *this = x;
   }

   /// Construct from the mpi lazy class of the implementation class, cf mpi section
   // NB : type must be the same, e.g. g2(mpi_reduce(g1)) will work only if mesh, Target, Singularity are the same...
   template <typename Tag> MAKO_GF(mpi_lazy<Tag, MAKO_GF_const_view<Var, Target>> x) : MAKO_GF() { operator=(x); }

   // mako %if ARITY == 1 :

   /// Construct from a vector of gf
   block_gf(data_t V) : _block_names(details::_make_block_names1(V.size())), _glist(std::move(V)) {}

   /// Constructs a n block
   block_gf(int n) : block_gf(data_t(n)) {}

   /// Constructs a n block with copies of g.
   block_gf(int n, g_t const& g) : block_gf(data_t(n, g)) {}

   /// Construct from the vector of names and one gf to be copied
   block_gf(block_names_t b, g_t const& g) : _block_names(std::move(b)), _glist(_block_names.size(), g) {}

   /// Construct from the vector of names 
   block_gf(block_names_t b) : _block_names(std::move(b)), _glist(_block_names.size()) {}

   // Create Block Green function from Mesh and gf_struct
   block_gf(gf_mesh<Var> const &m, triqs::hilbert_space::gf_struct_t const &gf_struct) {

     for (auto const & [ bname, idx_lst ] : gf_struct) {
       auto bl_size = idx_lst.size();
       _block_names.push_back(bname);
       std::vector<std::string> idx_str_lst(bl_size);
       std::transform(idx_lst.cbegin(), idx_lst.cend(), idx_str_lst.begin(), [](auto &arg){ return std::to_string(arg); });
       if constexpr (Target::rank == 0)
         _glist.emplace_back(m, make_shape(bl_size, bl_size));
       else
         _glist.emplace_back(m, make_shape(bl_size, bl_size), std::vector<std::vector<std::string>>{Target::rank, idx_str_lst});
     }
   }

   // mako %else:

   /// Constructs a n blocks with copies of g.
   block2_gf(int n, int p, g_t const& g) : _block_names(details::_make_block_names2(n, p)), _glist(n, std::vector<g_t>(p, g)) {}

   /// Construct from a vector of gf
   block2_gf(data_t V) : _block_names(details::_make_block_names2(V.size(), V[0].size())), _glist(std::move(V)) {}

   // mako %endif

   // mako %elif RVC == 'view' :

   MAKO_GF()                         = default;
   MAKO_GF(const_view_type const& g) = delete; // No view from a const_view
   MAKO_GF(regular_type const& g)    = delete; // No view from a const g

   /// Makes a view
   MAKO_GF(regular_type& g) : MAKO_GF(impl_tag{}, g) {}

   /// Makes a view
   MAKO_GF(regular_type&& g) noexcept : MAKO_GF(impl_tag{}, std::move(g)) {}

   // mako %elif RVC == 'const_view' :

   MAKO_GF() = default; // views can not be default constructed

   /// Makes a const view
   MAKO_GF(mutable_view_type const& g) : MAKO_GF(impl_tag{}, g) {}

   /// Makes a const view
   MAKO_GF(regular_type const& g) : MAKO_GF(impl_tag{}, g) {}

   // mako %endif

   /// ---------------  Operator = --------------------
   private:
   template <typename RHS> void _assign_impl(RHS&& rhs) {

    // mako %if ARITY == 1 :
    for (int w = 0; w < size(); ++w) _glist[w] = rhs[w];
    // mako %else:
    for (int w = 0; w < size1(); ++w)
     for (int v = 0; v < size2(); ++v) _glist[w][v] = rhs(w,v);
    // mako %endif
    _block_names = rhs.block_names();
   }

   public:
   // mako %if RVC == 'regular' :
   /// Copy assignment
   MAKO_GF& operator=(MAKO_GF & rhs) = default;
   MAKO_GF& operator=(MAKO_GF const& rhs) = default;

   /// Move assignment
   MAKO_GF& operator=(MAKO_GF&& rhs) = default;

   /**
    * Assignment operator overload specific for mpi_lazy objects (keep before general assignment)
    *
    * @param l The lazy object returned by mpi_reduce
    */
   MAKO_GF& operator=(mpi_lazy<mpi::tag::reduce, MAKO_GF::const_view_type> l) {
    _block_names = l.rhs.block_names();
    _glist       = mpi_reduce(l.rhs.data(), l.c, l.root, l.all, l.op);
    return *this; 
    // mpi_reduce of vector produces a new vector of gf, so it is fine here
   }

   /**
   * Assignment operator
   *
   * @tparam RHS Type of the right hand side rhs
   *
   *             RHS can be anything modeling the gf concept TBW
   *             In particular lazy expression with Green functions
   * @param rhs
   * @example
   *
   * The assignment resizes the mesh and the data, invalidating all pointers on them.
   *
   */
   template <typename RHS> MAKO_GF& operator=(RHS&& rhs) {
    // mako %if ARITY == 1 :
    _glist.resize(rhs.size());
    _block_names.resize(rhs.size());
    // mako %else:
    _glist.resize(rhs.size1());
    for( auto & g_bl : _glist ) g_bl.resize(rhs.size2());
    _block_names[0].resize(rhs.size1());
    _block_names[1].resize(rhs.size2());
    // mako %endif
    _assign_impl(rhs);
    return *this;
   }

   // mako %elif RVC == 'view' :
   /**
    * Assignment operator overload specific for lazy_transform objects
    *
    * @param rhs The lazy object returned e.g. by fourier(my_block_gf)
    */
   template <typename L, typename G> MAKO_GF& operator=(lazy_transform_t<L, G> const& rhs) {
    // for (auto & [ l, r ] : zip(*this, rhs.value)) FIXME C++17
    //  l = rhs.lambda(r);
    // mako %if ARITY == 1 :
    for (int i  = 0; i < rhs.value.size(); ++i)
     (*this)[i] = rhs.lambda(rhs.value[i]);
    // mako %else:
    for (int i  = 0; i < rhs.value.size1(); ++i)
     for (int j  = 0; j < rhs.value.size2(); ++j)
      (*this)(i,j) = rhs.lambda(rhs.value(i,j));
    // mako %endif
    return *this; 
   }

   /**
    * Assignment operator overload specific for mpi_lazy objects (keep before general assignment)
    *
    * @param l The lazy object returned by mpi_reduce
    */
   void operator=(mpi_lazy<mpi::tag::reduce, MAKO_GF::const_view_type> l) {
    if (l.rhs.size() != this->size())
     TRIQS_RUNTIME_ERROR << "mpi reduction of block_gf : size of RHS is incompatible with the size of the view to be assigned to";
    _block_names = l.rhs.block_names();
    // mako %if ARITY == 1 :
    for (int i = 0; i< size(); ++i)
     _glist[i] = mpi_reduce(l.rhs.data()[i], l.c, l.root, l.all, l.op);
    // mako %else:
    for (int i = 0; i< size1(); ++i)
     for (int j = 0; j< size2(); ++j)
      _glist[i][j] = mpi_reduce(l.rhs.data()[i][j], l.c, l.root, l.all, l.op);
    // mako %endif
    // here we need to enumerate the vector, the mpi_reduce produce a vector<gf>, NOT a gf_view, 
    // we can not overload the = of vector for better API.
   }

   /**
   * Assignment operator
   *
   * @tparam RHS Type of the right hand side rhs
   *
   * 		 RHS can be anything with .block_names() and [n] -> gf
   * @param rhs
   * @example
   *
   */
   template <typename RHS> std14::enable_if_t<!arrays::is_scalar<RHS>::value, MAKO_GF&> operator=(RHS const& rhs) {
    if (!(size() == rhs.size()))
     TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible size" << size() << " vs " << rhs.size();
    _assign_impl(rhs);
    return *this;
   }

   template <typename RHS> std14::enable_if_t<arrays::is_scalar<RHS>::value, MAKO_GF&> operator=(RHS&& rhs) {
    // mako %if ARITY == 1 :
    for (auto& y : _glist)
     y = rhs;
    // mako %else:
    for (auto& x : _glist)
     for (auto& y : x)
      y = rhs;
    // mako %endif
    return *this;
   }

   /// Copy the data, without resizing the view.
   MAKO_GF& operator=(MAKO_GF const& rhs) {
    _assign_impl(rhs);
    return *this;
   }

   // mako %elif RVC == 'const_view' :
   MAKO_GF& operator=(MAKO_GF const&) = delete; // a const view can not be assigned to
   // mako %endif

   // ---------------  Rebind --------------------
   // mako %if ISVIEW :
   /// Rebind
   void rebind(MAKO_GF x) noexcept {
    _block_names = x._block_names;
    _glist       = data_t{x._glist}; // copy of vector<vector<gf_view>>, makes new views on the gf of x
    name         = x.name;
   }
   // mako %endif
   // mako %if RVC == 'const_view' :
   /// Rebind on a non const view
   void rebind(mutable_view_type const& X) noexcept { rebind(const_view_type{X}); }
   void rebind(regular_type const& X) noexcept { rebind(const_view_type{X}); }
   // mako %endif

   public:
   // ------------- All the call operators without lazy arguments -----------------------------

   // First, a simple () returns a view, like for an array...
   /// Makes a const view of *this
   const_view_type operator()() const { return *this; }
   /// Makes a view of *this if it is non const
   view_type operator()() { return *this; }

   // mako %if ARITY == 1 :
   decltype(auto) operator()(int n) const { return _glist[n]; }
   // mako %else:
   decltype(auto) operator()(int n1, int n2) const { return _glist[n1][n2]; }
   decltype(auto) operator()(int n1, int n2) { return _glist[n1][n2]; }
   // mako %endif

   // ------------- Call with lazy arguments -----------------------------

   // Calls with at least one lazy argument : we make a clef expression, cf clef documentation
   template <typename... Args> clef::make_expr_call_t<MAKO_GF&, Args...> operator()(Args&&... args) & {
    return clef::make_expr_call(*this, std::forward<Args>(args)...);
   }

   template <typename... Args> clef::make_expr_call_t<MAKO_GF const&, Args...> operator()(Args&&... args) const & {
    return clef::make_expr_call(*this, std::forward<Args>(args)...);
   }

   template <typename... Args> clef::make_expr_call_t<MAKO_GF, Args...> operator()(Args&&... args) && {
    return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
   }
   // ------------- All the [] operators without lazy arguments -----------------------------

   // mako %if ARITY == 1 :
   decltype(auto) operator[](int n) const { return _glist[n]; }
   decltype(auto) operator[](int n) { return _glist[n]; }
   // mako %else:
   // mako %endif

   // ------------- [] with lazy arguments -----------------------------

   template <typename Arg> clef::make_expr_subscript_t<MAKO_GF const&, Arg> operator[](Arg&& arg) const & {
    return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
   }

   template <typename Arg> clef::make_expr_subscript_t<MAKO_GF&, Arg> operator[](Arg&& arg) & {
    return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
   }

   template <typename Arg> clef::make_expr_subscript_t<MAKO_GF, Arg> operator[](Arg&& arg) && {
    return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
   }

   //----------------------------- HDF5 -----------------------------

   /// HDF5 name
   // mako %if ARITY == 1 :
   static std::string hdf5_scheme() {return  "BlockGf";}
   // mako %else:
   static std::string hdf5_scheme() {return  "Block2Gf";}
   // mako %endif

   /// Write into HDF5
   friend void h5_write(h5::group fg, std::string const& subgroup_name, MAKO_GF const& g) {
    auto gr = fg.create_group(subgroup_name);
    gr.write_hdf5_scheme(g);

    // mako %if ARITY == 1 :
    h5_write(gr, "block_names", g.block_names());
    for (int i = 0; i < g.size(); ++i)
     h5_write(gr, g.block_names()[i], g.data()[i]);
    // mako %else:
    h5_write(gr, "block_names1", g.block_names()[0]);
    h5_write(gr, "block_names2", g.block_names()[1]);
    for (int i = 0; i < g.size1(); ++i)
     for (int j = 0; j < g.size2(); ++j)
      h5_write(gr, g.block_names()[0][i] + "_" + g.block_names()[1][j], g._glist[i][j]);
    // mako %endif
   }

   /// Read from HDF5
   friend void h5_read(h5::group fg, std::string const& subgroup_name, MAKO_GF& g) {
    auto gr = fg.open_group(subgroup_name);
    // Check the attribute or throw
    auto tag_file     = gr.read_hdf5_scheme();
    auto tag_expected = MAKO_GF::hdf5_scheme(); 
    if (tag_file != tag_expected)
     TRIQS_RUNTIME_ERROR << "h5_read : mismatch of the tag TRIQS_HDF5_data_scheme tag in the h5 group : found " << tag_file
                         << " while I expected " << tag_expected;
    // mako %if ARITY == 1 :
    auto block_names = h5::h5_read<std::vector<std::string>>(gr, "block_names");
    int s            = block_names.size();
    g._glist.resize(s);
    g._block_names = block_names;
    for (int i = 0; i < s; ++i)
     h5_read(gr, block_names[i], g._glist[i]);
    // mako %else:
    auto block_names1 = h5::h5_read<std::vector<std::string>>(gr, "block_names1");
    auto block_names2 = h5::h5_read<std::vector<std::string>>(gr, "block_names2");
    auto block_names  = std::vector<std::vector<std::string>>{block_names1, block_names2};
    int s0            = block_names[0].size();
    int s1            = block_names[1].size();
    g._glist.resize(s0);
    g._block_names = block_names;
    for (int i = 0; i < s0; ++i) {
     g._glist[i].resize(s1);
     for (int j = 0; j < s1; ++j)
      h5_read(gr, block_names[0][i] + "_" + block_names[1][j], g._glist[i][j]);
    }
    // mako %endif
   }

   //-----------------------------  BOOST Serialization -----------------------------
   friend class boost::serialization::access;
   /// The serialization as required by Boost
   template <class Archive> void serialize(Archive& ar, const unsigned int version) {
    ar& _glist;
    ar& _block_names;
    ar& name;
   }

   //----------------------------- print  -----------------------------

   /// IO
   friend std::ostream& operator<<(std::ostream& out, MAKO_GF const& x) { return out << "MAKO_GF"; }

   // -------------------------------  iterator  --------------------------------------------------

   template<bool is_const>
   class iterator_impl {
    std::conditional_t<is_const, const MAKO_GF*, MAKO_GF*> bgf = NULL;
    int n;

    public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = g_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = std::conditional_t<is_const, const g_t*, g_t*>;
    using reference         = std::conditional_t<is_const, g_t const&, g_t&>;
    using block_gf_ref      = std::conditional_t<is_const, MAKO_GF const&, MAKO_GF&>; 

    iterator_impl() = default;
    iterator_impl( block_gf_ref _bgf, bool at_end = false) : bgf(&_bgf), n(at_end ? bgf->size() : 0) {}
    iterator_impl( block_gf_ref _bgf, int _n) : bgf(&_bgf), n(_n) {}

    operator iterator_impl<true>() const{ return iterator_impl<true>(*bgf, n); } 

    // mako %if ARITY == 1 :
    reference operator*() { return (*bgf)[n]; }
    reference operator*() const { return (*bgf)[n]; }
    // mako %else :
    reference operator*() { return (*bgf)(n / bgf->size2(), n % bgf->size2()); }
    reference operator*() const { return (*bgf)(n / bgf->size2(), n % bgf->size2()); }
    // mako %endif
    reference operator->() { return operator*(); }

    iterator_impl& operator++() {
     ++n;
     return *this;
    }

    iterator_impl operator++(int) {
     auto it = *this;
     ++n;
     return it;
    }

    bool operator==(iterator_impl const& other) const { return ((bgf == other.bgf) && (n == other.n)); }
    bool operator!=(iterator_impl const& other) const { return (!operator==(other)); }
   };

   using iterator = iterator_impl<false>; 
   using const_iterator = iterator_impl<true>; 

   //------------

   iterator begin() { return {*this, false}; }
   const_iterator begin() const { return {*this, false}; }
   iterator end() { return {*this, true}; }
   const_iterator end() const { return {*this, true}; }
   auto cbegin() { return const_view_type(*this).begin(); }
   auto cend() { return const_view_type(*this).end(); }

  };

  //----------------------------- MPI  -----------------------------

  // mako <%def name="mpidoc(OP)">
  /**
    * Initiate (lazy) MPI ${OP}
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI ${OP} operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    *
    */
  // mako </%def>

  // mako ${mpidoc("Bcast")}
  template <typename V, typename T> void mpi_broadcast(MAKO_GF<V, T>& g, mpi::communicator c = {}, int root = 0) {
   // Shall we bcast mesh ?
   mpi_broadcast(g.data(), c, root);
  }

  // mako ${mpidoc("Reduce")}
  template <typename V, typename T>
  mpi_lazy<mpi::tag::reduce, MAKO_ROOT_const_view<V, T>> mpi_reduce(MAKO_GF<V, T> const& a, mpi::communicator c = {},
                                                                    int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
   return {a(), c, root, all, op};
  }

  // mako ${mpidoc("AllReduce")}
  template <typename V, typename T>
  mpi_lazy<mpi::tag::reduce, MAKO_ROOT_const_view<V, T>> mpi_all_reduce(MAKO_GF<V, T> const& a, mpi::communicator c = {},
                                                                        int root = 0, MPI_Op op = MPI_SUM) {
   return {a(), c, root, true, op};
  }

  // mako %endfor
  // mako %endfor

  // -------------------------------   Free Factories for regular type  --------------------------------------------------

  ///
  template <typename V, typename T> block_gf<V, T> make_block_gf(int n, gf<V, T> const& g) { return {n, g}; }

  ///
  template <typename V, typename T> block_gf<V, T> make_block_gf(std::vector<gf<V, T>> v) { return {std::move(v)}; }

  ///
  template <typename V, typename T> block_gf<V, T> make_block_gf(std::initializer_list<gf<V, T>> const& v) { return {v}; }

  ///
  template <typename V, typename T> block_gf<V, T> make_block_gf(std::vector<std::string> const& b, gf<V, T> const& g) {
   return {b, g};
  }

  ///
  template <typename V, typename T> block_gf<V, T> make_block_gf(std::vector<std::string> const& b, std::vector<gf<V, T>> v) {
   return {b, std::move(v)};
  }

  ///
  template <typename V, typename T>
  block_gf<V, T> make_block_gf(std::vector<std::string> b, std::initializer_list<gf<V, T>> const& v) {
   return {b, v};
  }

  // -------------------------------   Free Factories for block_gf_view and block_gf_const_view
  // --------------------------------------------------

  // mako %for VIEW in ['view', 'const_view']:
  /// Make a block MAKO_VIEW from the G. Indices are '0', '1', '2', ....
  template <typename G0, typename... G> block_gf_MAKO_VIEW_of<G0> make_block_gf_MAKO_VIEW(G0&& g0, G&&... g) {
   return {details::_make_block_names1(sizeof...(G) + 1), {std::forward<G0>(g0), std::forward<G>(g)...}};
  }

  // Create block_gf_MAKO_VIEW from vector of views
  template <typename Gf> block_gf_MAKO_VIEW_of<Gf> make_block_gf_MAKO_VIEW(std::vector<Gf> & v) {
   static_assert(Gf::is_view);
   return {details::_make_block_names1(v.size()), v};
  }
  template <typename Gf> block_gf_MAKO_VIEW_of<Gf> make_block_gf_MAKO_VIEW(std::vector<Gf> && v) {
   static_assert(Gf::is_view);
   return {details::_make_block_names1(v.size()), std::move(v)};
  }

  // Create block_gf_MAKO_VIEW from block_names and vector of views
  template <typename Gf> block_gf_MAKO_VIEW_of<Gf> make_block_gf_MAKO_VIEW(std::vector<std::string> b, std::vector<Gf> & v) {
   static_assert(Gf::is_view);
   return {std::move(b), v};
  }
  template <typename Gf> block_gf_MAKO_VIEW_of<Gf> make_block_gf_MAKO_VIEW(std::vector<std::string> b, std::vector<Gf> && v) {
   static_assert(Gf::is_view);
   return {std::move(b), std::move(v)};
  }
  // mako %endfor

  // -------------------------------   Free Factories for block2_gf   --------------------------------------------------

  /// From the size n x p and the g from a number and a gf to be copied
  template <typename V, typename T> block2_gf<V, T> make_block2_gf(int n, int p, gf<V, T> const& g) { return {n, p, g}; }

  // from vector<tuple<string,string>>, vector<gf>
  template <typename V, typename T>
  block2_gf<V, T> make_block2_gf(std::vector<std::string> const& block_names1, std::vector<std::string> const& block_names2,
                                 std::vector<std::vector<gf<V, T>>> vv) {
   if (block_names1.size() != vv.size())
    TRIQS_RUNTIME_ERROR << "make_block2_gf(vector<string>, vector<string>>, vector<vector<gf>>): incompatible outer vector size!";
   for (auto const& v : vv) {
    if (block_names2.size() != v.size())
     TRIQS_RUNTIME_ERROR
         << "make_block2_gf(vector<string>, vector<string>>, vector<vector<gf>>): incompatible inner vector size!";
   }
   return {{block_names1, block_names2}, std::move(vv)};
  }

  // -------------------------------   Free Factories for block2_gf_view and block2_gf_const_view  --------------------------------------------------

  // mako %for VIEW in ['view', 'const_view']:

  // Create block2_gf_MAKO_VIEW from vector of views
  template <typename Gf>
  block2_gf_MAKO_VIEW_of<Gf> make_block2_gf_MAKO_VIEW(std::vector<std::vector<Gf>> & v) {
   static_assert(Gf::is_view);
   if(v.size() == 0) return {details::_make_block_names2(0,0),v};
   return {details::_make_block_names2(v.size(), v[0].size()),v};
  }
  template <typename Gf>
  block2_gf_MAKO_VIEW_of<Gf> make_block2_gf_MAKO_VIEW(std::vector<std::vector<Gf>> && v) {
   static_assert(Gf::is_view);
   if(v.size() == 0) return {details::_make_block_names2(0,0),v};
   return {details::_make_block_names2(v.size(), v[0].size()),std::move(v)};
  }

  // Create block2_gf_MAKO_VIEW from block_names and vector of views
  template <typename Gf>
  block2_gf_MAKO_VIEW_of<Gf> make_block2_gf_MAKO_VIEW(std::vector<std::string> block_names1, std::vector<std::string> block_names2, std::vector<std::vector<Gf>> & v) {
   static_assert(Gf::is_view);
   return {{std::move(block_names1), std::move(block_names2)}, v};
  }
  template <typename Gf>
  block2_gf_MAKO_VIEW_of<Gf> make_block2_gf_MAKO_VIEW(std::vector<std::string> block_names1, std::vector<std::string> block_names2, std::vector<std::vector<Gf>> && v) {
   static_assert(Gf::is_view);
   return {{std::move(block_names1), std::move(block_names2)}, std::move(v)};
  }
  // mako %endfor
 }
}

/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
 template <typename Var, typename Target>
 void swap(triqs::gfs::block_gf_view<Var, Target>& a, triqs::gfs::block_gf_view<Var, Target>& b) = delete;
 template <typename Var, typename Target>
 void swap(triqs::gfs::block2_gf_view<Var, Target>& a, triqs::gfs::block2_gf_view<Var, Target>& b) = delete;
 template <typename Var, typename Target>
 void swap(triqs::gfs::block_gf_const_view<Var, Target>& a, triqs::gfs::block_gf_const_view<Var, Target>& b) = delete;
 template <typename Var, typename Target>
 void swap(triqs::gfs::block2_gf_const_view<Var, Target>& a, triqs::gfs::block2_gf_const_view<Var, Target>& b) = delete;
}
