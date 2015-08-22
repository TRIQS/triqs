/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
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
#include "./defs.hpp"

namespace triqs {
namespace gfs {

 template <typename Mesh, typename Target, typename Singularity, typename Evaluator>
 class gf_view : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {

  public:
  using mutable_view_type = gf_view<Mesh, Target, Singularity, Evaluator>;
  using const_view_type = gf_const_view<Mesh, Target, Singularity, Evaluator>;
  using view_type = gf_view<Mesh, Target, Singularity, Evaluator>;
  using regular_type = gf<Mesh, Target, Singularity, Evaluator>;

  using variable_t = Mesh;
  using target_t = Target;

  using mesh_t = gf_mesh<Mesh>;
  using domain_t = typename mesh_t::domain_t;
  using mesh_point_t = typename mesh_t::mesh_point_t;
  using mesh_index_t = typename mesh_t::index_t;
  using linear_mesh_index_t = typename mesh_t::linear_index_t;
  using symmetry_t = typename gf_symmetry<Mesh, Target, Singularity>::type;
  using indices_t = typename gf_indices<Target>::type;
  using evaluator_t = std14::conditional_t<std::is_same<Evaluator, void>::value,
                                           gf_evaluator<Mesh, Target, Singularity>, Evaluator>;

  using data_proxy_t = gf_data_proxy<Mesh, Target>;
  using data_regular_t = typename data_proxy_t::storage_t;
  using data_view_t = typename data_proxy_t::storage_view_t;
  using data_const_view_t = typename data_proxy_t::storage_const_view_t;
  using data_t = data_view_t;

  using _singularity_regular_t = typename Singularity::regular_type;
  using _singularity_view_t = typename Singularity::view_type;
  using _singularity_const_view_t = typename Singularity::const_view_type;
  using singularity_t = _singularity_view_t;

  static constexpr bool is_view = true;
  static constexpr bool is_const = false;

  // ------------- Accessors -----------------------------

  mesh_t const &mesh() const { return _mesh; }
  domain_t const &domain() const { return _mesh.domain(); }
  data_t &data() { return _data; }
  data_t const &data() const { return _data; }
  singularity_t &singularity() { return _singularity; }
  singularity_t const &singularity() const { return _singularity; }
  symmetry_t const &symmetry() const { return _symmetry; }
  indices_t const &indices() const { return _indices; }
  evaluator_t const &get_evaluator() const { return _evaluator; }

  private:
  mesh_t _mesh;
  data_t _data;
  singularity_t _singularity;
  symmetry_t _symmetry;
  indices_t _indices;
  public :
  std::string name;
  private :
  data_proxy_t _data_proxy;
  evaluator_t _evaluator;

  // -------------------------------- impl. details common to all classes -----------------------------------------------

  // for delegation only
  private:
  template <typename G>
  gf_view(G &&x, bool) // bool to disambiguate
      : _mesh(x.mesh()),
        _data(factory<data_t>(x.data())),
        _singularity(factory<singularity_t>(x.singularity())),
        _symmetry(x.symmetry()),
        _indices(x.indices()),
        name(x.name),
        _evaluator(this) {}

  struct impl_tag{};

  template <typename M, typename D, typename S, typename SY>
  gf_view(impl_tag, M &&m, D &&dat, S &&sing, SY &&sy, indices_t ind, std::string name)
     : _mesh(std::forward<M>(m))
     , _data(std::forward<D>(dat))
     , _singularity(std::forward<S>(sing))
     , _symmetry(std::forward<SY>(sy))
     , _indices(std::move(ind))
     , name(name)
     , _evaluator(this) {
   if (!_indices.check_size(this)) TRIQS_RUNTIME_ERROR << "Size of indices mismatch with data size";
  }

  public:

  /// Copy
  gf_view(gf_view const &x)
     : _mesh(x.mesh()),
       _data(factory<data_t>(x.data())),
       _singularity(factory<singularity_t>(x.singularity())),
       _symmetry(x.symmetry()),
       _indices(x.indices()),
       name(x.name),
       _evaluator(this) {}

