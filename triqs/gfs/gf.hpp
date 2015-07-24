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
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/factory.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/c14.hpp>
#include <triqs/arrays/h5.hpp>
#include "./impl/gf_indices.hpp"
#include "./impl/tools.hpp"
#include "./singularity/tail.hpp"
#include "./singularity/nothing.hpp"
#include "./singularity/tail_zero.hpp"
#include "./impl/data_proxies.hpp"
#include <triqs/mpi/vector.hpp>
#include <vector>

namespace triqs {
namespace gfs {
 using utility::factory;
 using arrays::make_shape;
 using arrays::array;
 using arrays::array_view;
 using arrays::matrix;
 using arrays::matrix_view;
 using arrays::matrix_const_view;
 using triqs::make_clone;

 /*----------------------------------------------------------
  *   Declarations related to the mesh
  *--------------------------------------------------------*/   
 
// Interpolation policies
 namespace interpol_t {
  struct None{};
  struct Product{};
  struct Linear1d{};
  struct Linear2d{};
 }

 // The mesh for each Mesh
 template <typename Mesh> struct gf_mesh;

 // The mesh point for each mesh
 template<typename MeshType> struct mesh_point;
 
 /*----------------------------------------------------------
  *   Default Target, Singularity
  *--------------------------------------------------------*/   
 
 template <typename Mesh> struct gf_default_target {
  using type = matrix_valued;
 };
 template <typename M> using gf_default_target_t = typename gf_default_target<M>::type;

 template <typename Mesh, typename Target> struct gf_default_singularity {
  using type = nothing;
 };
 template <typename M, typename T>
 using gf_default_singularity_t = typename gf_default_singularity<M, T>::type;

 /*----------------------------------------------------------
  *   Declaration of main types : gf, gf_view, gf_const_view
  *--------------------------------------------------------*/   
  // The regular type
 template <typename Mesh, typename Target = gf_default_target_t<Mesh>,
           typename Singularity = gf_default_singularity_t<Mesh, Target>, typename Evaluator = void>
 class gf;

 // The view type
 template <typename Mesh, typename Target = gf_default_target_t<Mesh>,
           typename Singularity = gf_default_singularity_t<Mesh, Target>, typename Evaluator = void, bool IsConst = false>
 class gf_view;

 // The const view type
 template <typename Mesh, typename Target = gf_default_target_t<Mesh>,
           typename Singularity = gf_default_singularity_t<Mesh, Target>, typename Evaluator = void>
 using gf_const_view = gf_view<Mesh, Target, Singularity, Evaluator, true>;

 // the implementation class : forward
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst> class gf_impl;

 /*----------------------------------------------------------
  *   Useful metafunctions, traits
  *--------------------------------------------------------*/   

 // Get the number of variables
 template<typename Mesh> struct get_n_variables { static const int value = 1;};

 /*----------------------------------------------------------
  *   Traits 
  *--------------------------------------------------------*/   

 // The trait that "marks" the Green function
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableGreenFunction);

 // Is G a gf, gf_view, gf_const_view
 template<typename G> struct is_gf_or_view : std::false_type{};
 template <typename M, typename T, typename S, typename E> struct is_gf_or_view<gf<M, T, S, E>> : std::true_type {};
 template <typename M, typename T, typename S, typename E, bool C>
 struct is_gf_or_view<gf_view<M, T, S, E, C>> : std::true_type {};

 /*----------------------------------------------------------
  *  Evaluator
  *--------------------------------------------------------*/   

 // gf_evaluator regroup functions to evaluate the function.
 // default : one variable. Will be specialized in more complex cases.
 template <typename Mesh, typename Target, typename Singularity> struct gf_evaluator {
  static constexpr int arity = 1;
  template <typename G> gf_evaluator(G *) {};

  template <typename G, typename X>
  auto operator()(G const &g, X x) const RETURN((g.mesh().evaluate(typename G::mesh_t::default_interpol_policy{}, g, x)));

  template <typename G> typename G::singularity_t operator()(G const &g, tail_view t) const {
   return compose(g.singularity(), t);
  }
 };

