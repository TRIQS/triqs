/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include "indexmaps/cuboid/map.hpp"
#include "indexmaps/cuboid/slice.hpp"
#include "impl/indexmap_storage_pair.hpp"
#include "impl/assignment.hpp"
namespace triqs { namespace arrays {

 template <typename ValueType, int Rank, typename TraversalOrder= void, bool Borrowed = false, bool IsConst = false> class array_view;
 template <typename ValueType, int Rank, typename TraversalOrder= void> class array;

 // ---------------------- array_view  --------------------------------

#define IMPL_TYPE                                                                                                                \
 indexmap_storage_pair<indexmaps::cuboid::map<Rank,TraversalOrder>, storages::shared_block<ValueType, Borrowed>, TraversalOrder, IsConst, true, \
                       Tag::array_view>

 template <typename ValueType, int Rank, typename TraversalOrder, bool Borrowed, bool IsConst>
  class array_view: Tag::array_view, TRIQS_CONCEPT_TAG_NAME(MutableArray), public IMPL_TYPE {
   static_assert( Rank>0, " Rank must be >0");

   public:
   using indexmap_type = typename IMPL_TYPE::indexmap_type;
   using storage_type = typename IMPL_TYPE::storage_type;
   using regular_type = array<ValueType, Rank, TraversalOrder>;
   using view_type = array_view<ValueType, Rank, TraversalOrder>;
   using const_view_type = array_view<ValueType, Rank, TraversalOrder, false, true>;
   using weak_view_type = array_view<ValueType, Rank, TraversalOrder, true>;

   /// Build from an IndexMap and a storage
   template <typename S> array_view(indexmap_type const& Ind, S const& Mem) : IMPL_TYPE(Ind, Mem) {}

   /// Copy constructor
   array_view(array_view const& X) : IMPL_TYPE(X.indexmap(), X.storage()) {}

   /// Build from anything that has an indexmap and a storage compatible with this class
   template <typename ISP> array_view(const ISP& X) : IMPL_TYPE(X.indexmap(), X.storage()) {
    // to be activated
    static_assert(IsConst || (!ISP::is_const), "Cannot construct a non const view from a const one !");
   }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
   /// Build from a numpy.array (X is a borrowed reference) : throws if X is not a numpy.array 
   explicit array_view (PyObject * X): IMPL_TYPE(X, false, "array_view "){}
#endif

   array_view () = default;

   // Move
   array_view(array_view && X) noexcept { this->swap_me(X); }

   /// Swap
   friend void swap( array_view & A, array_view & B) { A.swap_me(B);}

   /// Rebind the view
   void rebind(array_view const& X) {
    this->indexmap_ = X.indexmap_;
    this->storage_ = X.storage_;
   }

   // rebind the other view, iif this is const, and the other is not.
   template <typename To, bool C = IsConst> ENABLE_IFC(C) rebind(array_view<ValueType, Rank, To, Borrowed, !IsConst> const& X) {
    this->indexmap_ = X.indexmap();
    this->storage_ = X.storage();
   }

   /// Assignment. The size of the array MUST match exactly, except in the empty case 
   template<typename RHS> array_view & operator=(RHS const & X) { triqs_arrays_assign_delegation(*this,X); return *this; }

   ///
   array_view & operator=(array_view const & X) { triqs_arrays_assign_delegation(*this,X); return *this; } //without this, the standard = is synthetized...

   // Move assignment not defined : will use the copy = since view must copy data

   TRIQS_DEFINE_COMPOUND_OPERATORS(array_view);
   // to forbid serialization of views...
   //template<class Archive> void serialize(Archive & ar, const unsigned int version) = delete;

   template <typename... INT> friend view_type transposed_view(array_view const& a, INT... is) {
    return transposed_view(a,  mini_vector<int, Rank>{is...});
   }

   friend view_type transposed_view(array_view const& a, mini_vector<int, Rank> const& perm) {
    return {transpose(a.indexmap_, perm), a.storage_};
   }

