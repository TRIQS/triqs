/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_ARRAY_H
#define TRIQS_ARRAYS_ARRAY_H
#include <triqs/utility/first_include.hpp>
#include "indexmaps/cuboid/map.hpp"
#include "indexmaps/cuboid/slice.hpp"
#include "impl/indexmap_storage_pair.hpp"
#include "impl/assignment.hpp"
#include "impl/flags.hpp"
namespace triqs { namespace arrays {

 template <typename ValueType, int Rank, ull_t Opt=0, ull_t TraversalOrder= 0, bool Borrowed=false > class array_view;
 template <typename ValueType, int Rank, ull_t Opt=0, ull_t TraversalOrder= 0> class array;

 // ---------------------- array_view  --------------------------------

#define IMPL_TYPE indexmap_storage_pair< indexmaps::cuboid::map<Rank,Opt,TraversalOrder>, \
 storages::shared_block<ValueType, Borrowed>, Opt, TraversalOrder, Tag::array_view > 

 template <typename ValueType, int Rank, ull_t Opt, ull_t TraversalOrder, bool Borrowed>
  class array_view : Tag::array_view, TRIQS_CONCEPT_TAG_NAME(MutableArray), public IMPL_TYPE {
   static_assert( Rank>0, " Rank must be >0");
   public:   
   typedef typename IMPL_TYPE::indexmap_type indexmap_type;
   typedef typename IMPL_TYPE::storage_type storage_type;
   typedef array     <ValueType,Rank,Opt,TraversalOrder>       regular_type;
   typedef array_view<ValueType,Rank,Opt,TraversalOrder>       view_type;
   typedef array_view<ValueType,Rank,Opt,TraversalOrder,true>  weak_view_type;
 
   /// Build from an IndexMap and a storage 
   template<typename S> array_view (indexmap_type const & Ind,S const & Mem): IMPL_TYPE(Ind, Mem) {}

   /// Copy constructor
   array_view(array_view const & X): IMPL_TYPE(X.indexmap(),X.storage()) {}

   /// Build from anything that has an indexmap and a storage compatible with this class
   template<typename ISP> array_view(const ISP & X): IMPL_TYPE(X.indexmap(),X.storage()) {}

#ifdef TRIQS_WITH_PYTHON_SUPPORT
   /// Build from a numpy.array (X is a borrowed reference) : throws if X is not a numpy.array 
   explicit array_view (PyObject * X): IMPL_TYPE(X, false, "array_view "){}
#endif

   array_view () = delete;

   // Move
   array_view(array_view && X) { this->swap_me(X); }

   /// Swap
   friend void swap( array_view & A, array_view & B) { A.swap_me(B);}

   /// Rebind the view
   void rebind (array_view const & X) { this->indexmap_ = X.indexmap_; this->storage_ = X.storage_;}

   /// Assignment. The size of the array MUST match exactly, except in the empty case 
   template<typename RHS> array_view & operator=(RHS const & X) { triqs_arrays_assign_delegation(*this,X); return *this; }

   ///
   array_view & operator=(array_view const & X) { triqs_arrays_assign_delegation(*this,X); return *this; } //without this, the standard = is synthetized...

   // Move assignment not defined : will use the copy = since view must copy data

   TRIQS_DEFINE_COMPOUND_OPERATORS(array_view);
   // to forbid serialization of views...
   //template<class Archive> void serialize(Archive & ar, const unsigned int version) = delete;
  
  };

#undef IMPL_TYPE

 template <typename ValueType, int Rank, ull_t Opt=0, ull_t TraversalOrder=0, bool Borrowed=false>
 using array_const_view = array_view<ValueType, Rank, Opt, TraversalOrder, Borrowed>; 

 //------------------------------- array ---------------------------------------------------

#define IMPL_TYPE indexmap_storage_pair< indexmaps::cuboid::map<Rank,Opt,TraversalOrder>, \
 storages::shared_block<ValueType>, Opt, TraversalOrder, Tag::array_view > 

 template <typename ValueType, int Rank, ull_t Opt, ull_t TraversalOrder>
  class array: Tag::array,  TRIQS_CONCEPT_TAG_NAME(MutableArray), public IMPL_TYPE {
   public:
    typedef typename IMPL_TYPE::value_type value_type;
    typedef typename IMPL_TYPE::storage_type storage_type;
    typedef typename IMPL_TYPE::indexmap_type indexmap_type;
    typedef array     <ValueType,Rank,Opt,TraversalOrder>      regular_type;
    typedef array_view<ValueType,Rank,Opt,TraversalOrder>      view_type;
    typedef array_view<ValueType,Rank,Opt,TraversalOrder,true> weak_view_type;

    /// Empty array.
    explicit array(memory_layout<Rank> ml = memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order)) :IMPL_TYPE(ml){} 