 /*----------------------------------------------------------
  *  closest_point mechanism
  *  This trait will contain the specialisation to get 
  *  the closest point ...
  *--------------------------------------------------------*/   

 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_closest_point;

 /*----------------------------------------------------------
  *  symmetry
  *--------------------------------------------------------*/   

 template <typename Mesh, typename Target, typename Singularity> struct gf_symmetry {
  using type = nothing;
 };

 /*----------------------------------------------------------
  *  indices
  *--------------------------------------------------------*/   

 template <typename Target> struct gf_indices {
  using type = nothing;
 };

 template <> struct gf_indices<matrix_valued> {
  using type = gf_indices_pair;
 };

 /*----------------------------------------------------------
  *  data proxy
  *--------------------------------------------------------*/   

 // gf_data_proxy contains function to manipulate the data array, but no data itself.
 // this is used to specialize this part of the code to array of dim 3 (matrix gf), 
 // dim 1 (scalar gf) and vector (e.g. block gf, ...)
 template <typename Mesh, typename Target, typename Enable = void> struct gf_data_proxy;

 template <typename M> struct gf_data_proxy<M, matrix_valued> : data_proxy_array<dcomplex, 3> {};
 template <typename M> struct gf_data_proxy<M, scalar_valued> : data_proxy_array<dcomplex, 1> {};

 /*----------------------------------------------------------
  *  HDF5
  *  Traits to read/write in hdf5 files. 
  *  Can be specialized for some case (Cf block). Defined below
  *--------------------------------------------------------*/   

 template <typename Mesh, typename Target, typename Singularity> struct gf_h5_name;
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_h5_rw;

 /*----------------------------------------------------------
  *  Factories for data and singularity
  *  Defaults implemented below.
  *--------------------------------------------------------*/   
 
 template <typename Mesh, typename Target, typename Singularity> struct gf_data_factory;
 template <typename Mesh, typename Target, typename Singularity> struct gf_singularity_factory;

 /*----------------------------------------------------------
  *  Get shape of the data or of the target
  *--------------------------------------------------------*/   

 template <typename M, typename T, typename S, typename E, bool IsView, bool IsConst>
 auto get_gf_data_shape(gf_impl<M,T,S,E, IsView, IsConst> const &g) RETURN(get_shape(g.data()));

 template <typename M, typename T, typename S, typename E, bool IsView, bool IsConst>
 auto get_target_shape(gf_impl<M,T,S,E, IsView, IsConst> const &g)
     DECL_AND_RETURN(g.data().shape().front_pop());

 // overload get_shape for a vector to simplify code below in gf block case.
 template <typename T> long get_shape(std::vector<T> const &x) { return x.size(); }

 /*----------------------------------------------------------
  *  implementation class
  *--------------------------------------------------------*/   

 /// A common implementation class for gf and gf_view. They will only redefine contructor and = ...
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
 class gf_impl : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {
  static_assert(!(!IsView && IsConst), "Internal error");

  public:
  using mutable_view_type = gf_view<Mesh, Target, Singularity, Evaluator>;
  using const_view_type = gf_const_view<Mesh, Target, Singularity, Evaluator>;
  using view_type = typename std::conditional<IsConst, const_view_type, mutable_view_type>::type;
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
  using data_t = std14::conditional_t<IsView, std14::conditional_t<IsConst, data_const_view_t, data_view_t>, data_regular_t>;

  using _singularity_regular_t = typename Singularity::regular_type;
  using _singularity_view_t = typename Singularity::view_type;
  using _singularity_const_view_t = typename Singularity::const_view_type;
  using singularity_t = std14::conditional_t<IsView, std14::conditional_t<IsConst, _singularity_const_view_t, _singularity_view_t>,
                                             _singularity_regular_t>;

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
 
