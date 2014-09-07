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
#ifndef TRIQS_ARRAYS_ASSIGN2_H_
#define TRIQS_ARRAYS_ASSIGN2_H_
#include "iterator_adapter.hpp"
#include "../indexmaps/cuboid/foreach.hpp"
#include "../storages/memcopy.hpp"

namespace triqs { namespace arrays {

 namespace Tag {struct indexmap_storage_pair{}; }// defined here since needed below...
 
 namespace assignment { template<typename LHS, typename RHS, char OP, typename Enable = void>  struct impl; }

 // puts the contents of RHS into LHS. LHS must be an indexmap_storage_pair
 // it is specialized in various cases for optimisation.
 template<typename LHS, typename RHS>
  void triqs_arrays_assign_delegation (LHS & lhs, const RHS & rhs )  { 
   static_assert( !LHS::is_const, "Cannot assign to a const view !");
   assignment::impl<LHS, RHS, 'E'>(lhs, rhs).invoke();
 }

 template<typename LHS, typename RHS, char OP>
  void triqs_arrays_compound_assign_delegation  (LHS & lhs, const RHS & rhs, char_<OP> ) { 
   static_assert( !LHS::is_const, "Cannot apply a compound operator to a const view !");
   assignment::impl<LHS, RHS, OP>(lhs, rhs).invoke();
 }

#define TRIQS_DEFINE_COMPOUND_OPERATORS(MYTYPE)\
 template<typename RHS> MYTYPE & operator +=(RHS const & rhs) { triqs_arrays_compound_assign_delegation (*this,rhs, char_<'A'>()); return *this;}\
 template<typename RHS> MYTYPE & operator -=(RHS const & rhs) { triqs_arrays_compound_assign_delegation (*this,rhs, char_<'S'>()); return *this;}\
 template<typename RHS> MYTYPE & operator *=(RHS const & rhs) { triqs_arrays_compound_assign_delegation (*this,rhs, char_<'M'>()); return *this;}\
 template<typename RHS> MYTYPE & operator /=(RHS const & rhs) { triqs_arrays_compound_assign_delegation (*this,rhs, char_<'D'>()); return *this;}

#define TRIQS_DELETE_COMPOUND_OPERATORS(MYTYPE)\
 template<typename RHS> MYTYPE & operator +=(RHS const & rhs) = delete;\
 template<typename RHS> MYTYPE & operator -=(RHS const & rhs) = delete;\
 template<typename RHS> MYTYPE & operator *=(RHS const & rhs) = delete;\
 template<typename RHS> MYTYPE & operator /=(RHS const & rhs) = delete;

 // -------- IMPLEMENTATION ----------------------------

 namespace assignment {

  template<typename A,typename B, char OP> struct _ops_;
  template<typename A,typename B> struct _ops_ <A,B,'E'> { static void invoke (A & a, B const & b) { a =b;} };
  template<typename A,typename B> struct _ops_ <A,B,'A'> { static void invoke (A & a, B const & b) { a+=b;} };
  template<typename A,typename B> struct _ops_ <A,B,'S'> { static void invoke (A & a, B const & b) { a-=b;} };
  template<typename A,typename B> struct _ops_ <A,B,'M'> { static void invoke (A & a, B const & b) { a*=b;} };
  template<typename A,typename B> struct _ops_ <A,B,'D'> { static void invoke (A & a, B const & b) { a/=b;} };

  // RHS is considered to be an indexmap_storage_pair if it is one, ... except if it is the scalar type of hte LHS
  // think about an Array< Array<T,2> > e.g.
  template<class RHS,class LHS> struct is_isp :
   std::integral_constant<bool, std::is_base_of<Tag::indexmap_storage_pair,RHS>::value && (!is_scalar_for<RHS,LHS>::value) > {};

  /// RHS is special type that defines its own specialization of assign
  template<class RHS,class LHS> struct is_special : std::false_type {};

#define TRIQS_REJECT_ASSIGN_TO_CONST \
  static_assert( (!std::is_const<typename LHS::value_type>::value ), "Assignment : The value type of the LHS is const and cannot be assigned to !");
#define TRIQS_REJECT_MATRIX_COMPOUND_MUL_DIV_NON_SCALAR\
  static_assert( (!((OP=='M' || OP=='D') && MutableMatrix<LHS>::value && (!is_scalar_for<RHS,LHS>::value))),\
    "*= and /= operator for non scalar RHS are deleted for a type modeling MutableMatrix (e.g. matrix, matrix_view) matrix, because this is ambiguous");

  // -----------------    standard assignment for indexmap_storage_pair --------------------------------------------------
  template<typename LHS, typename RHS, char OP>
   struct impl<LHS,RHS, OP, ENABLE_IF(is_isp< RHS,LHS>) > {
    TRIQS_REJECT_ASSIGN_TO_CONST;
    TRIQS_REJECT_MATRIX_COMPOUND_MUL_DIV_NON_SCALAR;

    typedef typename LHS::value_type value_type;
    typedef typename LHS::indexmap_type indexmap_type;
    LHS & lhs; const RHS & rhs; 
    typedef typename std::remove_cv<value_type>::type v_t;
    impl(LHS & lhs_, const RHS & rhs_): lhs(lhs_), rhs(rhs_) {} 