   friend view_type c_ordered_transposed_view(array_view const& a) {
    return transposed_view(a, a.indexmap().memory_layout().get_memory_positions());
   }
 };
#undef IMPL_TYPE

 template <typename ValueType, int Rank, typename TraversalOrder = void, bool Borrowed = false>
 using array_const_view = array_view<ValueType, Rank, TraversalOrder, Borrowed, true>;

 //------------------------------- array ---------------------------------------------------

#define IMPL_TYPE                                                                                                                \
 indexmap_storage_pair<indexmaps::cuboid::map<Rank,TraversalOrder>, storages::shared_block<ValueType>, TraversalOrder, false, false, Tag::array_view>

 template <typename ValueType, int Rank, typename TraversalOrder>
 class array : Tag::array, TRIQS_CONCEPT_TAG_NAME(MutableArray), public IMPL_TYPE {
  public:
  using value_type = typename IMPL_TYPE::value_type;
  using storage_type = typename IMPL_TYPE::storage_type;
  using indexmap_type = typename IMPL_TYPE::indexmap_type;
  using regular_type = array<ValueType, Rank, TraversalOrder>;
  using view_type = array_view<ValueType, Rank, TraversalOrder>;
  using const_view_type = array_view<ValueType, Rank, TraversalOrder, false, true>;
  using weak_view_type = array_view<ValueType, Rank, TraversalOrder, true>;

  /// Empty array.
  explicit array(memory_layout_t<Rank> ml = memory_layout_t<Rank>{}) : IMPL_TYPE(ml) {}

  /// From a domain
  explicit array(typename indexmap_type::domain_type const& dom, memory_layout_t<Rank> ml = memory_layout_t<Rank>{})
     : IMPL_TYPE(indexmap_type(dom, ml)) {}

 /// From shape
  template<typename InitLambda>
   explicit array(typename indexmap_type::domain_type const& dom, InitLambda && lambda)
     : IMPL_TYPE(tags::_with_lambda_init{}, indexmap_type(dom), std::forward<InitLambda>(lambda)) {}

#ifdef TRIQS_DOXYGEN
    /// Construction from the dimensions. NB : the number of parameters must be exactly rank (checked at compile time). 
    array (size_t I_1, .... , size_t I_rank);
#else
#define IMPL(z, NN, unused)                                \
    explicit array (BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(NN), size_t I_),memory_layout_t<Rank> ml = memory_layout_t<Rank>{}): \
    IMPL_TYPE(indexmap_type(mini_vector<size_t,BOOST_PP_INC(NN)>(BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(NN), I_)),ml)) {\
     static_assert(IMPL_TYPE::rank-1==NN,"array : incorrect number of variables in constructor");}
    BOOST_PP_REPEAT(ARRAY_NRANK_MAX , IMPL, nil)
#undef IMPL
#endif

    // Makes a true (deep) copy of the data. 
    array(const array & X): IMPL_TYPE(X.indexmap(),X.storage().clone()) {}

    // Move
    explicit array(array && X) noexcept{ this->swap_me(X); } 

    // Makes a true (deep) copy of the data. 
    explicit array(const array & X, memory_layout_t<Rank> ml): array(X.indexmap().domain(), ml){
      triqs_arrays_assign_delegation(*this,X);
    }

    // from a temporary storage and an indexmap. Used for reshaping a temporary array
    explicit array(typename indexmap_type::domain_type const& dom, storage_type&& sto, memory_layout_t<Rank> ml = memory_layout_t<Rank>{})
       : IMPL_TYPE(indexmap_type(dom, ml), std::move(sto)) {}

    /** 
     * Build a new array from X.domain() and fill it with by evaluating X. X can be : 
     *  - another type of array, array_view, matrix,.... (any <IndexMap, Storage> pair)
     *  - the memory layout will be as given (ml)
     *  - a expression : e.g. array<int> A = B+ 2*C;
     */
    template <typename T>
    array(const T& X,
          std14::enable_if_t<ImmutableCuboidArray<T>::value && std::is_convertible<typename T::value_type, value_type>::value,
                             memory_layout_t<Rank>> ml)
       : IMPL_TYPE(indexmap_type(X.domain(), ml)) {
     triqs_arrays_assign_delegation(*this, X);
    }