    /// From a domain
    explicit array( typename indexmap_type::domain_type const & dom, memory_layout<Rank> ml = memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order)):
     IMPL_TYPE(indexmap_type(dom,ml)){}

#ifdef TRIQS_DOXYGEN
    /// Construction from the dimensions. NB : the number of parameters must be exactly rank (checked at compile time). 
    array (size_t I_1, .... , size_t I_rank);
#else
#define IMPL(z, NN, unused)                                \
    explicit array (BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(NN), size_t I_),memory_layout<Rank> ml = memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order)): \
    IMPL_TYPE(indexmap_type(mini_vector<size_t,BOOST_PP_INC(NN)>(BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(NN), I_)),ml)) {\
     static_assert(IMPL_TYPE::rank-1==NN,"array : incorrect number of variables in constructor");}
    BOOST_PP_REPEAT(ARRAY_NRANK_MAX , IMPL, nil)
#undef IMPL
#endif

    // Makes a true (deep) copy of the data. 
    array(const array & X): IMPL_TYPE(X.indexmap(),X.storage().clone()) {}

    // Move
    explicit array(array && X) { this->swap_me(X); } 

    // from a temporary storage and an indexmap. Used for reshaping a temporary array
    explicit array(typename indexmap_type::domain_type const & dom, storage_type && sto,
      memory_layout<Rank> ml = memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order)):
     IMPL_TYPE(indexmap_type(dom,ml), std::move(sto)){}

    /** 
     * Build a new array from X.domain() and fill it with by evaluating X. X can be : 
     *  - another type of array, array_view, matrix,.... (any <IndexMap, Storage> pair)
     *  - a expression : e.g. array<int> A = B+ 2*C;
     */
    template <typename T> 
     array(const T & X, TYPE_ENABLE_IF(memory_layout<Rank>, ImmutableCuboidArray<T>) ml = memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order)):
      IMPL_TYPE(indexmap_type(X.domain(),ml)) { triqs_arrays_assign_delegation(*this,X); }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
    ///Build from a numpy.array X (or any object from which numpy can make a numpy.array). Makes a copy.
    explicit array (PyObject * X): IMPL_TYPE(X, true, "array "){}
#endif

    // build from a init_list
    template<typename T, int R=Rank>
     array(std::initializer_list<T> const & l, typename std::enable_if<R==1>::type * dummy =0):
      IMPL_TYPE(indexmap_type(mini_vector<size_t,1>(l.size()),memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order))) {
       size_t i=0;
       for (auto const & x : l) (*this)(i++) = x;
      }

    template<typename T, int R=Rank>
     array (std::initializer_list<std::initializer_list<T>> const & l, typename std::enable_if<R==2>::type * dummy =0):
      IMPL_TYPE(memory_layout<Rank>(IMPL_TYPE::indexmap_type::traversal_order)) {
       size_t i=0,j=0; int s=-1;
       for (auto const & l1 : l) { if (s==-1) s= l1.size(); else if (s != l1.size()) TRIQS_RUNTIME_ERROR << "initializer list not rectangular !";}
       IMPL_TYPE::resize(typename IMPL_TYPE::domain_type (mini_vector<size_t,2>(l.size(),s)));
       for (auto const & l1 : l) {
	for (auto const & x : l1) { (*this)(i,j++) = x;}
	j=0; ++i;
       }
      }

    /** 
     * Resizes the array. NB : all references to the storage is invalidated.
     * Does not initialize the array by default: to resize and init, do resize(IND).init()
     */
    array & resize (const indexmaps::cuboid::domain_t<IMPL_TYPE::rank> & l) { IMPL_TYPE::resize(l); return *this; }

    /// Assignement resizes the array.  All references to the storage are therefore invalidated.
    array & operator=(const array & X) { IMPL_TYPE::resize_and_clone_data(X); return *this; }

    /// Move assignment
    array & operator=(array && X) { this->swap_me(X); return *this;}

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

  };//array class

#undef IMPL_TYPE

 //----------------------------------------------------------------------------------

 // how to build the view type ....
 template < class V, int R, ull_t Opt, ull_t TraversalOrder, bool Borrowed > 
  struct ISPViewType< V, R, Opt, TraversalOrder,Tag::array_view, Borrowed> { typedef array_view<V,R,Opt,TraversalOrder, Borrowed> type; };

}}//namespace triqs::arrays

// The std::swap is WRONG for a view because of the copy/move semantics of view.
// Use swap instead (the correct one, found by ADL).
namespace std { 
 template <typename V, int R,  triqs::ull_t Opt, triqs::ull_t To, bool B1, bool B2>
  void swap( triqs::arrays::array_view<V,R,Opt,To,B1> & a , triqs::arrays::array_view<V,R,Opt,To,B2> & b)= delete;
}

#include "./expression_template/array_algebra.hpp"

#endif