    template<typename ... Args> void operator()(Args const & ... args) const {
     _ops_<typename std::remove_cv<value_type>::type, typename RHS::value_type, OP>::invoke(lhs(args...), rhs(args...)) ;}

     void invoke ()  {
#ifdef TRIQS_ARRAYS_DEBUG
      if (!indexmaps::compatible_for_assignment(lhs.indexmap(), rhs.indexmap())) TRIQS_RUNTIME_ERROR<< "Size mismatch in operation "<<OP <<" : LHS "<< lhs << " \n RHS = "<< rhs;
#endif
      if (( (OP=='E') && indexmaps::raw_copy_possible(lhs.indexmap(), rhs.indexmap()))) {
       storages::memcopy(lhs.data_start(), rhs.data_start(), rhs.indexmap().domain().number_of_elements());
      }
      else { foreach(lhs,*this); }
     }
    };

    // -----------------   assignment for expressions RHS --------------------------------------------------
    template<typename LHS, typename RHS, char OP>
     struct impl<LHS,RHS,OP, ENABLE_IFC( ImmutableCuboidArray<RHS>::value && (!is_scalar_for<RHS,LHS>::value) && (!is_isp<RHS,LHS>::value)&& (!is_special<LHS,RHS>::value)) > {
      TRIQS_REJECT_ASSIGN_TO_CONST;
      TRIQS_REJECT_MATRIX_COMPOUND_MUL_DIV_NON_SCALAR;
      typedef typename LHS::value_type value_type;
      LHS & lhs; const RHS & rhs; 
      impl(LHS & lhs_, const RHS & rhs_): lhs(lhs_), rhs(rhs_) {}
      template<typename ... Args> void operator()(Args const & ... args) const { _ops_<value_type, typename RHS::value_type, OP>::invoke(lhs(args...),rhs(args...));}
      FORCEINLINE void invoke() { foreach(lhs,*this); }
     };

     // help compiler : in the most common case, less things to inline..
     template<typename LHS, typename RHS>
     struct impl<LHS,RHS,'E', ENABLE_IFC( ImmutableCuboidArray<RHS>::value && (!is_scalar_for<RHS,LHS>::value) && (!is_isp<RHS,LHS>::value)&& (!is_special<LHS,RHS>::value)) > {
      TRIQS_REJECT_ASSIGN_TO_CONST;
      typedef typename LHS::value_type value_type;
      LHS & lhs; const RHS & rhs;
      impl(LHS & lhs_, const RHS & rhs_): lhs(lhs_), rhs(rhs_) {}
      FORCEINLINE void invoke() { assign_foreach(lhs, rhs); }
     };

    // -----------------   assignment for scalar RHS, except some matrix case --------------------------------------------------
    template<typename LHS, typename RHS, char OP>
     struct impl<LHS,RHS,OP, ENABLE_IFC(is_scalar_for<RHS,LHS>::value && (!(MutableMatrix<LHS>::value && (OP=='A'||OP=='S'||OP=='E') ))) >{
      TRIQS_REJECT_ASSIGN_TO_CONST;
      typedef typename LHS::value_type value_type;
      LHS & lhs; const RHS & rhs; 
      impl(LHS & lhs_, const RHS & rhs_): lhs(lhs_), rhs(rhs_){}//, p(*(lhs_.data_start())) {}
      template<typename ... Args> void operator()(Args const & ...args) const {_ops_<value_type, RHS, OP>::invoke(lhs(args...), rhs);}
      void invoke() { foreach(lhs,*this);  }
     };

    // -----------------   assignment for scalar RHS for Matrices --------------------------------------------------

    template <typename T, int R> bool kronecker(mini_vector<T,R> const & key) { return ( (R==2) && (key[0]==key[1]));}
    template <typename T> bool kronecker(T const & x0, T const & x1) { return ( (x0==x1));}

    // Specialisation for Matrix Classes : scalar is a unity matrix, and operation is E, A, S, but NOT M, D
    template<typename LHS, typename RHS, char OP>
     struct impl<LHS,RHS,OP, ENABLE_IFC(is_scalar_for<RHS,LHS>::value && (MutableMatrix<LHS>::value && (OP=='A'||OP=='S'||OP=='E')))> {
      TRIQS_REJECT_ASSIGN_TO_CONST;
      typedef typename LHS::value_type value_type;
      LHS & lhs; const RHS & rhs; 
      impl(LHS & lhs_, const RHS & rhs_): lhs(lhs_), rhs(rhs_){} //, p(*(lhs_.data_start())) {}
      // we MUST make off_diag like this, if value_type is a complicated type (i.e. gf, matrix) with a size
      // off diagonal element is 0*rhs, i.e. a 0, but with the SAME SIZE as the diagonal part.
      // otherwise further operation may fail later.
      // TO DO : look at performance issue ?? (we can remote the multiplication by 0 using an auxiliary function)
      template<typename ... Args>
       void operator()(Args const & ... args) const {_ops_<value_type, RHS, OP>::invoke(lhs(args...), (kronecker(args...) ? rhs : RHS{0*rhs}));}
      void invoke() { foreach(lhs,*this); }
     };

#undef TRIQS_REJECT_MATRIX_COMPOUND_MUL_DIV_NON_SCALAR
#undef TRIQS_REJECT_ASSIGN_TO_CONST
   }// assignment
 }}//namespace triqs::arrays
#endif