  /// Move
  gf_view(gf_view &&) = default; // TO BE REWRITTEN ??

  void swap_impl(gf_view &b) noexcept {
   using std::swap;
   swap(this->_mesh, b._mesh);
   swap(this->_data, b._data);
   swap(this->_singularity, b._singularity);
   swap(this->_symmetry, b._symmetry);
   swap(this->_indices, b._indices);
   swap(this->name, b.name);
   swap(this->_evaluator, b._evaluator);
  }


 /// ---------------  Constructors --------------------

  // views can not be default constructed
  gf_view() = delete;

  // Allow to construct a view from a gf with a different evaluator, except const_views ...
  template <typename Ev2> gf_view(gf_const_view<Mesh, Target, Singularity, Ev2> const &g) = delete;          // from a const_view
  template <typename Ev2> gf_view(gf_view<Mesh, Target, Singularity, Ev2> const &g) : gf_view(g, bool{}) {}  // from a view
  template <typename Ev2> gf_view(gf<Mesh, Target, Singularity, Ev2> const &g) = delete;                     // from a const gf
  template <typename Ev2> gf_view(gf<Mesh, Target, Singularity, Ev2> &g) : gf_view(g, bool{}) {}             // from a gf &
  template <typename Ev2> gf_view(gf<Mesh, Target, Singularity, Ev2> &&g) noexcept : gf_view(std::move(g), bool{}) {} // from a gf &&

  /// Construct from mesh, data, ....
  template <typename D>
  gf_view(mesh_t const &m, D &&dat, singularity_t const &t, symmetry_t const &s, indices_t const &ind = indices_t{},
          std::string name = "")
     : gf_view(impl_tag{}, m, factory<data_t>(std::forward<D>(dat)), t, s, ind, name) {}

  /// ---------------  swap --------------------

  friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

  /// ---------------  Rebind --------------------
  //
  template <typename Ev2> void rebind(gf_view<Mesh, Target, Singularity, Ev2> const &X) noexcept {
   this->_mesh = X._mesh;
   this->_symmetry = X._symmetry;
   this->_data_proxy.rebind(this->_data, X);
   this->_singularity.rebind(X._singularity);
   this->_indices = X._indices;
   this->name = X.name;
   this->_evaluator = evaluator_t{this}; // reconstruct the evaluator in case it has some state...
  }

  /// ---------------  operator =  --------------------
  gf_view &operator=(gf_view const &rhs) {
   triqs_gf_view_assign_delegation(*this, rhs);
   return *this;
  }

  template <typename RHS> gf_view &operator=(RHS const &rhs) {
   triqs_gf_view_assign_delegation(*this, rhs);
   return *this;
  }


  public:
  // ------------- All the call operators without lazy arguments -----------------------------

  // First, a simple () returns a view, like for an array...
  const_view_type operator()() const { return *this; }
  view_type operator()() { return *this; }

  /// Calls are (perfectly) forwarded to the evaluator::operator(), except mesh_point_t and when
  /// there is at least one lazy argument ...
  template <typename... Args> // match any argument list, picking out the first type : () is not permitted
  typename boost::lazy_disable_if_c< // disable the template if one the following conditions it true
      (sizeof...(Args) == 0) || clef::is_any_lazy<Args...>::value ||
          ((sizeof...(Args) != evaluator_t::arity) && (evaluator_t::arity != -1)) // if -1 : no check
      ,
      std::result_of<evaluator_t(gf_view, Args...)> // what is the result type of call
      >::type                                         // end of lazy_disable_if
  operator()(Args &&... args) const {
   return _evaluator(*this, std::forward<Args>(args)...);
  }

  // ------------- Call with lazy arguments -----------------------------