  protected:
  mesh_t _mesh;
  data_t _data;
  singularity_t _singularity;
  symmetry_t _symmetry;
  indices_t _indices;
  public : 
  std::string name;
  protected : 
  data_proxy_t _data_proxy;
  evaluator_t _evaluator;

  // --------------------------------Constructors -----------------------------------------------
  // all protected but one, this is an implementation class, see gf/gf_view later for public one
  gf_impl() : _evaluator(this) {} // all arrays of zero size (empty)

  public: // everyone can make a copy and a move (for clef lib in particular, this one needs to be public)
  gf_impl(gf_impl const &x)
     : _mesh(x.mesh()),
       _data(factory<data_t>(x.data())),
       _singularity(factory<singularity_t>(x.singularity())),
       _symmetry(x.symmetry()),
       _indices(x.indices()),
       name(x.name),
       _evaluator(this) {}

  gf_impl(gf_impl &&) = default;

  protected:
  template <typename G>
  gf_impl(G &&x, bool) // bool to disambiguate
      : _mesh(x.mesh()),
        _data(factory<data_t>(x.data())),
        _singularity(factory<singularity_t>(x.singularity())),
        _symmetry(x.symmetry()),
        _indices(x.indices()),
        name(x.name),
        _evaluator(this) {}

  template <typename M, typename D, typename S, typename SY>
  gf_impl(M &&m, D &&dat, S &&sing, SY &&sy, indices_t ind, std::string name)
     : _mesh(std::forward<M>(m))
     , _data(std::forward<D>(dat))
     , _singularity(std::forward<S>(sing))
     , _symmetry(std::forward<SY>(sy))
     , _indices(std::move(ind))
     , name(name)
     , _evaluator(this) {
   if (!_indices.check_size(this)) TRIQS_RUNTIME_ERROR << "Size of indices mismatch with data size";
  }

  void operator=(gf_impl const &rhs) = delete; // done in derived class.

