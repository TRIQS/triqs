/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2014 by O. Parcollet
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
#include "gf_indices.hpp"
#include "tools.hpp"
#include <triqs/mpi/gf.hpp>
#include <vector>
#include "data_proxies.hpp"
#include "local/tail.hpp"

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

 template<typename MeshType> struct mesh_point;
 
 // The default target, for each Variable.
 template <typename Variable> struct gf_default_target {
  using type = matrix_valued;
 };
 template <typename Variable> using gf_default_target_t = typename gf_default_target<Variable>::type;

 // The default singularity, for each Variable.
 template <typename Variable, typename Target> struct gf_default_singularity {
  using type = nothing;
 };
 template <typename Variable, typename Target>
 using gf_default_singularity_t = typename gf_default_singularity<Variable, Target>::type;

 // the gf mesh
 template <typename Variable> struct gf_mesh;

 // The regular type
 template <typename Variable, typename Target = gf_default_target_t<Variable>,
           typename Singularity = gf_default_singularity_t<Variable, Target>, typename Evaluator = void>
 class gf;

 // The view type
 template <typename Variable, typename Target = gf_default_target_t<Variable>,
           typename Singularity = gf_default_singularity_t<Variable, Target>, typename Evaluator = void, bool IsConst = false>
 class gf_view;

 // The const view type
 template <typename Variable, typename Target = gf_default_target_t<Variable>,
           typename Singularity = gf_default_singularity_t<Variable, Target>, typename Evaluator = void>
 using gf_const_view = gf_view<Variable, Target, Singularity, Evaluator, true>;

 template<typename Variable> struct get_n_variables { static const int value = 1;};

 // the implementation class
 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst> class gf_impl;

 // various implementation traits
 namespace gfs_implementation { // never use using of this...

  // evaluator regroup functions to evaluate the function.
  template <typename Variable, typename Target, typename Singularity> struct evaluator {
   static constexpr int arity = 0;
   template <typename G> evaluator(G *) {};
  };

  // closest_point mechanism
  template <typename Variable, typename Target, typename Singularity, typename Evaluator> struct get_closest_point;

  // symmetry
  template <typename Variable, typename Target, typename Singularity> struct symmetry {
   using type = nothing;
  };

  // indices
  template <typename Target> struct indices {
   using type = nothing;
  };

  template <> struct indices<matrix_valued> {
   using type = gf_indices_pair;
  };

  // data_proxy contains function to manipulate the data array, but no data itself.
  // this is used to specialize this part of the code to array of dim 3 (matrix gf), dim 1 (scalar gf) and vector (e.g. block gf,
  // ...)
  template <typename Variable, typename Target, typename Enable = void> struct data_proxy;

  // Traits to read/write in hdf5 files. Can be specialized for some case (Cf block). Defined below
  template <typename Variable, typename Target, typename Singularity> struct h5_name; // value is a const char
  template <typename Variable, typename Target, typename Singularity, typename Evaluator> struct h5_rw;

  // factories (constructors..) for all types of gf. Defaults implemented below.
  template <typename Variable, typename Target, typename Singularity> struct data_factory;
  template <typename Variable, typename Target, typename Singularity> struct singularity_factory;

 } // gfs_implementation

 // The trait that "marks" the Green function
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableGreenFunction);

 template <typename Variable, typename Target, typename Singularity, typename Evaluator>
 auto get_gf_data_shape(gf<Variable, Target, Singularity, Evaluator> const &g) RETURN(get_shape(g.data()));

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsConst>
 auto get_gf_data_shape(gf_view<Variable, Target, Singularity, Evaluator, IsConst> const &g) RETURN(get_shape(g.data()));

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
 auto get_gf_data_shape(gf_impl<Variable, Target, Singularity, Evaluator, IsView, IsConst> const &g) RETURN(get_shape(g.data()));

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
 auto get_target_shape(gf_impl<Variable, Target, Singularity, Evaluator, IsView, IsConst> const &g)
     DECL_AND_RETURN(g.data().shape().front_pop());

 // ---------------------- implementation --------------------------------

 // overload get_shape for a vector to simplify code below in gf block case.
 template <typename T> long get_shape(std::vector<T> const &x) { return x.size(); }

 /// A common implementation class for gf and gf_view. They will only redefine contructor and = ...
 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
 class gf_impl : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {
  static_assert(!(!IsView && IsConst), "Internal error");

  public:
  using mutable_view_type = gf_view<Variable, Target, Singularity, Evaluator>;
  using const_view_type = gf_const_view<Variable, Target, Singularity, Evaluator>;
  using view_type = typename std::conditional<IsConst, const_view_type, mutable_view_type>::type;
  using regular_type = gf<Variable, Target, Singularity, Evaluator>;

  using variable_t = Variable;
  using target_t = Target;

  using mesh_t = gf_mesh<Variable>;
  using domain_t = typename mesh_t::domain_t;
  using mesh_point_t = typename mesh_t::mesh_point_t;
  using mesh_index_t = typename mesh_t::index_t;
  using linear_mesh_index_t = typename mesh_t::linear_index_t;
  using symmetry_t = typename gfs_implementation::symmetry<Variable, Target, Singularity>::type;
  using indices_t = typename gfs_implementation::indices<Target>::type;
  using evaluator_t = std14::conditional_t<std::is_same<Evaluator, void>::value,
                                           gfs_implementation::evaluator<Variable, Target, Singularity>, Evaluator>;

  using data_proxy_t = gfs_implementation::data_proxy<Variable, Target>;
  using data_regular_t = typename data_proxy_t::storage_t;
  using data_view_t = typename data_proxy_t::storage_view_t;
  using data_const_view_t = typename data_proxy_t::storage_const_view_t;
  using data_t = std14::conditional_t<IsView, std14::conditional_t<IsConst, data_const_view_t, data_view_t>, data_regular_t>;

  using _singularity_regular_t = typename Singularity::regular_type;
  using _singularity_view_t = typename Singularity::view_type;
  using _singularity_const_view_t = typename Singularity::const_view_type;
  using singularity_t = std14::conditional_t<IsView, std14::conditional_t<IsConst, _singularity_const_view_t, _singularity_view_t>,
                                             _singularity_regular_t>;

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
  // ------------- All the call operators -----------------------------

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
      std::result_of<evaluator_t(gf_impl *, Args...)> // what is the result type of call
      >::type                                         // end of lazy_disable_if
  operator()(Args &&... args) const {
   return _evaluator(this, std::forward<Args>(args)...);
  }

  template <typename... Args> typename clef::_result_of::make_expr_call<gf_impl &, Args...>::type operator()(Args &&... args) & {
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args>
  typename clef::_result_of::make_expr_call<gf_impl const &, Args...>::type operator()(Args &&... args) const &{
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args> typename clef::_result_of::make_expr_call<gf_impl, Args...>::type operator()(Args &&... args) && {
   return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
  }

  // ------------- All the [] operators -----------------------------
  //  [] and access to the grid point
  using r_type = std14::result_of_t<data_proxy_t(data_t &, typename mesh_t::linear_index_t)>;
  using cr_type = std14::result_of_t<data_proxy_t(data_t const &, typename mesh_t::linear_index_t)>;

  r_type operator[](mesh_index_t const &arg) { return _data_proxy(_data, _mesh.index_to_linear(arg)); }
  cr_type operator[](mesh_index_t const &arg) const { return _data_proxy(_data, _mesh.index_to_linear(arg)); }

  r_type operator[](mesh_point_t const &x) { return _data_proxy(_data, x.linear_index()); }
  cr_type operator[](mesh_point_t const &x) const { return _data_proxy(_data, x.linear_index()); }

  template <typename... U> r_type operator[](closest_pt_wrap<U...> const &p) {
   return _data_proxy(
       _data,
       _mesh.index_to_linear(gfs_implementation::get_closest_point<Variable, Target, Singularity, Evaluator>::invoke(this, p)));
  }
  template <typename... U> cr_type operator[](closest_pt_wrap<U...> const &p) const {
   return _data_proxy(
       _data,
       _mesh.index_to_linear(gfs_implementation::get_closest_point<Variable, Target, Singularity, Evaluator>::invoke(this, p)));
  }

  template <typename Arg>
  typename clef::_result_of::make_expr_subscript<gf_impl const &, Arg>::type operator[](Arg &&arg) const &{
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> typename clef::_result_of::make_expr_subscript<gf_impl &, Arg>::type operator[](Arg &&arg) & {
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> typename clef::_result_of::make_expr_subscript<gf_impl, Arg>::type operator[](Arg &&arg) && {
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
   auto s = gfs_implementation::h5_name<Variable, Target, Singularity>::invoke();
   return (s == "BlockGf" ? s : "Gf" + s);
  }

  friend struct gfs_implementation::h5_rw<Variable, Target, Singularity, Evaluator>;

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, gf_impl const &g) {
   auto gr = fg.create_group(subgroup_name);
   gr.write_triqs_hdf5_data_scheme(g);
   gfs_implementation::h5_rw<Variable, Target, Singularity, Evaluator>::write(gr, g);
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
   gfs_implementation::h5_rw<Variable, Target, Singularity, Evaluator>::read(gr, g);
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

  /// print
  friend std::ostream &operator<<(std::ostream &out, gf_impl const &x) { return out << (IsView ? "gf_view" : "gf"); }
  friend std::ostream &triqs_nvl_formal_print(std::ostream &out, gf_impl const &x) { return out << (IsView ? "gf_view" : "gf"); }
 };

 // -------------------------Interaction with the CLEF library : auto assignement implementation -----------------
 // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

 template <typename RHS, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView>
 void triqs_clef_auto_assign(gf_impl<Variable, Target, Singularity, Evaluator, IsView, false> &g, RHS const &rhs) {
  triqs_clef_auto_assign_impl(g, rhs, typename std::is_base_of<tag::composite, gf_mesh<Variable>>::type());
  assign_singularity_from_function(g.singularity(), rhs);
  // access to the data . Beware, we view it as a *matrix* NOT an array... (crucial for assignment to scalars !)
  // if f is an expression, replace the placeholder with a simple tail. 
 }

 // enable the writing g[om_] << .... also
 template <typename RHS, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView>
 void triqs_clef_auto_assign_subscript(gf_impl<Variable, Target, Singularity, Evaluator, IsView, false> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g, rhs);
 }

 template <typename G, typename RHS> void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
  std::forward<G>(g) = std::forward<RHS>(rhs);
 }

 template <typename G, typename Expr, int... Is>
 void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
  triqs_clef_auto_assign(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
 }

 template <typename RHS, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView>
 void triqs_clef_auto_assign_impl(gf_impl<Variable, Target, Singularity, Evaluator, IsView, false> &g, RHS const &rhs,
                                  std::integral_constant<bool, false>) {
  for (auto const &w : g.mesh()) {
   triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w));
   //(*this)[w] = rhs(w);
  }
 }

 template <typename RHS, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView>
 void triqs_clef_auto_assign_impl(gf_impl<Variable, Target, Singularity, Evaluator, IsView, false> &g, RHS const &rhs,
                                  std::integral_constant<bool, true>) {
  for (auto const &w : g.mesh()) {
   triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w.components_tuple()));
   //(*this)[w] = triqs::tuple::apply(rhs, w.components_tuple());
  }
 }

 // -------------------------The regular class of GF --------------------------------------------------------

 template <typename Variable, typename Target, typename Singularity, typename Evaluator> class gf : public gf_impl<Variable, Target, Singularity, Evaluator, false, false> {
  using B = gf_impl<Variable, Target, Singularity, Evaluator, false, false>;
  using data_factory = gfs_implementation::data_factory<Variable, Target, Singularity>;
  using singularity_factory = gfs_implementation::singularity_factory<Variable, Target, Singularity>;

  public:
  gf() : B() {}
  gf(gf const &g) : B(g) {}
  gf(gf &&g) noexcept : B(std::move(g)) {}
  template <typename Ev2> gf(gf_view<Variable, Target, Singularity, Ev2> const &g) : B(g, bool {}) {}
  template <typename Ev2> gf(gf_const_view<Variable, Target, Singularity, Ev2> const &g) : B(g, bool {}) {}

  template <typename GfType>
  gf(GfType const &x, std14::enable_if_t<ImmutableGreenFunction<GfType>::value> *dummy = 0)
     : B() {
   *this = x;
  }
 
  // mpi lazy 
  template <typename Tag> gf(mpi::mpi_lazy<Tag, gf> x) : gf() { operator=(x); }

  gf(typename B::mesh_t m, typename B::data_t dat, typename B::singularity_t const &si, typename B::symmetry_t const &s,
     typename B::indices_t const &ind, std::string name = "")
     : B(std::move(m), std::move(dat), si, s, ind, name) {}

  using target_shape_t = typename data_factory::target_shape_t;

  // with aux, and indices
  gf(typename B::mesh_t m, target_shape_t shape, typename data_factory::aux_t aux,
     typename B::indices_t const &ind = typename B::indices_t{}, std::string name = "")
     : B(std::move(m), data_factory::make(m, shape, aux), singularity_factory::make(m, shape), typename B::symmetry_t{}, ind,
         name) {
   if (this->_indices.is_empty()) this->_indices = typename B::indices_t(shape);
  }

  // without the aux (defaulted)
  gf(typename B::mesh_t m, target_shape_t shape = target_shape_t{}, typename B::indices_t const &ind = typename B::indices_t{},
     std::string name = "")
     : B(std::move(m), data_factory::make(m, shape, typename data_factory::aux_t{}), singularity_factory::make(m, shape),
         typename B::symmetry_t{}, ind, name) {
   if (this->_indices.is_empty()) this->_indices = typename B::indices_t(shape);
  }

  friend void swap(gf &a, gf &b) noexcept { a.swap_impl(b); }

  // CLEAN THIS : one op only gf rhs, + move 
  gf &operator=(gf const &rhs) {
   *this = gf(rhs);
   return *this;
  } // use move =
  gf &operator=(gf &rhs) {
   *this = gf(rhs);
   return *this;
  } // use move =
  gf &operator=(gf &&rhs) noexcept {
   swap(*this, rhs);
   return *this;
  }

  friend struct mpi::mpi_impl_triqs_gfs<gf>; //allowed to modify mesh

  // 
  template <typename Tag> void operator=(mpi::mpi_lazy<Tag, gf> x) {
   mpi::mpi_impl_triqs_gfs<gf>::complete_operation(*this, x);
  }

  template <typename RHS> void operator=(RHS &&rhs) {
   this->_mesh = rhs.mesh();
   this->_data.resize(get_gf_data_shape(rhs));
   for (auto const &w : this->mesh()) {
    (*this)[w] = rhs[w];
   }
   this->_singularity = rhs.singularity();
   // to be implemented : there is none in the gf_expr in particular....
   // this->_symmetry = rhs.symmetry();
   // indices and name are not affected by it ???
  }
 };

 // in most expression, the gf_impl version is enough.
 // But in chained clef expression, A(i_)(om_) where A is an array of gf
 // we need to call it with the gf, not gf_impl (or the template resolution find the deleted funciton in clef).
 // Another fix is to make gf, gf_view in the expression tree, but this requires using CRPT in gf_impl...
 template <typename RHS, typename Variable, typename Target, typename Singularity, typename Evaluator>
 void triqs_clef_auto_assign(gf<Variable, Target, Singularity, Evaluator> &g, RHS const &rhs) {
  triqs_clef_auto_assign(static_cast<gf_impl<Variable, Target, Singularity, Evaluator, false, false> &>(g), rhs);
 }

 // --------------------------The const View class of GF -------------------------------------------------------

 template <typename Variable, typename Target, typename Singularity, typename Evaluator>
 class gf_view<Variable, Target, Singularity, Evaluator, true> : public gf_impl<Variable, Target, Singularity, Evaluator, true, true> {
  using B = gf_impl<Variable, Target, Singularity, Evaluator, true, true>;

  public:
  gf_view() = delete;
  gf_view(gf_view const &g) : B(g) {}
  gf_view(gf_view &&g) noexcept : B(std::move(g)) {}

  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, true, true> const &g) : B(g, bool {}) {}   // from a const_view
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, true, false> const &g) : B(g, bool {}) {}  // from a view
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, false, false> const &g) : B(g, bool {}) {} // from a const gf
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, false, false> &g) : B(g, bool {}) {}       // from a gf &
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, false, false> &&g) noexcept : B(std::move(g), bool {}) {} // from a gf &&

  template <typename D>
  gf_view(typename B::mesh_t const &m, D const &dat, typename B::singularity_t const &t, typename B::symmetry_t const &s,
          typename B::indices_t const &ind, std::string name = "")
     : B(m, factory<typename B::data_t>(dat), t, s, ind, name) {}

  void rebind(gf_view const &X) noexcept {
   this->_mesh = X._mesh;
   this->_symmetry = X._symmetry;
   this->_data_proxy.rebind(this->_data, X);
   this->_singularity.rebind(X._singularity);
   this->_indices = X._indices;
   this->name = X.name;
  }

  void rebind(gf_view<Variable, Target, Singularity, Evaluator, false> const &X) noexcept {
   rebind(gf_view{X});
  }

  gf_view &operator=(gf_view const &) = delete;
 }; // class gf_const_view

 // -------------------------  The View class of GF  -------------------------------------------------------

 template <typename Variable, typename Target, typename Singularity, typename Evaluator>
 class gf_view<Variable, Target, Singularity, Evaluator, false> : public gf_impl<Variable, Target, Singularity, Evaluator, true, false> {
  using B = gf_impl<Variable, Target, Singularity, Evaluator, true, false>;

  public:
  gf_view() = delete;
  gf_view(gf_view const &g) : B(g) {}
  gf_view(gf_view &&g) noexcept : B(std::move(g)) {}

  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, true, true> const &g) = delete;           // from a const view : impossible
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, true, false> const &g) : B(g, bool {}) {} // from a view
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, false, false> const &g) = delete;         // from a const gf : impossible
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, false, false> &g) : B(g, bool {}) {}      // from a gf &
  template <typename Ev2> gf_view(gf_impl<Variable, Target, Singularity, Ev2, false, false> &&g) noexcept : B(std::move(g), bool {}) {} // from a gf &&

  template <typename D>
  gf_view(typename B::mesh_t const &m, D &&dat, typename B::singularity_t const &t, typename B::symmetry_t const &s,
          typename B::indices_t const &ind = typename B::indices_t{}, std::string name = "")
     : B(m, factory<typename B::data_t>(std::forward<D>(dat)), t, s, ind, name) {}

  friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

  void rebind(gf_view const &X) noexcept {
   this->_mesh = X._mesh;
   this->_symmetry = X._symmetry;
   this->_data_proxy.rebind(this->_data, X);
   this->_singularity.rebind(X._singularity);
   this->_indices = X._indices;
   this->name = X.name;
  }

  gf_view &operator=(gf_view const &rhs) {
   triqs_gf_view_assign_delegation(*this, rhs);
   return *this;
  }

  template <typename RHS> gf_view &operator=(RHS const &rhs) {
   triqs_gf_view_assign_delegation(*this, rhs);
   return *this;
  }
 }; // class gf_view

 // delegate = so that I can overload it for specific RHS...
 template <typename Variable, typename Target, typename Singularity, typename Evaluator, typename RHS>
 DISABLE_IF(arrays::is_scalar<RHS>) triqs_gf_view_assign_delegation(gf_view<Variable, Target, Singularity, Evaluator> g, RHS const &rhs) {
  if (!(g.mesh() == rhs.mesh()))
   TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh" << g.mesh() << " vs " << rhs.mesh();
  for (auto const &w : g.mesh()) g[w] = rhs[w];
  g.singularity() = rhs.singularity();
 }

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, typename T>
 ENABLE_IF(arrays::is_scalar<T>) triqs_gf_view_assign_delegation(gf_view<Variable, Target, Singularity, Evaluator> g, T const &x) {
  gf_view<Variable, Target, Singularity, Evaluator>::data_proxy_t::assign_to_scalar(g.data(), x);
  g.singularity() = x;
 }

 // tool for lazy transformation
 // TODO  : clean this : typename G
 template <typename Tag, typename D, typename Target = matrix_valued,
           typename Singularity = gf_default_singularity_t<D, Target>, typename Evaluator = void>
 struct gf_keeper {
  gf_const_view<D, Target, Singularity, Evaluator> g;
 };

 // Cf gf
 template <typename RHS, typename Variable, typename Target, typename Singularity, typename Evaluator>
 void triqs_clef_auto_assign(gf_view<Variable, Target, Singularity, Evaluator> &g, RHS const &rhs) {
  triqs_clef_auto_assign( static_cast<gf_impl<Variable, Target, Singularity, Evaluator, true, false>&>(g), rhs);
 }

 // ---------------------------------- slicing ------------------------------------

 // slice
 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsConst, typename... Args>
 gf_view<Variable, matrix_valued, Singularity, Evaluator, IsConst> slice_target(gf_view<Variable, Target, Singularity, Evaluator, IsConst> g,
                                                                          Args &&... args) {
  static_assert(std::is_same<Target, matrix_valued>::value, "slice_target only for matrix_valued GF's");
  using arrays::range;
  return {g.mesh(),                                                   g.data()(range(), std::forward<Args>(args)...),
          slice_target(g.singularity(), std::forward<Args>(args)...), g.symmetry(),
          slice(g.indices(), std::forward<Args>(args)...),            g.name};
 }

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, typename... Args>
 gf_view<Variable, matrix_valued, Singularity, Evaluator> slice_target(gf<Variable, Target, Singularity, Evaluator> &g, Args &&... args) {
  return slice_target(g(), std::forward<Args>(args)...);
 }

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, typename... Args>
 gf_const_view<Variable, matrix_valued, Singularity, Evaluator> slice_target(gf<Variable, Target, Singularity, Evaluator> const &g,
                                                                       Args &&... args) {
  return slice_target(g(), std::forward<Args>(args)...);
 }

 // slice to scalar
 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsConst, typename... Args>
 gf_view<Variable, scalar_valued, Singularity, void, IsConst>
 slice_target_to_scalar(gf_view<Variable, Target, Singularity, Evaluator, IsConst> g, Args &&... args) {
  static_assert(std::is_same<Target, matrix_valued>::value, "slice_target only for matrix_valued GF's");
  using arrays::range;
  return {g.mesh(),
          g.data()(range(), std::forward<Args>(args)...),
          slice_target(g.singularity(), range(args, args + 1)...),
          g.symmetry(),
          {},
          g.name};
 }

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, typename... Args>
 gf_view<Variable, scalar_valued, Singularity, void> slice_target_to_scalar(gf<Variable, Target, Singularity, Evaluator> &g,
                                                                           Args &&... args) {
  return slice_target_to_scalar(g(), std::forward<Args>(args)...);
 }

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, typename... Args>
 gf_const_view<Variable, scalar_valued, Singularity, void> slice_target_to_scalar(gf<Variable, Target, Singularity, Evaluator> const &g,
                                                                                 Args &&... args) {
  return slice_target_to_scalar(g(), std::forward<Args>(args)...);
 }

 // ---------------------------------- target reinterpretation ------------------------------------

 // a scalar_valued gf can be viewed as a 1x1 matrix
 template <typename Variable, typename Singularity, typename Evaluator, bool IsConst>
 gf_view<Variable, matrix_valued, Singularity, void, IsConst>
 reinterpret_scalar_valued_gf_as_matrix_valued(gf_view<Variable, scalar_valued, Singularity, Evaluator, IsConst> g) {
  using a_t = typename gf_view<Variable, matrix_valued, Singularity, void, IsConst>::data_view_t;
  auto & imap = g.data().indexmap();
  typename a_t::indexmap_type index_map(join(imap.lengths(), make_shape(1, 1)), join(imap.strides(), make_shape(1, 1)),
                                        imap.start_shift());
  auto a = a_t{index_map, g.data().storage()};
  return {g.mesh(), a, g.singularity(), g.symmetry(), {}, g.name};
 }

 template <typename Variable, typename Singularity, typename Evaluator>
 gf_view<Variable, matrix_valued, Singularity, void>
 reinterpret_scalar_valued_gf_as_matrix_valued(gf<Variable, scalar_valued, Singularity, Evaluator> &g) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(g());
 }

 template <typename Variable, typename Singularity, typename Evaluator>
 gf_const_view<Variable, matrix_valued, Singularity, void>
 reinterpret_scalar_valued_gf_as_matrix_valued(gf<Variable, scalar_valued, Singularity, Evaluator> const &g) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(g());
 }

 // ---------------------------------- some functions : invert, conjugate, transpose, ... ------------------------------------

 //  ---- inversion
 // auxiliary function : invert the data : one function for all matrix valued gf (save code).
 template <typename A3> void _gf_invert_data_in_place(A3 &&a) {
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   auto v = make_matrix_view(a(i, arrays::range(), arrays::range()));
   v = triqs::arrays::inverse(v);
  }
 }

 template <typename Variable, typename Singularity, typename Evaluator>
 void invert_in_place(gf_view<Variable, matrix_valued, Singularity, Evaluator> g) {
  _gf_invert_data_in_place(g.data());
  g.singularity() = inverse(g.singularity());
 }

 template <typename Variable, typename Singularity, typename Evaluator>
 gf<Variable, matrix_valued, Singularity, Evaluator> inverse(gf<Variable, matrix_valued, Singularity, Evaluator> const &g) {
  auto res = g;
  gf_view<Variable, matrix_valued, Singularity, Evaluator> v = res;
  invert_in_place(v);
  return res;
 }

 template <typename Variable, typename Singularity, typename Evaluator, bool B>
 gf<Variable, matrix_valued, Singularity, Evaluator> inverse(gf_view<Variable, matrix_valued, Singularity, Evaluator, B> g) {
  return inverse(gf<Variable, matrix_valued, Singularity, Evaluator>(g));
 }

 //  ---- transpose : a new gf

 template <typename Variable, typename Singularity, typename Evaluator>
 gf<Variable, matrix_valued, Singularity, Evaluator> transpose(gf_view<Variable, matrix_valued, Singularity, Evaluator> g) {
  return {g.mesh(), transposed_view(g.data(), 0, 2, 1), transpose(g.singularity()), g.symmetry(), transpose(g.indices()), g.name};
 }

 //  ---- conjugate : always a new function -> changelog
 //  Note that this calls the default conjugate for the singularity: needs to be specialized for imaginary gfs

 template <typename Variable, typename Singularity, typename Evaluator>
 gf<Variable, matrix_valued, Singularity, Evaluator> conj(gf_view<Variable, matrix_valued, Singularity, Evaluator> g) {
  return {g.mesh(), conj(g.data()), conj(g.singularity()), g.symmetry(), g.indices(), g.name};
 }

 //  ---- matrix mult R and L

 template <typename A3, typename T> void _gf_data_mul_R(A3 &&a, matrix<T> const &r) {
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   matrix_view<T> v = a(i, arrays::range(), arrays::range());
   v = v * r;
  }
 }

 template <typename A3, typename T> void _gf_data_mul_L(matrix<T> const &l, A3 &&a) {
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   matrix_view<T> v = a(i, arrays::range(), arrays::range());
   v = l * v;
  }
 }

 template <typename Variable, typename Singularity, typename Evaluator, typename T>
 gf<Variable, matrix_valued, Singularity, Evaluator> operator*(gf<Variable, matrix_valued, Singularity, Evaluator> g, matrix<T> r) {
  _gf_data_mul_R(g.data(), r);
  g.singularity() = g.singularity() * r;
  return g;
 }

 template <typename Variable, typename Singularity, typename Evaluator, typename T>
 gf<Variable, matrix_valued, Singularity, Evaluator> operator*(matrix<T> l, gf<Variable, matrix_valued, Singularity, Evaluator> g) {
  _gf_data_mul_L(l, g.data());
  g.singularity() = l * g.singularity();
  return g;
 }

