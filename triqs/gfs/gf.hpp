/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_GF_GFBASECLASS_H
#define TRIQS_GF_GFBASECLASS_H
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/std_vector_expr_template.hpp>
#include <triqs/utility/factory.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/arrays/h5.hpp>
#include <vector>
#include "./tools.hpp"
#include "./data_proxies.hpp"

namespace triqs { namespace gfs {
 using utility::factory;
 using arrays::make_shape;

 // the gf mesh
 template <typename Variable, typename Opt = void> struct gf_mesh;

 // The regular type
 template <typename Variable, typename Target = matrix_valued, typename Opt = void> class gf;

 // The view type
 template <typename Variable, typename Target = matrix_valued, typename Opt = void, bool IsConst = false> class gf_view;

 // The const view type
 template <typename Variable, typename Target = matrix_valued, typename Opt = void>
 using gf_const_view = gf_view<Variable, Target, Opt, true>;

 // the implementation class
 template<typename Variable, typename Target, typename Opt, bool IsView, bool IsConst> class gf_impl;

 // various implementation traits
 namespace gfs_implementation { // never use using of this...

  // evaluator regroup functions to evaluate the function.
  template<typename Variable, typename Target, typename Opt> struct evaluator{ static constexpr int arity = 0;};

  // closest_point mechanism
  template<typename Variable, typename Target, typename Opt> struct get_closest_point;

  // singularity
  template<typename Variable, typename Target, typename Opt> struct singularity { typedef nothing type;};

  // symmetry
  template<typename Variable, typename Target, typename Opt> struct symmetry { typedef nothing type;};

  // data_proxy contains function to manipulate the data array, but no data itself.
  // this is used to specialize this part of the code to array of dim 3 (matrix gf), dim 1 (scalar gf) and vector (e.g. block gf, ...)
  template<typename Variable, typename Target, typename Opt, typename Enable = void> struct data_proxy;

  // Traits to read/write in hdf5 files. Can be specialized for some case (Cf block). Defined below
  template <typename Variable, typename Target, typename Opt> struct h5_name; // value is a const char 
  template <typename Variable, typename Target, typename Opt> struct h5_rw;
 
  // factories regroup all factories (constructors..) for all types of gf. Defaults implemented below.
  template <typename Variable, typename Target, typename Opt> struct factories;

 } // gfs_implementation

 // OBSOLETE : kept for backward compatibility only. Do not document. 
 // make_gf and make_gf_view forward any args to them
 template <typename Variable, typename Target=matrix_valued, typename Opt=void, typename ... U> 
  gf<Variable,Target,Opt> make_gf(gf_mesh<Variable,Opt> m, U && ... x) 
  { return gfs_implementation::factories<Variable,Target,Opt>::make_gf(std::move(m),std::forward<U>(x)...);}

 template <typename Variable, typename Target=matrix_valued, typename Opt=void, typename ... U> 
  gf<Variable,Target,Opt> make_gf(U && ... x) { return gfs_implementation::factories<Variable,Target,Opt>::make_gf(std::forward<U>(x)...);}

 template <typename Variable, typename Target=matrix_valued, typename Opt=void, typename ... U> 
  gf_view<Variable,Target,Opt> make_gf_view(U && ... x) { return gfs_implementation::factories<Variable,Target,Opt>::make_gf_view(std::forward<U>(x)...);}

 // The trait that "marks" the Green function
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableGreenFunction);

 template <typename G> auto get_gf_data_shape(G const &g) DECL_AND_RETURN(g.get_data_shape());

 template <typename Variable, typename Target, typename Opt, bool IsView, bool IsConst>
 auto get_target_shape(gf_impl<Variable, Target, Opt, IsView, IsConst> const &g) DECL_AND_RETURN(g.data().shape().front_pop());

 // ---------------------- implementation --------------------------------

 // overload get_shape for a vector to simplify code below in gf block case.
 template <typename T> long get_shape(std::vector<T> const &x) { return x.size(); }
 
