/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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
#include "./common.hpp"
#include "../storages/shared_block.hpp"
#include "./assignment.hpp"
#include "./print.hpp"
#include "../indexmaps/cuboid/foreach.hpp"
#include "triqs/utility/exceptions.hpp"
#include "triqs/utility/typeid_name.hpp"
#include "triqs/utility/view_tools.hpp"
#include <type_traits>
#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include "../python/numpy_extractor.hpp"
#include "../python/array_view_to_python.hpp"
#endif

#include "triqs/h5/base_public.hpp"

namespace triqs { namespace arrays {

 template<int Rank, class T, class = std17::void_t<>> struct get_memory_layout {
   static auto invoke( T const & ){ return memory_layout_t<Rank>{}; }
 };
 
 template<int Rank, class T> struct get_memory_layout<Rank, T, std17::void_t<decltype(std::declval<T>().memory_layout())>> {
   static auto invoke( T const & x ){ return x.memory_layout(); }
 };

 template<typename A> AUTO_DECL get_shape  (A const & x) RETURN(x.domain().lengths());

 template<typename A> size_t first_dim   (A const & x) { return x.domain().lengths()[0];}
 template<typename A> size_t second_dim  (A const & x) { return x.domain().lengths()[1];}
 template<typename A> size_t third_dim   (A const & x) { return x.domain().lengths()[2];}
 template<typename A> size_t fourth_dim  (A const & x) { return x.domain().lengths()[3];}
 template<typename A> size_t fifth_dim   (A const & x) { return x.domain().lengths()[4];}
 template<typename A> size_t sixth_dim   (A const & x) { return x.domain().lengths()[5];}
 template<typename A> size_t seventh_dim (A const & x) { return x.domain().lengths()[6];}
 template<typename A> size_t eighth_dim  (A const & x) { return x.domain().lengths()[7];}
 template<typename A> size_t ninth_dim   (A const & x) { return x.domain().lengths()[8];}

 template <bool Const, typename IndexMapIterator, typename StorageType > class iterator_adapter;

 template <class V, int R, typename TraversalOrder, class ViewTag, bool Borrowed, bool IsConst> struct ISPViewType;