template <typename A, typename B, typename M> void set_from_gf_data_mul_LR(A & a, M const & l, B const &b, M const & r) {
  auto tmp = matrix<typename M::value_type> (second_dim(b), second_dim(r));
  auto _ = arrays::range{};
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   auto rhs_v = make_matrix_view(b(i, _, _));
   auto lhs_v = make_matrix_view(a(i, _, _));
   arrays::blas::gemm(1,rhs_v,r,0,tmp);
   arrays::blas::gemm(1,l,tmp,0,lhs_v);
  }
 }

template <typename G1, typename G2, typename M>
 void set_from_L_G_R(G1 & g1, M const & l, G2 const & g2, M const & r) {
  set_from_gf_data_mul_LR(g1.data(), l, g2.data(), r);
  g1.singularity() = l * g2.singularity() * r;
 }

 namespace gfs_implementation { // implement some default traits

  // -------------------------  default factories ---------------------

  // Factory for the data
  template <typename Var, typename Target, int VarDim, typename Singularity> struct data_factory_default_impl {
   using gf_t = gf<Var, Target, Singularity>;
   using target_shape_t = arrays::mini_vector<int, VarDim>;
   using mesh_t = gf_mesh<Var>; //typename gf_t::mesh_t;
   using aux_t = arrays::memory_layout< get_n_variables<Var>::value + VarDim>;
   using data_t = typename gfs_implementation::data_proxy<Var, Target>::storage_t;

   //
   static data_t make(mesh_t const &m, target_shape_t shape, aux_t ml) {
    data_t A(gf_t::data_proxy_t::join_shape(m.size_of_components(), shape), ml);
    A() = 0;
    return A;
   }
  };

  template <int R, typename Var, typename Singularity>
  struct data_factory<Var, tensor_valued<R>, Singularity> : data_factory_default_impl<Var, tensor_valued<R>, R, Singularity> {};

  template <typename Var, typename Singularity>
  struct data_factory<Var, matrix_valued, Singularity> : data_factory_default_impl<Var, matrix_valued, 2, Singularity> {};

  template <typename Var, typename Singularity>
  struct data_factory<Var, scalar_valued, Singularity> : data_factory_default_impl<Var, scalar_valued, 0, Singularity> {};

  // Factory for the singularity
  template <typename Var, typename Target, typename Singularity> struct singularity_factory {
   template <typename TargetShape> static Singularity make(gf_mesh<Var> const &m, TargetShape shape) {
    return Singularity{shape};
   }
  };

  // ---------------------   hdf5 ---------------------------------------
  // scalar function : just add a _s
  template <typename Variable, typename Singularity> struct h5_name<Variable, scalar_valued, Singularity> {
   static std::string invoke() { return h5_name<Variable, matrix_valued, Singularity>::invoke() + "_s"; }
  };

  // the h5 write and read of gf members, so that we can specialize it e.g. for block gf
  template <typename Variable, typename Target, typename Singularity,typename Evaluator> struct h5_rw {

   static void write(h5::group gr, gf_const_view<Variable, Target, Singularity, Evaluator> g) {
    h5_write(gr, "data", g._data);
    h5_write(gr, "singularity", g._singularity);
    h5_write(gr, "mesh", g._mesh);
    h5_write(gr, "symmetry", g._symmetry);
    h5_write(gr, "indices", g._indices);
    // h5_write(gr, "name", g.name);
   }

   template<bool B> static void read(h5::group gr, gf_impl<Variable, Target, Singularity, Evaluator, B, false> &g) {
    h5_read(gr, "data", g._data);
    h5_read(gr, "singularity", g._singularity);
    h5_read(gr, "mesh", g._mesh);
    h5_read(gr, "symmetry", g._symmetry);
    h5_read(gr, "indices", g._indices);
    // h5_read(gr, "name", g.name);
   }
  };
 } // gfs_implementation
}

namespace mpi {

 template <typename Variable, typename Target, typename Singularity, typename Evaluator>
 struct mpi_impl<gfs::gf<Variable, Target, Singularity, Evaluator>,
                 void> : mpi_impl_triqs_gfs<gfs::gf<Variable, Target, Singularity, Evaluator>> {};

 template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsConst>
 struct mpi_impl<gfs::gf_view<Variable, Target, Singularity, Evaluator, IsConst>,
                 void> : mpi_impl_triqs_gfs<gfs::gf_view<Variable, Target, Singularity, Evaluator, IsConst>> {};
}
}

// same as for arrays : views cannot be swapped by the std::swap. Delete it
namespace std {
template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool C1, bool C2>
void swap(triqs::gfs::gf_view<Variable, Target, Singularity, Evaluator, C1> &a,
          triqs::gfs::gf_view<Variable, Target, Singularity, Evaluator, C2> &b) = delete;
}
#include "./gf_expr.hpp"