  void swap_impl(gf_impl &b) noexcept {
   using std::swap;
   swap(this->_mesh, b._mesh);
   swap(this->_data, b._data);
   swap(this->_singularity, b._singularity);
   swap(this->_symmetry, b._symmetry);
   swap(this->_indices, b._indices);
   swap(this->name, b.name);
   swap(this->_evaluator, b._evaluator);
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
      std::result_of<evaluator_t(gf_impl, Args...)> // what is the result type of call
      >::type                                         // end of lazy_disable_if
  operator()(Args &&... args) const {
   return _evaluator(*this, std::forward<Args>(args)...);
  }

  // ------------- Call with lazy arguments -----------------------------
 
  /// Calls with at least one lazy argument : we make a clef expression, cf clef documentation
  template <typename... Args> clef::make_expr_call_t<gf_impl &, Args...> operator()(Args &&... args) & {
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args>
  clef::make_expr_call_t<gf_impl const &, Args...> operator()(Args &&... args) const &{
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args> clef::make_expr_call_t<gf_impl, Args...> operator()(Args &&... args) && {
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
  clef::make_expr_subscript_t<gf_impl const &, Arg> operator[](Arg &&arg) const &{
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<gf_impl &, Arg> operator[](Arg &&arg) & {
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<gf_impl, Arg> operator[](Arg &&arg) && {
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
   gf_impl const &f;
   template <typename... Args>
    //requires(triqs::clef::is_any_lazy<Args...>)
    //cr_type
   std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, cr_type> operator()(Args &&... args) const {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };
  struct _on_mesh_wrapper {
   gf_impl &f;
   template <typename... Args>
   std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, r_type> operator()(Args &&... args) const {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };

  public:
  _on_mesh_wrapper_const friend on_mesh(gf_impl const &f) {
   return {f};
  }
  _on_mesh_wrapper friend on_mesh(gf_impl &f) {
   return {f};
  }

  //----------------------------- HDF5 -----------------------------

  friend std::string get_triqs_hdf5_data_scheme(gf_impl const &g) {
   auto s = gf_h5_name<Mesh, Target, Singularity>::invoke();
   return (s == "BlockGf" ? s : "Gf" + s);
  }

  friend struct gf_h5_rw<Mesh, Target, Singularity, Evaluator>;

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, gf_impl const &g) {
   auto gr = fg.create_group(subgroup_name);
   gr.write_triqs_hdf5_data_scheme(g);
   gf_h5_rw<Mesh, Target, Singularity, Evaluator>::write(gr, g);
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, gf_impl &g) {
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

  friend std::ostream &operator<<(std::ostream &out, gf_impl const &x) { return out << (IsView ? "gf_view" : "gf"); }
  friend std::ostream &triqs_nvl_formal_print(std::ostream &out, gf_impl const &x) { return out << (IsView ? "gf_view" : "gf"); }

  //----------------------------- MPI  -----------------------------

  friend void mpi_broadcast(gf_impl &g, mpi::communicator c = {}, int root = 0) {
   // Shall we bcast mesh ?
   mpi_broadcast(g.data(), c, root);
   mpi_broadcast(g.singularity(), c, root);
  }

  friend mpi::lazy<mpi::tag::reduce, gf_impl> mpi_reduce(gf_impl const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   return {a, c, root, all};
  }
  friend mpi::lazy<mpi::tag::reduce, gf_impl> mpi_all_reduce(gf_impl const &a, mpi::communicator c = {}, int root = 0) {
   return {a, c, root, true};
  }
  friend mpi::lazy<mpi::tag::scatter, gf_impl> mpi_scatter(gf_impl const &a, mpi::communicator c = {}, int root = 0) {
   return {a, c, root, true};
  }
  friend mpi::lazy<mpi::tag::gather, gf_impl> mpi_gather(gf_impl const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   return {a, c, root, all};
  }
  friend mpi::lazy<mpi::tag::gather, gf_impl> mpi_all_gather(gf_impl const &a, mpi::communicator c = {}, int root = 0) {
   return {a, c, root, true};
  }
  //---- reduce ----
  void operator=(mpi::lazy<mpi::tag::reduce, gf_impl> l) {
   _mesh = l.rhs._mesh;
   _data = mpi_reduce(l.rhs.data(), l.c, l.root, l.all);
   _singularity = mpi_reduce(l.rhs.singularity(), l.c, l.root, l.all);
  }

  //---- scatter ----
  void operator=(mpi::lazy<mpi::tag::scatter, gf_impl> l) {
   _mesh = mpi_scatter(l.rhs.mesh(), l.c, l.root);
   _data = mpi_scatter(l.rhs.data(), l.c, l.root, true);
   if (l.c.rank() == l.root) _singularity = l.rhs.singularity();
   mpi_broadcast(_singularity, l.c, l.root);
  }

  //---- gather ----
  void operator=(mpi::lazy<mpi::tag::gather, gf_impl> l) {
   _mesh = mpi_gather(l.rhs.mesh(), l.c, l.root);
   _data = mpi_gather(l.rhs.data(), l.c, l.root, l.all);
   if (l.all || (l.c.rank() == l.root)) _singularity = l.rhs.singularity();
  }

 };

 /*------------------------------------------------------------------------------------------------------
  *                                     The regular class 
  *-----------------------------------------------------------------------------------------------------*/

 template <typename Mesh, typename Target, typename Singularity, typename Evaluator>
 class gf : public gf_impl<Mesh, Target, Singularity, Evaluator, false, false> {
  using B = gf_impl<Mesh, Target, Singularity, Evaluator, false, false>;
  using data_factory = gf_data_factory<Mesh, Target, Singularity>;
  using singularity_factory = gf_singularity_factory<Mesh, Target, Singularity>;
  using target_shape_t = typename data_factory::target_shape_t;
  using typename B::mesh_t;
  using typename B::data_t;
  using typename B::singularity_t;
  using typename B::indices_t;
  using typename B::symmetry_t;
  using aux_t = typename data_factory::aux_t;

  public:
 
  /// ---------------  Constructors --------------------
  
  /// Empty gf (with empty array). For regular type concept.
  gf() : B() {}

  /// Copy
  gf(gf const &g) : B(g) {}

  /// Move
  gf(gf &&g) noexcept : B(std::move(g)) {}

  /// Construct from the data. Using the "pass by value" pattern + move
  gf(mesh_t m, data_t dat, singularity_t si, symmetry_t s, indices_t ind, std::string name = "")
     : B(std::move(m), std::move(dat), std::move(si), std::move(s), std::move(ind), std::move(name)) {}

  /// Construct from mesh, target_shape, aux (typically a memory order), [indices, name]
  /// aux is defined in the data_factory
  gf(mesh_t m, target_shape_t shape, aux_t aux, indices_t ind = indices_t{}, std::string name = "")
     : B(std::move(m), data_factory::make(m, shape, aux), singularity_factory::make(m, shape), symmetry_t{}, std::move(ind),
         std::move(name)) {
   if (this->_indices.is_empty()) this->_indices = indices_t(shape);
  }

  /// Construct from mesh, [target shape, indices, name]
  gf(mesh_t m, target_shape_t shape = target_shape_t{}, indices_t const &ind = indices_t{}, std::string name = "")
     : B(std::move(m), data_factory::make(m, shape, aux_t{}), singularity_factory::make(m, shape), symmetry_t{}, std::move(ind),
         std::move(name)) {
   if (this->_indices.is_empty()) this->_indices = indices_t(shape);
  }

  /// From a gf_view of the same kind
  template <typename Ev2> gf(gf_view<Mesh, Target, Singularity, Ev2> const &g) : B(g, bool {}) {}

  /// From a const_gf_view of the same kind
  template <typename Ev2> gf(gf_const_view<Mesh, Target, Singularity, Ev2> const &g) : B(g, bool {}) {}

  /// Construct from anything which models ImmutableGreenFunction.
  // TODO: We would like to refine this, G should have the same mesh, target, at least ...
  template <typename G>
  gf(G const &x, std14::enable_if_t<ImmutableGreenFunction<G>::value> *dummy = 0)
     : B() {
   *this = x;
  }
 
  /// Construct from the mpi lazy class of the implementation class, cf mpi section of gf_impl
  // NB : type must be the same, e.g. g2(mpi_reduce(g1)) will work only if mesh, Target, Singularity are the same...
  // Evaluator ?? 
  template <typename Tag> gf(mpi::lazy<Tag, B> x) : gf() { B::operator=(x); }

  /// ---------------  swap --------------------
  
  /// implement the swap
  friend void swap(gf &a, gf &b) noexcept { a.swap_impl(b); }

  /// ---------------  Operator = --------------------

  gf &operator=(gf const &rhs) { return *this = gf(rhs); } // use move =
  gf &operator=(gf &rhs) { return *this = gf(rhs); }       // use move =
  
  gf &operator=(gf &&rhs) noexcept {
   this->swap_impl(rhs);
   return *this;
  }
  
  // NB template is lower priority than the previous one when RHS is gf ...
  template <typename RHS> gf & operator=(RHS &&rhs) {
   this->_mesh = rhs.mesh();
   this->_data.resize(get_gf_data_shape(rhs));
   for (auto const &w : this->mesh()) (*this)[w] = rhs[w];
   this->_singularity = rhs.singularity();
   // to be implemented : there is none in the gf_expr in particular....
   // this->_symmetry = rhs.symmetry();
   // indices and name are not affected by it ???
   return *this;
  }
  
  template<typename Tag> gf & operator=(mpi::lazy<Tag,B> l) { B::operator =(l); return *this;}
 };

 /*------------------------------------------------------------------------------------------------------
  *                                     The const View 
  *-----------------------------------------------------------------------------------------------------*/

 template <typename Mesh, typename Target, typename Singularity, typename Evaluator>
 class gf_view<Mesh, Target, Singularity, Evaluator, true> : public gf_impl<Mesh, Target, Singularity, Evaluator, true, true> {
  using B = gf_impl<Mesh, Target, Singularity, Evaluator, true, true>;
  using typename B::mesh_t;
  using typename B::data_t;
  using typename B::singularity_t;
  using typename B::indices_t;
  using typename B::symmetry_t;
  using typename B::evaluator_t;

  public:

  /// ---------------  Constructors --------------------

  // views can not be default constructed
  gf_view() = delete;

  /// Copy
  gf_view(gf_view const &g) : B(g) {}

  /// Move (trivial here)
  gf_view(gf_view &&g) noexcept : B(std::move(g)) {}

  // Allow to construct a view from a gf with a different evaluator
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, true, true> const &g) : B(g, bool {}) {}   // from a const_view
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, true, false> const &g) : B(g, bool {}) {}  // from a view
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, false, false> const &g) : B(g, bool {}) {} // from a const gf
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, false, false> &g) : B(g, bool {}) {}       // from a gf &
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, false, false> &&g) noexcept : B(std::move(g), bool {}) {} // from a gf &&

  /// Construct from mesh, data, ....
  template <typename D>
  gf_view(mesh_t const &m, D const &dat, singularity_t const &t, symmetry_t const &s, indices_t const &ind, std::string name = "")
     : B(m, factory<data_t>(dat), t, s, ind, name) {}

  /// ---------------  Rebind --------------------
  /// Rebind the view to view X
  void rebind(gf_view const &X) noexcept {
   this->_mesh = X._mesh;
   this->_symmetry = X._symmetry;
   this->_data_proxy.rebind(this->_data, X);
   this->_singularity.rebind(X._singularity);
   this->_indices = X._indices;
   this->name = X.name;
   this->_evaluator = evaluator_t{this}; // reconstruct the evaluator in case it has some state...
  }

  /// Rebind on a const view
  void rebind(gf_view<Mesh, Target, Singularity, Evaluator, false> const &X) noexcept {
   rebind(gf_view{X});
  }

  /// ---------------  No = since it is const ... --------------------
  gf_view &operator=(gf_view const &) = delete;
 }; // class gf_const_view

 /*------------------------------------------------------------------------------------------------------
  *                                     The View 
  *-----------------------------------------------------------------------------------------------------*/

 template <typename Mesh, typename Target, typename Singularity, typename Evaluator>
 class gf_view<Mesh, Target, Singularity, Evaluator, false> : public gf_impl<Mesh, Target, Singularity, Evaluator, true,
                                                                                 false> {
  using B = gf_impl<Mesh, Target, Singularity, Evaluator, true, false>;
  using typename B::mesh_t;
  using typename B::data_t;
  using typename B::singularity_t;
  using typename B::indices_t;
  using typename B::symmetry_t;
  using typename B::evaluator_t;

  public:
 
  /// ---------------  Constructors --------------------

  // views can not be default constructed
  gf_view() = delete;
  
  /// Copy
  gf_view(gf_view const &g) : B(g) {}
  
  /// Move (trivial)
  gf_view(gf_view &&g) noexcept : B(std::move(g)) {}

  // Allow to construct a view from a gf with a different evaluator, except const_views ...
  // from a const view : impossible
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, true, true> const &g) = delete;
  // from a view
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, true, false> const &g) : B(g, bool {}) {}
  // from const gf : impossible
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, false, false> const &g) = delete;
  // from a gf &
  template <typename Ev2> gf_view(gf_impl<Mesh, Target, Singularity, Ev2, false, false> &g) : B(g, bool {}) {}
  // from a gf &&
  template <typename Ev2>
  gf_view(gf_impl<Mesh, Target, Singularity, Ev2, false, false> &&g) noexcept : B(std::move(g), bool {}) {}