  /// Calls with at least one lazy argument : we make a clef expression, cf clef documentation
  template <typename... Args> clef::make_expr_call_t<gf_view &, Args...> operator()(Args &&... args) & {
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args>
  clef::make_expr_call_t<gf_view const &, Args...> operator()(Args &&... args) const &{
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args> clef::make_expr_call_t<gf_view, Args...> operator()(Args &&... args) && {
   return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
  }

  // ------------- All the [] operators without lazy arguments -----------------------------

  //  [] and access to the grid point
  using r_type = std14::result_of_t<data_proxy_t(data_t &, typename mesh_t::linear_index_t)>;
  using cr_type = std14::result_of_t<data_proxy_t(data_t const &, typename mesh_t::linear_index_t)>;

  /// pass a index_t of the mesh
  r_type operator[](mesh_index_t const &arg) { return _data_proxy(_data, _mesh.index_to_linear(arg)); }
  cr_type operator[](mesh_index_t const &arg) const { return _data_proxy(_data, _mesh.index_to_linear(arg)); }

  /// pass a mesh_point of the mesh
  r_type operator[](mesh_point_t const &x) { return _data_proxy(_data, x.linear_index()); }
  cr_type operator[](mesh_point_t const &x) const { return _data_proxy(_data, x.linear_index()); }

  /// pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait
  template <typename... U> r_type operator[](closest_pt_wrap<U...> const &p) {
   return _data_proxy(
       _data,
       _mesh.index_to_linear(gf_closest_point<Mesh, Target, Singularity, Evaluator>::invoke(this, p)));
  }
  template <typename... U> cr_type operator[](closest_pt_wrap<U...> const &p) const {
   return _data_proxy(
       _data,
       _mesh.index_to_linear(gf_closest_point<Mesh, Target, Singularity, Evaluator>::invoke(this, p)));
  }

  // ------------- [] with lazy arguments -----------------------------

  template <typename Arg>
  clef::make_expr_subscript_t<gf_view const &, Arg> operator[](Arg &&arg) const &{
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<gf_view &, Arg> operator[](Arg &&arg) & {
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<gf_view, Arg> operator[](Arg &&arg) && {
   return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
  }

  /// --------------------- A direct access to the grid point --------------------------

  template <typename... Args> r_type get_from_linear_index(Args &&... args) {
   return _data_proxy(_data, linear_mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> cr_type get_from_linear_index(Args &&... args) const {
   return _data_proxy(_data, linear_mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> r_type on_mesh(Args &&... args) {
   return _data_proxy(_data, _mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
  }

  template <typename... Args> cr_type on_mesh(Args &&... args) const {
   return _data_proxy(_data, _mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
  }

  /// --------------------- on mesh (g) : the call before [] -------------------------
  // This is a workaround the the lack of multi argument [] in C++

  // mesh points should be treated slighly differently : take their index....
  template <typename... T> r_type on_mesh(mesh_point<T> const &... args) { return on_mesh(args.index()...); }
  template <typename... T> cr_type on_mesh(mesh_point<T> const &... args) const { return on_mesh(args.index()...); }

  // The on_mesh little adaptor ....
  private:
  struct _on_mesh_wrapper_const {
   gf_view const &f;
   template <typename... Args>
    //requires(triqs::clef::is_any_lazy<Args...>)
    //cr_type
   std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, cr_type> operator()(Args &&... args) const {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };
  struct _on_mesh_wrapper {
   gf_view &f;
   template <typename... Args>
   std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, r_type> operator()(Args &&... args) const {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };

  public:
  _on_mesh_wrapper_const friend on_mesh(gf_view const &f) {
   return {f};
  }
  _on_mesh_wrapper friend on_mesh(gf_view &f) {
   return {f};
  }

  //----------------------------- HDF5 -----------------------------

  friend std::string get_triqs_hdf5_data_scheme(gf_view const &g) {
   auto s = gf_h5_name<Mesh, Target, Singularity>::invoke();
   return (s == "BlockGf" ? s : "Gf" + s);
  }

  friend struct gf_h5_rw<Mesh, Target, Singularity, Evaluator>;

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, gf_view const &g) {
   auto gr = fg.create_group(subgroup_name);
   gr.write_triqs_hdf5_data_scheme(g);
   gf_h5_rw<Mesh, Target, Singularity, Evaluator>::write(gr, g);
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, gf_view &g) {
   auto gr = fg.open_group(subgroup_name);
   // Check the attribute or throw
   auto tag_file = gr.read_triqs_hdf5_data_scheme();
   auto tag_expected = get_triqs_hdf5_data_scheme(g);
   if (tag_file != tag_expected)
    TRIQS_RUNTIME_ERROR << "h5_read : mismatch of the tag TRIQS_HDF5_data_scheme tag in the h5 group : found " << tag_file
                        << " while I expected " << tag_expected;
   gf_h5_rw<Mesh, Target, Singularity, Evaluator>::read(gr, g);
  }

  //-----------------------------  BOOST Serialization -----------------------------
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &TRIQS_MAKE_NVP("data", _data);
   ar &TRIQS_MAKE_NVP("singularity", _singularity);
   ar &TRIQS_MAKE_NVP("mesh", _mesh);
   ar &TRIQS_MAKE_NVP("symmetry", _symmetry);
   ar &TRIQS_MAKE_NVP("indices", _indices);
   ar &TRIQS_MAKE_NVP("name", name);
  }

  //----------------------------- print  -----------------------------

  friend std::ostream &operator<<(std::ostream &out, gf_view const &x) { return out << "gf_view"; }

  //----------------------------- MPI  -----------------------------

  friend void mpi_broadcast(gf_view &g, mpi::communicator c = {}, int root = 0) {
   // Shall we bcast mesh ?
   mpi_broadcast(g.data(), c, root);
   mpi_broadcast(g.singularity(), c, root);
  }

  friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_reduce(gf_view const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   return {a(), c, root, all};
  }
  friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_all_reduce(gf_view const &a, mpi::communicator c = {}, int root = 0) {
   return {a(), c, root, true};
  }
  friend mpi_lazy<mpi::tag::scatter, const_view_type> mpi_scatter(gf_view const &a, mpi::communicator c = {}, int root = 0) {
   return {a(), c, root, true};
  }
  friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_gather(gf_view const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   return {a(), c, root, all};
  }
  friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_all_gather(gf_view const &a, mpi::communicator c = {}, int root = 0) {
   return {a(), c, root, true};
  }


  //---- reduce ----
  template <typename E>
  void operator=(mpi_lazy<mpi::tag::reduce, gf_const_view<Mesh, Target, Singularity, E>> l) {
   _mesh = l.rhs.mesh();
   _data = mpi_reduce(l.rhs.data(), l.c, l.root, l.all);
   _singularity = mpi_reduce(l.rhs.singularity(), l.c, l.root, l.all);
  }

  //---- scatter ----
  template <typename E>
  void operator=(mpi_lazy<mpi::tag::scatter, gf_const_view<Mesh, Target, Singularity, E>> l) {
   _mesh = mpi_scatter(l.rhs.mesh(), l.c, l.root);
   _data = mpi_scatter(l.rhs.data(), l.c, l.root, true);
   if (l.c.rank() == l.root) _singularity = l.rhs.singularity();
   mpi_broadcast(_singularity, l.c, l.root);
  }

  //---- gather ----
  template <typename E>
  void operator=(mpi_lazy<mpi::tag::gather, gf_const_view<Mesh, Target, Singularity, E>> l) {
   _mesh = mpi_gather(l.rhs.mesh(), l.c, l.root);
   _data = mpi_gather(l.rhs.data(), l.c, l.root, l.all);
   if (l.all || (l.c.rank() == l.root)) _singularity = l.rhs.singularity();
  }

 };

}}