 /// A common implementation class for gf and gf_view. They will only redefine contructor and = ...
 template <typename Variable, typename Target, typename Opt, bool IsView, bool IsConst>
 class gf_impl : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {
  static_assert(!(!IsView && IsConst), "Internal error");

   public :
    typedef gf_view<Variable,Target,Opt> mutable_view_type;
    typedef gf_const_view<Variable,Target,Opt> const_view_type;
    typedef typename std::conditional <IsConst, const_view_type, mutable_view_type>::type view_type;
    typedef gf<Variable,Target,Opt>      regular_type;

    typedef Variable variable_t;
    typedef Target target_t;
    typedef Opt option_t;

    typedef gf_mesh<Variable,Opt>                                                               mesh_t;
    typedef typename mesh_t::domain_t                                                           domain_t;
    typedef typename mesh_t::mesh_point_t                                                       mesh_point_t;
    typedef typename mesh_t::index_t                                                            mesh_index_t;
    typedef typename gfs_implementation::symmetry<Variable,Target,Opt>::type                    symmetry_t;
    typedef gfs_implementation::evaluator<Variable,Target,Opt>                                  evaluator_t;

    typedef gfs_implementation::data_proxy<Variable,Target,Opt>                                 data_proxy_t;
    typedef typename data_proxy_t::storage_t                                                    data_regular_t;
    typedef typename data_proxy_t::storage_view_t                                               data_view_t;
    typedef typename data_proxy_t::storage_const_view_t                                         data_const_view_t;
    typedef typename std::conditional<IsView, typename std::conditional<IsConst, data_const_view_t, data_view_t>::type,
	    data_regular_t>::type data_t;

    typedef typename gfs_implementation::singularity<Variable,Target,Opt>::type                 singularity_non_view_t;
    typedef typename view_type_if_exists_else_type<singularity_non_view_t>::type                singularity_view_t;
    typedef typename std::conditional<IsView, singularity_view_t, singularity_non_view_t>::type singularity_t;

    mesh_t const &        mesh()          const { return _mesh;}
    domain_t const &      domain()        const { return _mesh.domain();}
    data_t &              data()                { return _data;}
    data_t const &        data()          const { return _data;}
    singularity_t &       singularity()         { return _singularity;}
    singularity_t const & singularity()   const { return _singularity;}
    symmetry_t const &    symmetry()      const { return _symmetry;}
    evaluator_t const &   get_evaluator() const { return _evaluator;}

    auto get_data_shape() const DECL_AND_RETURN(get_shape(this->data()));

   protected:
    mesh_t        _mesh;
    data_t        _data;
    singularity_t _singularity;
    symmetry_t    _symmetry;
    evaluator_t   _evaluator;
    data_proxy_t  _data_proxy;

    // --------------------------------Constructors -----------------------------------------------
    // all protected but one, this is an implementation class, see gf/gf_view later for public one
    gf_impl() {} // all arrays of zero size (empty)

   public : //everyone can make a copy and a move (for clef lib in particular, this one needs to be public)
   gf_impl(gf_impl const &x)
      : _mesh(x.mesh()),
        _data(factory<data_t>(x.data())),
        _singularity(factory<singularity_t>(x.singularity())),
        _symmetry(x.symmetry()),
        _evaluator(x._evaluator) {}

    gf_impl(gf_impl &&) = default;

   protected:
   template <typename G>
   gf_impl(G &&x, bool) // bool to disambiguate
      : _mesh(x.mesh()),
        _data(factory<data_t>(x.data())),
        _singularity(factory<singularity_t>(x.singularity())),
        _symmetry(x.symmetry()),
        _evaluator(x.get_evaluator()) {}