  /// Construct from mesh, data, ....
  template <typename D>
  gf_view(mesh_t const &m, D &&dat, singularity_t const &t, symmetry_t const &s, indices_t const &ind = indices_t{},
          std::string name = "")
     : B(m, factory<data_t>(std::forward<D>(dat)), t, s, ind, name) {}

  /// ---------------  swap --------------------
  
  friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

  /// ---------------  Rebind --------------------
  //
  void rebind(gf_view const &X) noexcept {
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

  // cf gf : from the mpi
  template <typename Tag> gf_view &operator=(mpi::lazy<Tag, B> l) {
   B::operator=(l);
   return *this;
  }

 }; // class gf_view

 /*------------------------------------------------------------------------------------------------------
  *                                     View  assignment
  *-----------------------------------------------------------------------------------------------------*/

 // delegate = so that I can overload it for specific RHS...
 template <typename M, typename T, typename S, typename E, typename RHS>
 std14::enable_if_t<!arrays::is_scalar<RHS>::value>
 triqs_gf_view_assign_delegation(gf_view<M, T, S, E> g, RHS const &rhs) {
  if (!(g.mesh() == rhs.mesh()))
   TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh" << g.mesh() << " vs " << rhs.mesh();
  for (auto const &w : g.mesh()) g[w] = rhs[w];
  g.singularity() = rhs.singularity();
 }