    /** 
     * Build a new array from X.domain() and fill it with by evaluating X. X can be : 
     *  - another type of array, array_view, matrix,.... (any <IndexMap, Storage> pair)
     *  - the memory layout will be deduced from X if possible, or default constructed
     *  - a expression : e.g. array<int> A = B+ 2*C;
     */
    template <typename T>
    array(const T& X,
          std14::enable_if_t<ImmutableCuboidArray<T>::value && std::is_convertible<typename T::value_type, value_type>::value, void*> _unused = nullptr )
       : IMPL_TYPE(indexmap_type(X.domain(), get_memory_layout<Rank, T>::invoke(X))) {
     triqs_arrays_assign_delegation(*this, X);
    }


#ifdef TRIQS_WITH_PYTHON_SUPPORT
    ///Build from a numpy.array X (or any object from which numpy can make a numpy.array). Makes a copy.
    explicit array (PyObject * X): IMPL_TYPE(X, true, "array "){}
#endif

    // build from a init_list
    template<typename T>
     array(std::initializer_list<T> const & l, memory_layout_t<1> ml = memory_layout_t<1>{}, std14::enable_if_t<std::is_constructible<value_type, T>::value> * dummy =0):
      IMPL_TYPE(indexmap_type(mini_vector<size_t,1>(l.size()),memory_layout_t<1>{std::move(ml)})) {
       size_t i=0;
       for (auto const & x : l) (*this)(i++) = x;
      }

    template<typename T>
     array (std::initializer_list<std::initializer_list<T>> const & l, memory_layout_t<2> ml = memory_layout_t<2>{}, std14::enable_if_t<std::is_constructible<value_type, T>::value > * dummy =0):
      IMPL_TYPE(memory_layout_t<2>{std::move(ml)}) {
       size_t i=0,j=0;

       // FIXME Enforce this check at compile time
       if (!std::all_of(l.begin(), l.end(), [s = l.begin()->size()](auto const & x){ return x.size() == s; }))
	 TRIQS_RUNTIME_ERROR << "initializer list not rectangular !";

       IMPL_TYPE::resize(typename IMPL_TYPE::domain_type (mini_vector<size_t,2>(l.size(), l.begin()->size())));
       for (auto const & l1 : l) {
	for (auto const & x : l1) { (*this)(i,j++) = x;}
	j=0; ++i;
       }
      }

    template<typename T>
     array (std::initializer_list<std::initializer_list<std::initializer_list<T>>> const & l, memory_layout_t<3> ml = memory_layout_t<3>{}, std14::enable_if_t<std::is_constructible<value_type, T>::value > * dummy =0):
      IMPL_TYPE(memory_layout_t<3>{std::move(ml)}) {
       size_t i=0,j=0,k=0;

       // FIXME Enforce this check at compile time
       if (!std::all_of(l.begin(), l.end(), [s = l.begin()->size()](auto const & x){ return x.size() == s; }))
	 TRIQS_RUNTIME_ERROR << "initializer list not rectangular !";

       for (auto const & l1 : l)
         if (!std::all_of(l1.begin(), l1.end(), [s = l1.begin()->size()](auto const & x){ return x.size() == s; }))
	   TRIQS_RUNTIME_ERROR << "initializer list not rectangular !";

       IMPL_TYPE::resize(typename IMPL_TYPE::domain_type (mini_vector<size_t,3>(l.size(), l.begin()->size(), l.begin()->begin()->size())));
       for (auto const & l1 : l) {
        for (auto const & l2 : l1) {
	 for (auto const & x : l2)
	  (*this)(i,j,k++) = x;
	 k=0; ++j;
        }
        j=0; ++i;
       }
      }