   template <typename M, typename D, typename S, typename SY, typename EV>
   gf_impl(M &&m, D &&dat, S &&sing, SY &&sy, EV &&ev)
      : _mesh(std::forward<M>(m)),
        _data(std::forward<D>(dat)),
        _singularity(std::forward<S>(sing)),
        _symmetry(std::forward<SY>(sy)),
        _evaluator(std::forward<EV>(ev)) {}

    void operator = (gf_impl const & rhs) = delete; // done in derived class.

    void swap_impl (gf_impl & b) noexcept {
     using std::swap;
     swap(this->_mesh, b._mesh);
     swap(this->_data, b._data);
     swap(this->_singularity, b._singularity);
     swap(this->_symmetry, b._symmetry);
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
    typename std::add_const<typename boost::lazy_disable_if_c< // disable the template if one the following conditions it true
        (sizeof...(Args) == 0) || clef::is_any_lazy<Args...>::value ||
            ((sizeof...(Args) != evaluator_t::arity) && (evaluator_t::arity != -1)) // if -1 : no check
        ,
        std::result_of<evaluator_t(gf_impl *, Args...)> // what is the result type of call
        >::type                                         // end of lazy_disable_if
                            >::type                     // end of add_Const
    operator()(Args &&... args) const {
     return _evaluator(this, std::forward<Args>(args)...);
    }

    template <typename... Args>
        typename clef::_result_of::make_expr_call<gf_impl &, Args...>::type operator()(Args &&... args) & {
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
    typedef typename std::result_of<data_proxy_t(data_t &, size_t)>::type r_type;
    typedef typename std::result_of<data_proxy_t(data_t const &, size_t)>::type cr_type;

    r_type  operator[] (mesh_index_t const & arg)       { return _data_proxy(_data,_mesh.index_to_linear(arg));}
    cr_type operator[] (mesh_index_t const & arg) const { return _data_proxy(_data,_mesh.index_to_linear(arg));}

    r_type  operator[] (mesh_point_t const & x)       { return _data_proxy(_data, x.linear_index());}
    cr_type operator[] (mesh_point_t const & x) const { return _data_proxy(_data, x.linear_index());}

    template<typename ... U>
     r_type  operator[] (closest_pt_wrap<U...> const & p)       { return _data_proxy(_data, _mesh.index_to_linear( gfs_implementation::get_closest_point<Variable,Target,Opt>::invoke(this,p)));}
    template<typename ... U>
     cr_type operator[] (closest_pt_wrap<U...> const & p) const { return _data_proxy(_data, _mesh.index_to_linear( gfs_implementation::get_closest_point<Variable,Target,Opt>::invoke(this,p)));}

    template<typename Arg>
     typename clef::_result_of::make_expr_subscript<gf_impl const &,Arg>::type
     operator[](Arg && arg) const & { return clef::make_expr_subscript(*this,std::forward<Arg>(arg));}

    template<typename Arg>
     typename clef::_result_of::make_expr_subscript<gf_impl &,Arg>::type
     operator[](Arg && arg) & { return clef::make_expr_subscript(*this,std::forward<Arg>(arg));}

    template<typename Arg>
     typename clef::_result_of::make_expr_subscript<gf_impl,Arg>::type
     operator[](Arg && arg) && { return clef::make_expr_subscript(std::move(*this),std::forward<Arg>(arg));}

    /// --------------------- A direct access to the grid point --------------------------
    template<typename... Args>
     r_type on_mesh (Args&&... args) { return _data_proxy(_data,_mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));}

    template<typename... Args>
     cr_type on_mesh (Args&&... args) const { return _data_proxy(_data,_mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));}

    // The on_mesh little adaptor ....
    private:
    struct _on_mesh_wrapper_const {
     gf_impl const &f;
     template <typename... Args> cr_type operator()(Args &&... args) const { return f.on_mesh(std::forward<Args>(args)...); }
    };
    struct _on_mesh_wrapper {
     gf_impl &f;
     template <typename... Args> r_type operator()(Args &&... args) const { return f.on_mesh(std::forward<Args>(args)...); }
    };