 // Auxiliary class for the auto_assign of indexmap_storage_pair, proxies.
 // When implementing triqs_clef_auto_assign (A, f), if the result of f is itself a
 // clef expression, we call again triqs_clef_auto_assign.
 // This allows chain calls, cf clef adapter/vector
 // This class is moved out of indexmap_storage_pair to be reused for proxy.
 template <typename ArrayType, typename Function> struct array_auto_assign_worker {
  ArrayType &A;
  Function const &f;
  template <typename T, typename RHS> void FORCEINLINE assign(T &x, RHS &&rhs) { x = std::forward<RHS>(rhs); }
  template <typename Expr, int... Is, typename T> FORCEINLINE void assign(T &x, clef::make_fun_impl<Expr, Is...> &&rhs) {
   triqs_clef_auto_assign(x, std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
  }
  template <typename... Args> FORCEINLINE void operator()(Args const &... args) { this->assign(A(args...), f(args...)); }
 };

 namespace tags { 
 struct _with_lambda_init {};
 }

 //---------------

 template <typename IndexMapType, typename StorageType, typename TraversalOrder, bool IsConst, bool IsView, typename ViewTag>
 class indexmap_storage_pair : Tag::indexmap_storage_pair, TRIQS_CONCEPT_TAG_NAME(MutableCuboidArray) {

   public :
    using value_type=typename StorageType::value_type ;
    //static_assert(std::is_constructible<value_type>::value, "array/array_view and const operate only on values");
    static_assert(!std::is_const<value_type>::value,         "no const type");
    using storage_type=StorageType ;
    using indexmap_type=IndexMapType ;
    using traversal_order_t = typename _get_traversal_order_t<TraversalOrder>::type;
    using traversal_order_arg = TraversalOrder;
    static constexpr int rank = IndexMapType::domain_type::rank;
    static constexpr bool is_const = IsConst;

    static std::string hdf5_scheme() { 
       return "array<" + triqs::h5::get_hdf5_scheme<value_type>() + "," + std::to_string(rank) + ">";
    }
    
   protected:

    indexmap_type indexmap_;
    storage_type storage_;

    // ------------------------------- constructors --------------------------------------------

    indexmap_storage_pair() {}

    indexmap_storage_pair(indexmap_type IM, storage_type ST) : indexmap_(std::move(IM)), storage_(std::move(ST)) {}

     /// The storage is allocated from the size of IM.
     indexmap_storage_pair(const indexmap_type &IM) : indexmap_(IM), storage_() {
      storage_ = StorageType(indexmap_.domain().number_of_elements());
     }

  
   template<typename InitLambda>
    explicit indexmap_storage_pair(tags::_with_lambda_init, indexmap_type IM, InitLambda && lambda): indexmap_(std::move(IM)), storage_() {
      storage_ = StorageType(indexmap_.domain().number_of_elements(), storages::tags::_allocate_only{}); // DO NOT construct the element of the array
       _foreach_on_indexmap (indexmap_, [&](auto const & ...x) { storage_._init_raw(indexmap_(x...), lambda(x...));});
      }

     public:
    // Shallow copy
    indexmap_storage_pair(indexmap_storage_pair const &X) = default;
    indexmap_storage_pair(indexmap_storage_pair &&X) = default;

    protected:
#ifdef TRIQS_WITH_PYTHON_SUPPORT
    indexmap_storage_pair (PyObject * X, bool enforce_copy, const char * name ) {
     numpy_interface::numpy_extractor<value_type, indexmap_type::rank> E;
     bool ok = E.extract(X, enforce_copy);
     if (!ok) TRIQS_RUNTIME_ERROR<< " construction of a "<< name <<" from a numpy  "
       <<"\n   T = "<< triqs::utility::typeid_name(value_type())
       // lead to a nasty link pb ???
       // linker search for IndexMapType::domain_type::rank in triqs.so
       // and cannot resolve it ???
       //<<"\n   rank = "<< IndexMapType::domain_type::rank//this->rank
       <<"\nfrom the python object \n"<< numpy_interface::object_to_string(X)
       <<"\nThe error was :\n "<<E.error;
     indexmap_ = indexmap_type {E.lengths,E.strides,0};
     storage_ = storages::shared_block<value_type> (E.numpy_obj, true);
    }
#endif
    // ------------------------------- swap --------------------------------------------

    void swap_me( indexmap_storage_pair & X) {
     std::swap(this->indexmap_,X.indexmap_); std::swap (this->storage_, X.storage_);
    }

    friend void swap( indexmap_storage_pair & A, indexmap_storage_pair & B) { A.swap_me(B);}

    // ------------------------------- ==  --------------------------------------------
    // at your own risk with floating value, but it is useful for int, string, etc....
    // in particular for tests
    template <typename RHS>
    friend bool operator==(indexmap_storage_pair const & A, RHS const & B) {
     static_assert(is_amv_value_or_view_class<RHS>::value, "Can only compare against array,matrix or vector"); 
     if (A.shape() != B.shape()) return false;
     auto ita = A.begin(); auto itb = B.begin();
     for (;ita != A.end();++ita, ++itb) {if (!(*ita == *itb)) return false;}
     return true;
    }

    template <typename RHS>
    friend bool operator!=( indexmap_storage_pair const & A, RHS const & B) { 
      static_assert(is_amv_value_or_view_class<RHS>::value, "Can only compare against array,matrix or vector"); 
      return (!(A==B));
    }

   public:
    // ------------------------------- data access --------------------------------------------

    auto const & memory_layout() const { return indexmap_.memory_layout(); }
    indexmap_type const & indexmap() const {return indexmap_;}
    storage_type storage() && {return std::move(storage_);}
    storage_type const & storage() const &{return storage_;}
    storage_type & storage() &{return storage_;}

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  PyObject *to_python() const {
   if (is_empty()) TRIQS_RUNTIME_ERROR << "Error : trying to return an empty array/matrix/vector to python";
   return numpy_interface::array_view_to_python(*this);
  }
#endif

    /// data_start is the starting point of the data of the object
    /// this it NOT &storage()[0], which is the start of the underlying blokc
    /// they are not equal for a view in general
    value_type const * restrict data_start() const { return &storage_[indexmap_.start_shift()];}
    value_type * restrict data_start() { return &storage_[indexmap_.start_shift()];}

    using domain_type=typename indexmap_type::domain_type ;
    domain_type const & domain() const { return indexmap_.domain();}

    using shape_type=typename domain_type::index_value_type ;
    shape_type const & shape() const { return domain().lengths();}

    size_t shape(size_t i) const { return domain().lengths()[i];}

    // deprecate this strange name
    size_t num_elements() const { return domain().number_of_elements();}
    size_t size() const { return domain().number_of_elements();}

    //bool is_empty() const { return this->num_elements()==0;}
    bool is_empty() const { return this->storage_.empty();}

    // ------------------------------- operator () --------------------------------------------

    using view_type = typename ISPViewType<value_type, domain_type::rank, TraversalOrder, ViewTag, false, IsConst>::type;
    using weak_view_type = typename ISPViewType<value_type, domain_type::rank, TraversalOrder, ViewTag, true, IsConst>::type;

    // Evaluation and slices
    template<typename... Args>
     std14::enable_if_t<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank==0) && (!IsConst)
     , value_type &>
     operator()(Args const & ... args)  & {  return storage_[indexmap_(args...)]; }

    template<typename... Args>
     std14::enable_if_t<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank==0)
     , value_type const &>
     operator()(Args const & ... args) const & { return storage_[indexmap_(args...)]; }