    /** 
     * Resizes the array. NB : all references to the storage is invalidated.
     * Does not initialize the array by default: to resize and init, do resize(IND).init()
     *
     */
    template<typename ... Args>
    array & resize (Args && ... args) { 
     static_assert(std::is_copy_constructible<ValueType>::value, "Can not resize an array if its value_type is not copy constructible");
     IMPL_TYPE::resize(indexmaps::cuboid::domain_t<IMPL_TYPE::rank>(args...)); return *this; 
    }

    /// Assignement resizes the array.  All references to the storage are therefore invalidated.
    array & operator=(const array & X) { IMPL_TYPE::resize_and_clone_data(X); return *this; }

    /// Move assignment
    array & operator=(array && X) noexcept{ this->swap_me(X); return *this;}

    /// Swap
    friend void swap( array & A, array & B) { A.swap_me(B);}

    /** 
     * Assignement resizes the array (if necessary).
     * All references to the storage are therefore invalidated.
     * NB : to avoid that, do make_view(A) = X instead of A = X
     */
    template<typename RHS> 
     array & operator=(const RHS & X) { 
      static_assert(ImmutableCuboidArray<RHS>::value, "Assignment : RHS not supported");
      IMPL_TYPE::resize(X.domain());
      triqs_arrays_assign_delegation(*this,X);
      return *this; 
     }

    TRIQS_DEFINE_COMPOUND_OPERATORS(array);

    template <typename... INT> friend const_view_type transposed_view(array const& a, INT... is) {
     return transposed_view(a(), is...);
    };
    template <typename... INT> friend view_type transposed_view(array& a, INT... is) {
     return transposed_view(a(), is...);
    };

  };//array class

#undef IMPL_TYPE

  /// Make a array of zeros with the given dimensions.
  /// FIXME : add variadic form
  /// FIXME : add eyes 
  template<typename T, int R> array<T, R> zeros(mini_vector<long, R> const & len) { 
    array<T, R> r (typename array<T, R>::indexmap_type::domain_type{len});
    r() = 0;
    return r;
  }

  // swap two indices i,j
  template<typename A> 
    std::enable_if_t<is_amv_value_or_view_class<std::decay_t<A>>::value, typename std::decay_t<A>::view_type>
  swap_index_view(A && a, int i, int j) { 
   auto imp = a.indexmap();
   auto l= imp.lengths();
   auto s = imp.strides();
   std::swap(l[i], l[j]);
   std::swap(s[i], s[j]);
   using r_t = typename std::decay_t<A>::view_type;
   // FIXME : long only
   auto imp2 = typename r_t::indexmap_type { l, s, static_cast<ptrdiff_t>(imp.start_shift())};
   return r_t{imp2, a.storage()};
  }

 // Rotate the index n to 0, preserving the relative order of the other indices
  template<typename A> 
    std::enable_if_t<is_amv_value_or_view_class<std::decay_t<A>>::value, typename std::decay_t<A>::view_type>
  rotate_index_view(A && a, int n) { 
    typename std::decay_t<A>::view_type r{a};
    for (int i = n; i > 0; --i) r.rebind(swap_index_view(r, i - 1, i));
   return r;
  }


 //----------------------------------------------------------------------------------

 // how to build the view type ....
 template <class V, int R, typename TraversalOrder, bool Borrowed, bool IsConst>
 struct ISPViewType<V, R, TraversalOrder, Tag::array_view, Borrowed, IsConst> {
  using type = array_view<V, R, TraversalOrder, Borrowed, IsConst>;
 };
}}//namespace triqs::arrays

// The std::swap is WRONG for a view because of the copy/move semantics of view.
// Use swap instead (the correct one, found by ADL).
namespace std {
template <typename V, typename To1, typename To2, int R, bool B1, bool B2, bool C1, bool C2>
void swap(triqs::arrays::array_view<V, R, To1, B1, C1>& a, triqs::arrays::array_view<V, R, To2, B2, C2>& b) = delete;
}

#include "./expression_template/array_algebra.hpp"

