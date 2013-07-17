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
#ifndef TRIQS_ARRAYS_MATRIX_H
#define TRIQS_ARRAYS_MATRIX_H
#include "indexmaps/cuboid/map.hpp"
#include "indexmaps/cuboid/slice.hpp"
#include "impl/indexmap_storage_pair.hpp"
#include "impl/assignment.hpp"
#include "vector.hpp"
namespace triqs { namespace arrays {

 template <typename ValueType, ull_t Opt=0, ull_t TraversalOrder= 0> class matrix_view;
 template <typename ValueType, ull_t Opt=0, ull_t TraversalOrder= 0> class matrix;
 
 // ---------------------- matrix --------------------------------
 //
#define _IMPL_MATRIX_COMMON \
 size_t dim0() const { return this->shape()[0];}\
 size_t dim1() const { return this->shape()[1];}\
 bool is_square() const { return dim0() == dim1();}\
 \
 view_type transpose() const {\
  typename indexmap_type::lengths_type l; l[0] = this->indexmap().lengths()[1];l[1] = this->indexmap().lengths()[0];\
  typename indexmap_type::strides_type s; s[0] = this->indexmap().strides()[1];s[1] = this->indexmap().strides()[0];\
  return view_type( indexmap_type(l,s, this->indexmap().start_shift()), this->storage());\
 }\
 bool memory_layout_is_c() const {   return this->indexmap().strides()[0] >= this->indexmap().strides()[1]; } \
 bool memory_layout_is_fortran() const { return this->indexmap().strides()[0] < this->indexmap().strides()[1]; }

#define IMPL_TYPE indexmap_storage_pair < indexmaps::cuboid::map<2,Opt,TraversalOrder>, \
 storages::shared_block<ValueType>, Opt, TraversalOrder, Tag::matrix_view > 

 template <typename ValueType, ull_t Opt, ull_t TraversalOrder >
  class matrix_view : Tag::matrix_view,  TRIQS_MODEL_CONCEPT(MutableMatrix), public IMPL_TYPE {
   public :
    typedef matrix_view<ValueType,Opt,TraversalOrder> view_type;
    typedef matrix<ValueType,Opt,TraversalOrder>      non_view_type;
    typedef void has_view_type_tag;

    typedef typename IMPL_TYPE::indexmap_type indexmap_type;
    typedef typename IMPL_TYPE::storage_type storage_type;

    /// Build from an IndexMap and a storage 
    template<typename S> matrix_view (typename IMPL_TYPE::indexmap_type const & Ind,S const & Mem): IMPL_TYPE(Ind, Mem) {}

    /// Build from anything that has an indexmap and a storage compatible with this class
    template<typename ISP> matrix_view(const ISP & X): IMPL_TYPE(X.indexmap(),X.storage()) {}

#ifdef TRIQS_WITH_PYTHON_SUPPORT
    /// Build from a numpy.array : throws if X is not a numpy.array 
    explicit matrix_view (PyObject * X): IMPL_TYPE(X, false, "matrix_view "){}
#endif

    /// Copy construction
    matrix_view( matrix_view const & X): IMPL_TYPE(X.indexmap(),X.storage()) {}

    matrix_view () = delete;

    // Move
    matrix_view(matrix_view && X) { this->swap_me(X); }

    /// Swap
    friend void swap( matrix_view & A, matrix_view & B) { A.swap_me(B);}

    /// Rebind the view
    void rebind (matrix_view const & X) { this->indexmap_ = X.indexmap_; this->storage_ = X.storage_;}

    /** Assignement.  The size of the array MUST match exactly.  */
    template<typename RHS> matrix_view & operator=(const RHS & X) {triqs_arrays_assign_delegation(*this,X); return *this; }

    matrix_view & operator=(matrix_view const & X) {triqs_arrays_assign_delegation(*this,X); return *this; }//cf array_view class comment

    // Move assignment not defined : will use the copy = since view must copy data

    TRIQS_DEFINE_COMPOUND_OPERATORS(matrix_view); 
    _IMPL_MATRIX_COMMON;
  };

 //---------------------------------------------------------------------
 // this traits is used by indexmap_storage_pair, when slicing to find the correct view type.
 template < class V, int R, ull_t OptionFlags, ull_t To > struct ViewFactory< V, R, OptionFlags, To, Tag::matrix_view> { 
  typedef typename std::conditional <R == 1, vector_view<V,OptionFlags >, matrix_view<V,OptionFlags, To > >::type type;
 };

 // ---------------------- matrix --------------------------------

 template <typename ValueType, ull_t Opt, ull_t TraversalOrder >
  class matrix: Tag::matrix,  TRIQS_MODEL_CONCEPT(MutableMatrix), public IMPL_TYPE {
   public :
    typedef typename IMPL_TYPE::value_type value_type;
    typedef typename IMPL_TYPE::storage_type storage_type;
    typedef typename IMPL_TYPE::indexmap_type indexmap_type;
    typedef matrix_view<ValueType,Opt,TraversalOrder> view_type;
    typedef matrix<ValueType,Opt,TraversalOrder> non_view_type;
    typedef void has_view_type_tag;