 template <typename M, typename T, typename S, typename E, typename RHS>
 std14::enable_if_t<arrays::is_scalar<RHS>::value>
 triqs_gf_view_assign_delegation(gf_view<M, T, S, E> g, RHS const &x) {
  gf_view<M, T, S, E>::data_proxy_t::assign_to_scalar(g.data(), x);
  g.singularity() = x;
 }

 /*------------------------------------------------------------------------------------------------------
  *                                     gf_keeper
  *-----------------------------------------------------------------------------------------------------*/

 // tool for lazy transformation
 // TODO  : clean this : typename G
 template <typename Tag, typename D, typename Target = matrix_valued, typename Singularity = gf_default_singularity_t<D, Target>,
           typename Evaluator = void>
 struct gf_keeper {
  gf_const_view<D, Target, Singularity, Evaluator> g;
 };

 /*------------------------------------------------------------------------------------------------------
  *             Interaction with the CLEF library : auto assignment implementation
  *-----------------------------------------------------------------------------------------------------*/

 // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

 template <typename RHS, typename M, typename T, typename S, typename E, bool IsView>
 void triqs_clef_auto_assign(gf_impl<M, T, S, E, IsView, false> &g, RHS const &rhs) {
  triqs_clef_auto_assign_impl(g, rhs, typename std::is_base_of<tag::composite, gf_mesh<M>>::type());
  assign_singularity_from_function(g.singularity(), rhs);
  // access to the data . Beware, we view it as a *matrix* NOT an array... (crucial for assignment to scalars !)
  // if f is an expression, replace the placeholder with a simple tail.
 }