    public:
    _on_mesh_wrapper_const friend on_mesh(gf_impl const &f) {
     return {f};
    }
    _on_mesh_wrapper friend on_mesh(gf_impl &f) {
     return {f};
    }

    //----------------------------- HDF5 -----------------------------

    friend std::string get_triqs_hdf5_data_scheme(gf_impl const & g) { return "Gf" + gfs_implementation::h5_name<Variable,Target,Opt>::invoke();}

    friend class gfs_implementation::h5_rw<Variable,Target,Opt>;

    /// Write into HDF5
    friend void h5_write (h5::group fg, std::string subgroup_name, gf_impl const & g) {
     auto gr =  fg.create_group(subgroup_name);
     gr.write_triqs_hdf5_data_scheme(g); 
     gfs_implementation::h5_rw<Variable,Target,Opt>::write(gr, g);
    }

    /// Read from HDF5
    friend void h5_read (h5::group fg, std::string subgroup_name, gf_impl & g) {
     auto gr = fg.open_group(subgroup_name);
     // Check the attribute or throw
     auto tag_file = gr.read_triqs_hdf5_data_scheme();
     auto tag_expected= get_triqs_hdf5_data_scheme(g);
     if (tag_file != tag_expected) 
      TRIQS_RUNTIME_ERROR<< "h5_read : mismatch of the tag TRIQS_HDF5_data_scheme tag in the h5 group : found "<<tag_file << " while I expected "<< tag_expected; 
     gfs_implementation::h5_rw<Variable,Target,Opt>::read(gr, g);
    }

    //-----------------------------  BOOST Serialization -----------------------------
    friend class boost::serialization::access;
    template<class Archive>
     void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::make_nvp("data",_data);
      ar & boost::serialization::make_nvp("singularity",_singularity);
      ar & boost::serialization::make_nvp("mesh",_mesh);
      ar & boost::serialization::make_nvp("symmetry",_symmetry);
     }