    /// Empty matrix.
    matrix(memory_layout<2> ml = memory_layout<2>(IMPL_TYPE::indexmap_type::traversal_order) ):  IMPL_TYPE(indexmap_type(ml)) {}

    /// Move
    explicit matrix(matrix && X) { this->swap_me(X); } 

    ///
    matrix(size_t dim1, size_t dim2, memory_layout<2> ml = memory_layout<2>(IMPL_TYPE::indexmap_type::traversal_order) ) :
     IMPL_TYPE(indexmap_type(mini_vector<size_t,2>(dim1,dim2),ml)) {}

    ///
    matrix(mini_vector<size_t,2> const & sha, memory_layout<2> ml = memory_layout<2>(IMPL_TYPE::indexmap_type::traversal_order)) :
     IMPL_TYPE(indexmap_type(sha,ml)) {}

    /** Makes a true (deep) copy of the data. */
    matrix(const matrix & X): IMPL_TYPE(X.indexmap(),X.storage().clone()) {}

    /// Build a new matrix from X.domain() and fill it with by evaluating X. X can be : 
    template <typename T> 
     matrix(const T & X, TYPE_ENABLE_IF(memory_layout<2>, ImmutableArray<T>) ml = memory_layout<2>(IMPL_TYPE::indexmap_type::traversal_order)):
      IMPL_TYPE(indexmap_type(X.domain(),ml)) { triqs_arrays_assign_delegation(*this,X); }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
    ///Build from a numpy.array X (or any object from which numpy can make a numpy.array). Makes a copy.
    explicit matrix (PyObject * X): IMPL_TYPE(X, true, "matrix "){}
#endif

    // build from a init_list
    template<typename T>
     matrix (std::initializer_list<std::initializer_list<T>> const & l):
      IMPL_TYPE(memory_layout<2>(IMPL_TYPE::indexmap_type::traversal_order)) {
       size_t i=0,j=0; int s=-1;
       for (auto const & l1 : l) { if (s==-1) s= l1.size(); else if (s != l1.size()) TRIQS_RUNTIME_ERROR << "initializer list not rectangular !";}
       IMPL_TYPE::resize(typename IMPL_TYPE::domain_type (mini_vector<size_t,2>(l.size(),s)));
       for (auto const & l1 : l) {
	for (auto const & x : l1) { (*this)(i,j++) = x;}
	j=0; ++i;
       }
      }

    /** 
     * Resizes the matrix. NB : all references to the storage is invalidated.
     * Does not initialize the matrix by default
     */
    matrix & resize (size_t n1, size_t n2) { IMPL_TYPE::resize(typename IMPL_TYPE::domain_type (mini_vector<size_t,2>(n1,n2))); return *this;}

    /** 
     * Resizes the matrix. NB : all references to the storage is invalidated.
     * Does not initialize the matrix by default
     */
    matrix & resize (const indexmaps::cuboid::domain_t<IMPL_TYPE::rank> & l) { IMPL_TYPE::resize(l); return *this; }

    /// Assignement resizes the matrix.  All references to the storage are therefore invalidated.
    matrix & operator=(const matrix & X) { IMPL_TYPE::resize_and_clone_data(X); return *this; }

    /// Move assignment
    matrix & operator=(matrix && X) { this->swap_me(X); return *this;}

    /// Swap
    friend void swap( matrix & A, matrix & B) { A.swap_me(B);}

    /** 
     * Assignement resizes the matrix.  All references to the storage are therefore invalidated.
     * NB : to avoid that, do make_view(A) = X instead of A = X
     */
    template<typename RHS> 
     matrix & operator=(const RHS & X) { 
      static_assert(  ImmutableArray<RHS>::value, "Assignment : RHS not supported");
      IMPL_TYPE::resize(X.domain());
      triqs_arrays_assign_delegation(*this,X);
      return *this; 
     }

    TRIQS_DEFINE_COMPOUND_OPERATORS(matrix);
    _IMPL_MATRIX_COMMON;
  };//matrix class
#undef _IMPL_MATRIX_COMMON
#undef IMPL_TYPE

}}//namespace triqs::arrays

// The std::swap is WRONG for a view because of the copy/move semantics of view.
// Use swap instead (the correct one, found by ADL).
namespace std { 
 template <typename V, triqs::ull_t Opt, triqs::ull_t To >
  void swap( triqs::arrays::matrix_view<V,Opt,To> & a , triqs::arrays::matrix_view<V,Opt,To> & b)= delete;
}
#include "./expression_template/matrix_algebra.hpp"
#endif