    // && : return a & iif it is a non const view 
    template<typename... Args>
     std14::enable_if_t<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank==0) && (!IsConst&&IsView)
     , value_type &>
     operator()(Args const & ... args) && {
      // add here a security check in case it is a view, unique. For a regular type, move the result... 
#ifdef TRIQS_ARRAYS_DEBUG
      if (!storage_type::is_weak && storage_.is_unique()) TRIQS_RUNTIME_ERROR <<"triqs::array. Attempting to call an rvalue unique view ...";
#endif
      return storage_[indexmap_(args...)]; 
     }

    // && return a value if this is not a view (regular class) or it is a const_view
    template<typename... Args>
     std14::enable_if_t<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank==0) && (!(!IsConst&&IsView))
     , value_type>
     operator()(Args const & ... args) && { return storage_[indexmap_(args...)]; }

    template<bool is_const, bool ForceBorrowed, typename ... Args> struct result_of_call_as_view {
     using IM2=typename indexmaps::slicer<indexmap_type,Args...>::r_type ;
     //using V2=typename std::conditional<is_const, typename std::add_const<value_type>::type, value_type>::type ;
     using V2=value_type ;
     static_assert(IM2::domain_type::rank !=0, "Internal error");
     typedef typename ISPViewType < V2, IM2::domain_type::rank, typename IM2::traversal_order_in_template, ViewTag,
         ForceBorrowed || StorageType::is_weak, is_const > ::type type;
    };

    template<typename... Args>   // non const version
     typename boost::lazy_enable_if_c<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank!=0) && (!IsConst)
     , result_of_call_as_view<false,true,Args...>
     >::type // enable_if
     operator()(Args const & ... args) & {
      // simplify
      return typename result_of_call_as_view<false,true,Args...>::type ( indexmaps::slicer<indexmap_type,Args...>::invoke(indexmap_,args...), storage()); }

    template<typename... Args>  // const version
     typename boost::lazy_enable_if_c<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank!=0)
     , result_of_call_as_view<true,true,Args...>
     >::type // enable_if
     operator()(Args const & ... args) const & {
      return typename result_of_call_as_view<true,true,Args...>::type ( indexmaps::slicer<indexmap_type,Args...>::invoke(indexmap_,args...), storage()); }

    template<typename... Args>  // rvalue version : same value of weak as this
     typename boost::lazy_enable_if_c<
     (!clef::is_any_lazy<Args...>::value) && (indexmaps::slicer<indexmap_type,Args...>::r_type::domain_type::rank!=0)
     , result_of_call_as_view<false,false,Args...>
     >::type // enable_if
     operator()(Args const & ... args) && {
      //std::cerr  << "slicing a temporary"<< this->storage().is_weak<< result_of_call_as_view<true,true,Args...>::type::storage_type::is_weak << std::endl;
      return typename result_of_call_as_view<false,false,Args...>::type ( indexmaps::slicer<indexmap_type,Args...>::invoke(indexmap_,args...), std::move(storage())); 
     }

    /// Equivalent to make_view
    typename ISPViewType<value_type,domain_type::rank, TraversalOrder, ViewTag, true, true >::type
     operator()() const & { return *this; }

    typename ISPViewType<value_type,domain_type::rank, TraversalOrder, ViewTag, true, IsConst >::type
     operator()() & { return *this; }

    typename ISPViewType<value_type,domain_type::rank, TraversalOrder, ViewTag, StorageType::is_weak,IsConst >::type
     operator()() && { return *this; }

    // Interaction with the CLEF library : calling with any clef expression as argument build a new clef expression
    // NB : this is ok because indexmap_storage_pair has a shallow copy constructor ...
    // Correction : no copy, just a ref...
    // so A(i_) if A is an array will NOT copy the data....
    template< typename... Args>
     typename clef::_result_of::make_expr_call<indexmap_storage_pair const &,Args...>::type
     operator()( Args&&... args ) const & {
      static_assert(sizeof...(Args) <= indexmap_type::rank, "Incorrect number of variable in call");// not perfect : ellipsis ...
      return make_expr_call(*this,std::forward<Args>(args)...);
     }

    template< typename... Args>
     typename clef::_result_of::make_expr_call<indexmap_storage_pair &,Args...>::type
     operator()( Args&&... args ) & {
      static_assert(sizeof...(Args) <= indexmap_type::rank, "Incorrect number of variable in call");// not perfect : ellipsis ...
      return make_expr_call(*this,std::forward<Args>(args)...);
     }

    template< typename... Args>
     typename clef::_result_of::make_expr_call<indexmap_storage_pair,Args...>::type
     operator()( Args&&... args ) && {
      static_assert(sizeof...(Args) <= indexmap_type::rank, "Incorrect number of variable in call");// not perfect : ellipsis ...
      return make_expr_call(std::move(*this),std::forward<Args>(args)...);
     }

     // ------------------------------- clef auto assign --------------------------------------------

    template <typename Fnt> friend void triqs_clef_auto_assign(indexmap_storage_pair &x, Fnt f) {
      foreach (x, array_auto_assign_worker<indexmap_storage_pair, Fnt>{x, f});
     }
     // for views only !
     template <typename Fnt> friend void triqs_clef_auto_assign(indexmap_storage_pair &&x, Fnt f) {
      static_assert(IsView, "Internal errro");
      foreach (x, array_auto_assign_worker<indexmap_storage_pair, Fnt>{x, f});
     }
     // template<typename Fnt> friend void triqs_clef_auto_assign (indexmap_storage_pair & x, Fnt f) { assign_foreach(x,f);}

    // ------------------------------- Iterators --------------------------------------------
    using const_iterator=iterator_adapter<true,typename IndexMapType::iterator, StorageType> ;
    using iterator=iterator_adapter<false,typename IndexMapType::iterator, StorageType> ;
    const_iterator begin() const {return const_iterator(indexmap(),storage(),false);}
    const_iterator end() const {return const_iterator(indexmap(),storage(),true);}
    const_iterator cbegin() const {return const_iterator(indexmap(),storage(),false);}
    const_iterator cend() const {return const_iterator(indexmap(),storage(),true);}
    iterator begin() {return iterator(indexmap(),storage(),false);}
    iterator end() {return iterator(indexmap(),storage(),true);}

   protected:

    // ------------------------------- resize --------------------------------------------
    //
    void resize (domain_type const & d) {
     indexmap_ = IndexMapType(d,indexmap_.memory_layout());
     // build a new one with the lengths of IND BUT THE SAME layout !
     // optimisation. Construct a storage only if the new index is not compatible (size mismatch).
     if (storage_.size() != indexmap_.domain().number_of_elements())
      storage_ = StorageType(indexmap_.domain().number_of_elements());
    }

    template<typename Xtype>
     void resize_and_clone_data( Xtype const & X) { indexmap_ = X.indexmap(); storage_ = X.storage().clone(); }

    //  BOOST Serialization
    friend class boost::serialization::access;
    template<class Archive>
     void serialize(Archive & ar, const unsigned int version) {
      ar & TRIQS_MAKE_NVP("storage",this->storage_);
      ar & TRIQS_MAKE_NVP("indexmap",this->indexmap_);
     }

    // pretty print of the array
    friend std::ostream & operator << (std::ostream & out, const indexmap_storage_pair & A) {
     if (A.storage().size()==0) out<<"empty ";
     else pretty_print(out,A);
     return out;
    }
  };// end class

}}//namespace triqs::arrays