    /// print
    friend std::ostream & operator << (std::ostream & out, gf_impl const & x) { return out<<(IsView ? "gf_view": "gf");}
    friend std::ostream & triqs_nvl_formal_print(std::ostream & out, gf_impl const & x) { return out<<(IsView ? "gf_view": "gf");}

 };

 // -------------------------Interaction with the CLEF library : auto assignement implemnetation--------------------------------------
 // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

  template <typename RHS, typename Variable, typename Target, typename Opt, bool IsView>
  void triqs_clef_auto_assign(gf_impl<Variable, Target, Opt, IsView, false> &g, RHS const &rhs) {
   triqs_clef_auto_assign_impl(g, rhs, typename std::is_base_of<tag::composite, gf_mesh<Variable,Opt>>::type());
   assign_from_expression(g.singularity(), rhs);
   // access to the data . Beware, we view it as a *matrix* NOT an array... (crucial for assignment to scalars !)
   // if f is an expression, replace the placeholder with a simple tail. If f is a function callable on freq_infty,
   // it uses the fact that tail_non_view_t can be casted into freq_infty
  }

  // enable the writing g[om_] << .... also
  template <typename RHS, typename Variable, typename Target, typename Opt, bool IsView>
  void triqs_clef_auto_assign_subscript(gf_impl<Variable, Target, Opt, IsView, false> &g, RHS const &rhs) {
   triqs_clef_auto_assign(g, rhs);
  }

  template <bool B, typename G, typename RHS>
  void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs, std::integral_constant<bool, B>) {
   std::forward<G>(g) = std::forward<RHS>(rhs);
  }

  template <typename G, bool B, typename Expr, int... Is>
  void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs, std::integral_constant<bool, B>) {
   triqs_clef_auto_assign_impl(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs), std::integral_constant<bool, B>());
  }

  template <typename RHS, typename Variable, typename Target, typename Opt, bool IsView>
  void triqs_clef_auto_assign_impl(gf_impl<Variable, Target, Opt, IsView, false> &g, RHS const &rhs,
                                   std::integral_constant<bool, false>) {
   for (auto const &w : g.mesh()) {
    triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w), std::integral_constant<bool, false>());
    //(*this)[w] = rhs(w);
   }
  }

  template <typename RHS, typename Variable, typename Target, typename Opt, bool IsView>
  void triqs_clef_auto_assign_impl(gf_impl<Variable, Target, Opt, IsView, false> &g, RHS const &rhs,
                                   std::integral_constant<bool, true>) {
   for (auto const &w : g.mesh()) {
     triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w.components_tuple()), std::integral_constant<bool, true>());
     //(*this)[w] = triqs::tuple::apply(rhs, w.components_tuple());
   }
  }

 // -------------------------The regular class of GF --------------------------------------------------------
 
 template<typename Variable, typename Target, typename Opt> class gf : public gf_impl<Variable,Target,Opt,false, false> {
  typedef gf_impl<Variable,Target,Opt,false,false> B;
  typedef gfs_implementation::factories<Variable,Target,Opt> factory;
  public :
  gf() : B() {}
  gf(gf const &g) : B(g) {}
  gf(gf &&g) noexcept : B(std::move(g)) {}
  gf(gf_view<Variable, Target, Opt> const &g) : B(g, bool{}) {}
  gf(gf_const_view<Variable, Target, Opt> const &g) : B(g, bool{}) {}

  template <typename GfType>
  gf(GfType const &x, typename std::enable_if<ImmutableGreenFunction<GfType>::value>::type *dummy = 0)
     : B() {
   *this = x;
  }

  gf(typename B::mesh_t m, typename B::data_t dat, typename B::singularity_view_t const &si, typename B::symmetry_t const &s)
     : B(std::move(m), std::move(dat), si, s, typename B::evaluator_t{}) {}

  typedef typename factory::target_shape_t target_shape_t;

  gf(typename B::mesh_t m, target_shape_t shape = target_shape_t{} ):
   B(std::move(m), factory::make_data(m,shape), factory::make_singularity(m,shape), typename B::symmetry_t {}, typename B::evaluator_t{}) {} 

  friend void swap (gf & a, gf & b) noexcept { a.swap_impl (b);}

  gf & operator = (gf const & rhs)     { *this = gf(rhs); return *this;} // use move =
  gf & operator = (gf & rhs)           { *this = gf(rhs); return *this;} // use move =
  gf & operator = (gf && rhs) noexcept { swap(*this,rhs); return *this;}

  template<typename RHS> void operator = (RHS && rhs) {
   this->_mesh = rhs.mesh();
   this->_data.resize(get_gf_data_shape(rhs));
   for (auto const & w: this->mesh())  { (*this)[w] = rhs[w]; }
   this->_singularity = rhs.singularity();
   // to be implemented : there is none in the gf_expr in particular....
   //this->_symmetry = rhs.symmetry(); 
  }

 };

 // --------------------------The const View class of GF -------------------------------------------------------
 
 template<typename Variable, typename Target, typename Opt> class gf_view<Variable,Target,Opt,true> : public gf_impl<Variable,Target,Opt,true,true> {
  typedef gf_impl<Variable,Target,Opt,true,true> B;
  public :
  gf_view() = delete;
  gf_view(gf_view const &g) : B(g) {}
  gf_view(gf_view &&g) noexcept : B(std::move(g)) {}

  gf_view(gf_impl<Variable, Target, Opt, true, true> const &g) : B(g, bool{}) {}                  // from a const_view
  gf_view(gf_impl<Variable, Target, Opt, true, false> const &g) : B(g, bool{}) {}                 // from a view
  gf_view(gf_impl<Variable, Target, Opt, false, false> const &g) : B(g, bool{}) {}                // from a const gf
  gf_view(gf_impl<Variable, Target, Opt, false, false> &g) : B(g, bool{}) {}                      // from a gf &
  gf_view(gf_impl<Variable, Target, Opt, false, false> &&g) noexcept : B(std::move(g), bool{}) {} // from a gf &&

  template <typename D>
  gf_view(typename B::mesh_t const &m, D const &dat, typename B::singularity_view_t const &t, typename B::symmetry_t const &s)
     : B(m, factory<typename B::data_t>(dat), t, s, typename B::evaluator_t{}) {}

  void rebind(gf_view const &X) noexcept {
   this->_mesh = X._mesh; this->_symmetry = X._symmetry; 
   this->_data_proxy.rebind(this->_data,X); 
   this->_singularity.rebind(X._singularity);
  }

  void rebind(gf_view<Variable, Target, Opt, false> const &X) noexcept {
   rebind(gf_view{X});
  }

  gf_view & operator = (gf_view const & ) = delete;
 }; // class gf_const_view

 // -------------------------  The View class of GF  -------------------------------------------------------
 
 template<typename Variable, typename Target, typename Opt> class gf_view<Variable,Target,Opt,false> : public gf_impl<Variable,Target,Opt,true,false> {
  typedef gf_impl<Variable,Target,Opt,true,false> B;
  public :
  gf_view() = delete;
  gf_view(gf_view const &g) : B(g) {}
  gf_view(gf_view &&g) noexcept : B(std::move(g)) {}

  gf_view(gf_impl<Variable, Target, Opt, true, true> const &g) = delete;          // from a const view : impossible
  gf_view(gf_impl<Variable, Target, Opt, true, false> const &g) : B(g, bool{}) {} // from a view
  gf_view(gf_impl<Variable, Target, Opt, false, false> const &g) = delete;        // from a const gf : impossible
  gf_view(gf_impl<Variable, Target, Opt, false, false> &g) : B(g, bool{}) {}                      // from a gf &
  gf_view(gf_impl<Variable, Target, Opt, false, false> &&g) noexcept : B(std::move(g), bool{}) {} // from a gf &&

  template <typename D>
  gf_view(typename B::mesh_t const &m, D const &dat, typename B::singularity_view_t const &t, typename B::symmetry_t const &s)
     : B(m, factory<typename B::data_t>(dat), t, s, typename B::evaluator_t{}) {}

  friend void swap (gf_view & a, gf_view & b) noexcept { a.swap_impl (b);}

  void rebind(gf_view const &X) noexcept {
   this->_mesh = X._mesh; this->_symmetry = X._symmetry; 
   this->_data_proxy.rebind(this->_data,X); 
   this->_singularity.rebind(X._singularity);
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
 template<typename Variable, typename Target, typename Opt, typename RHS>
  DISABLE_IF(arrays::is_scalar<RHS>) triqs_gf_view_assign_delegation( gf_view<Variable,Target,Opt> g, RHS const & rhs) {
   if (!(g.mesh() == rhs.mesh()))  TRIQS_RUNTIME_ERROR<<"Gf Assignment in View : incompatible mesh"<<g.mesh() << " vs "<< rhs.mesh();
   for (auto const & w: g.mesh()) g[w] = rhs[w];
   g.singularity() = rhs.singularity();
  }

 template<typename Variable, typename Target, typename Opt, typename T>
  ENABLE_IF(arrays::is_scalar<T>) triqs_gf_view_assign_delegation( gf_view<Variable,Target,Opt> g, T const & x) {
   gf_view<Variable,Target,Opt>::data_proxy_t::assign_to_scalar(g.data(), x); 
   g.singularity() = x;
  }

 // tool for lazy transformation
  template <typename Tag, typename D, typename Target = matrix_valued, typename Opt=void> struct gf_keeper {
   gf_const_view<D, Target, Opt> g;
  };

 // ---------------------------------- slicing ------------------------------------

  // slice
  template <typename Variable, typename Target, typename Opt, bool IsConst, typename... Args>
  gf_view<Variable, matrix_valued, Opt, IsConst> slice_target(gf_view<Variable, Target, Opt, IsConst> g, Args &&... args) {
   static_assert(std::is_same<Target, matrix_valued>::value, "slice_target only for matrix_valued GF's");
   using arrays::range;
   return {g.mesh(),                                                   g.data()(range(), std::forward<Args>(args)...),
           slice_target(g.singularity(), std::forward<Args>(args)...), g.symmetry()};
  }

  template <typename Variable, typename Target, typename Opt, typename... Args>
  gf_view<Variable, matrix_valued, Opt> slice_target(gf<Variable, Target, Opt> &g, Args &&... args) {
   return slice_target(g(), std::forward<Args>(args)...);
  }

  template <typename Variable, typename Target, typename Opt, typename... Args>
  gf_const_view<Variable, matrix_valued, Opt> slice_target(gf<Variable, Target, Opt> const &g, Args &&... args) {
   return slice_target(g(), std::forward<Args>(args)...);
  }

  // slice to scalar
  template <typename Variable, typename Target, typename Opt, bool IsConst, typename... Args>
  gf_view<Variable, scalar_valued, Opt, IsConst> slice_target_to_scalar(gf_view<Variable, Target, Opt, IsConst> g,
                                                                        Args &&... args) {
   static_assert(std::is_same<Target, matrix_valued>::value, "slice_target only for matrix_valued GF's");
   using arrays::range;
   return {g.mesh(),                                                g.data()(range(), std::forward<Args>(args)...),
           slice_target(g.singularity(), range(args, args + 1)...), g.symmetry()};
  }

  template <typename Variable, typename Target, typename Opt, typename... Args>
  gf_view<Variable, scalar_valued, Opt> slice_target_to_scalar(gf<Variable, Target, Opt> &g, Args &&... args) {
   return slice_target_to_scalar(g(), std::forward<Args>(args)...);
  }

  template <typename Variable, typename Target, typename Opt, typename... Args>
  gf_const_view<Variable, scalar_valued, Opt> slice_target_to_scalar(gf<Variable, Target, Opt> const &g, Args &&... args) {
   return slice_target_to_scalar(g(), std::forward<Args>(args)...);
  }

  // a scalar_valued gf can be viewed as a 1x1 matrix
  template <typename Variable, typename Opt, bool IsConst>
  gf_view<Variable, matrix_valued, Opt, IsConst>
  reinterpret_scalar_valued_gf_as_matrix_valued(gf_view<Variable, scalar_valued, Opt, IsConst> g) {
   typedef typename gf_view<Variable, matrix_valued, Opt, IsConst>::data_view_t a_t;
   auto a = a_t{typename a_t::indexmap_type(join(g.data().shape(), make_shape(1, 1))), g.data().storage()};
   return {g.mesh(), a, g.singularity(), g.symmetry()};
  }

  template <typename Variable, typename Opt>
  gf_view<Variable, matrix_valued, Opt> reinterpret_scalar_valued_gf_as_matrix_valued(gf<Variable, scalar_valued, Opt> &g) {
   return reinterpret_scalar_valued_gf_as_matrix_valued(g());
  }

  template <typename Variable, typename Opt>
  gf_const_view<Variable, matrix_valued, Opt>
  reinterpret_scalar_valued_gf_as_matrix_valued(gf<Variable, scalar_valued, Opt> const &g) {
   return reinterpret_scalar_valued_gf_as_matrix_valued(g());
  }

 /*
    template<typename Variable1,typename Variable2, typename Target, typename Opt, bool V, typename... Args>
    gf_view<Variable2,Target,Opt> slice_mesh (gf_impl<Variable1,Target,Opt,V> const & g, Args... args) {
    return gf_view<Variable2,Target,Opt>(g.mesh().slice(args...), g.data()(g.mesh().slice_get_range(args...),arrays::ellipsis()), g.singularity(), g.symmetry());
    }*/

 namespace gfs_implementation { // implement some default traits

  // -------------------------  default factories ---------------------

  // ----- tensor_valued
  template <int R, typename Var, typename Opt> struct factories<Var, tensor_valued<R>, Opt> {
   typedef gf<Var, tensor_valued<R>, Opt> gf_t;
   typedef tqa::mini_vector<size_t, R> target_shape_t;
   typedef typename gf_t::mesh_t mesh_t;

   static typename gf_t::data_t make_data(mesh_t const &m, target_shape_t shape) {
    typename gf_t::data_t A(shape.front_append(m.size()));
    A() = 0;
    return A;
   }

   static typename gf_t::singularity_t make_singularity(mesh_t const &m, target_shape_t shape) {
    return typename gf_t::singularity_t(shape);
   }
  };

  // ----- matrix_valued
  template <typename Var, typename Opt> struct factories<Var, matrix_valued, Opt> {
   typedef gf<Var, matrix_valued, Opt> gf_t;
   typedef tqa::mini_vector<size_t, 2> target_shape_t;
   typedef typename gf_t::mesh_t mesh_t;

   static typename gf_t::data_t make_data(mesh_t const &m, target_shape_t shape) {
    typename gf_t::data_t A(shape.front_append(m.size()));
    A() = 0;
    return A;
   }

   static typename gf_t::singularity_t make_singularity(mesh_t const &m, target_shape_t shape) {
    return typename gf_t::singularity_t(shape);
   }
  };

  // ----- scalar_valued
  template <typename Var, typename Opt> struct factories<Var, scalar_valued, Opt> {
   typedef gf<Var, scalar_valued, Opt> gf_t;
   struct target_shape_t {
    target_shape_t front_pop() const { // this make the get_target_shape function works in this case...
     return {};
    }
    target_shape_t() = default;
    template <typename T> target_shape_t(utility::mini_vector<T, 0>) {}
   };
   
   typedef typename gf_t::mesh_t mesh_t;

   static typename gf_t::data_t make_data(mesh_t const &m, target_shape_t shape) {
    typename gf_t::data_t A(m.size());
    A() = 0;
    return A;
   }
   static typename gf_t::singularity_t make_singularity(mesh_t const &m, target_shape_t shape) {
    return typename gf_t::singularity_t{1, 1};
   }
  };

 // ---------------------   hdf5 ---------------------------------------
  // scalar function : just add a _s
  template <typename Variable, typename Opt> struct h5_name<Variable, scalar_valued, Opt> {
   static std::string invoke() { return h5_name<Variable, matrix_valued, Opt>::invoke() + "_s"; }
  };

  // the h5 write and read of gf members, so that we can specialize it e.g. for block gf
  template <typename Variable, typename Target, typename Opt> struct h5_rw {

   static void write(h5::group gr, gf_const_view<Variable, Target, Opt> g) {
    h5_write(gr, "data", g._data);
    h5_write(gr, "singularity", g._singularity);
    h5_write(gr, "mesh", g._mesh);
    h5_write(gr, "symmetry", g._symmetry);
   }

   template <bool B> static void read(h5::group gr, gf_impl<Variable, Target, Opt, B, false> &g) {
    h5_read(gr, "data", g._data);
    h5_read(gr, "singularity", g._singularity);
    h5_read(gr, "mesh", g._mesh);
    h5_read(gr, "symmetry", g._symmetry);
   }
  };
 } // gfs_implementation


}}

// same as for arrays : views can not be swapped by the std::swap. Delete it
namespace std {
 template <typename Variable, typename Target, typename Opt, bool C1, bool C2>
  void swap(triqs::gfs::gf_view<Variable, Target, Opt, C1> &a, triqs::gfs::gf_view<Variable, Target, Opt, C2> &b) = delete;
}
#include "./gf_expr.hpp"
#endif