 // enable the writing g[om_] << .... also
 template <typename RHS, typename M, typename T, typename S, typename E, bool IsView>
 void triqs_clef_auto_assign_subscript(gf_impl<M, T, S, E, IsView, false> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g, rhs);
 }

 template <typename G, typename RHS> void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
  std::forward<G>(g) = std::forward<RHS>(rhs);
 }

 template <typename G, typename Expr, int... Is>
 void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
  triqs_clef_auto_assign(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
 }

 template <typename RHS, typename M, typename T, typename S, typename E, bool IsView>
 void triqs_clef_auto_assign_impl(gf_impl<M, T, S, E, IsView, false> &g, RHS const &rhs, std::integral_constant<bool, false>) {
  for (auto const &w : g.mesh()) {
   triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w));
  }
 }

 template <typename RHS, typename M, typename T, typename S, typename E, bool IsView>
 void triqs_clef_auto_assign_impl(gf_impl<M, T, S, E, IsView, false> &g, RHS const &rhs, std::integral_constant<bool, true>) {
  for (auto const &w : g.mesh()) {
   triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w.components_tuple()));
  }
 }

 // in most expression, the gf_impl version is enough.
 // But in chained clef expression, A(i_)(om_) where A is an array of gf
 // we need to call it with the gf, not gf_impl (or the template resolution find the deleted funciton in clef).
 // Another fix is to make gf, gf_view in the expression tree, but this requires using CRPT in gf_impl...
 template <typename RHS, typename M, typename T, typename S, typename E>
 void triqs_clef_auto_assign(gf<M, T, S, E> &g, RHS const &rhs) {
  triqs_clef_auto_assign(static_cast<gf_impl<M, T, S, E, false, false> &>(g), rhs);
 }

 // Idem for gf_view
 template <typename RHS, typename M, typename T, typename S, typename E>
 void triqs_clef_auto_assign(gf_view<M, T, S, E> &g, RHS const &rhs) {
  triqs_clef_auto_assign(static_cast<gf_impl<M, T, S, E, true, false> &>(g), rhs);
 }

 /*------------------------------------------------------------------------------------------------------
  *                            default factories
  *-----------------------------------------------------------------------------------------------------*/

 // Factory for the data
 template <typename M, typename T, typename S> struct gf_data_factory {
  using gf_t = gf<M, T, S>;
  using target_shape_t = arrays::mini_vector<int, T::dim>;
  using mesh_t = gf_mesh<M>;
  using aux_t = arrays::memory_layout<get_n_variables<M>::value + T::dim>;
  using data_t = typename gf_data_proxy<M, T>::storage_t;

  //
  static data_t make(mesh_t const &m, target_shape_t shape, aux_t ml) {
   data_t A(join(m.size_of_components(), shape), ml);
   A() = 0;
   return A;
  }
 };

 // Factory for the singularity
 template <typename M, typename T, typename S> struct gf_singularity_factory {
  template <typename TargetShape> static S make(gf_mesh<M> const &m, TargetShape shape) {
   return S{shape};
  }
 };

 /*------------------------------------------------------------------------------------------------------
  *                              HDF5
  *-----------------------------------------------------------------------------------------------------*/

 // scalar function : just add a _s
 template <typename Mesh, typename Singularity> struct gf_h5_name<Mesh, scalar_valued, Singularity> {
  static std::string invoke() { return gf_h5_name<Mesh, matrix_valued, Singularity>::invoke() + "_s"; }
 };

 // the h5 write and read of gf members, so that we can specialize it e.g. for block gf
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_h5_rw {

  static void write(h5::group gr, gf_const_view<Mesh, Target, Singularity, Evaluator> g) {
   h5_write(gr, "data", g._data);
   h5_write(gr, "singularity", g._singularity);
   h5_write(gr, "mesh", g._mesh);
   h5_write(gr, "symmetry", g._symmetry);
   h5_write(gr, "indices", g._indices);
  }

  template <bool B> static void read(h5::group gr, gf_impl<Mesh, Target, Singularity, Evaluator, B, false> &g) {
   h5_read(gr, "data", g._data);
   h5_read(gr, "singularity", g._singularity);
   h5_read(gr, "mesh", g._mesh);
   h5_read(gr, "symmetry", g._symmetry);
   h5_read(gr, "indices", g._indices);
  }
 };
}
}

/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views, as for arrays 
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
template <typename Mesh, typename Target, typename Singularity, typename Evaluator, bool C1, bool C2>
void swap(triqs::gfs::gf_view<Mesh, Target, Singularity, Evaluator, C1> &a,
          triqs::gfs::gf_view<Mesh, Target, Singularity, Evaluator, C2> &b) = delete;
}

// To cut the file ...
#include "./impl/gf_expr.hpp"
#include "./impl/functions.hpp"
