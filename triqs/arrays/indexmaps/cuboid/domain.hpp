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
#ifndef TRIQS_ARRAYS_INDEXMAP_CUBOID_DOMAIN_H
#define TRIQS_ARRAYS_INDEXMAP_CUBOID_DOMAIN_H
#include "../common.hpp"
#include "../range.hpp"
#include "./mem_layout.hpp"
#include <triqs/utility/mini_vector.hpp>
#include "../../impl/exceptions.hpp"
#include <iostream>
#include <sstream>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

namespace triqs { namespace arrays { namespace indexmaps { namespace cuboid {
 using namespace triqs::arrays::permutations;

 /// Standard hyper_rectangular domain for arrays
 template<int Rank>
  class domain_t {
   typedef mini_vector<size_t,Rank> n_uple;
   n_uple lengths_;
   friend class boost::serialization::access;
   template<class Archive> void serialize(Archive & ar, const unsigned int version) { ar & boost::serialization::make_nvp("dimensions",lengths_);}
   public :
   static constexpr unsigned int rank = Rank;
   typedef n_uple index_value_type;

   domain_t () =default;
   domain_t (const domain_t & C) = default;
   domain_t (domain_t && C) { *this = std::move(C);}
   friend void swap( domain_t & a, domain_t & b) { swap(a.lengths_,b.lengths_);}
   domain_t & operator =( domain_t const &) = default;
   domain_t & operator =( domain_t && x) { swap(*this,x); return *this;}

   domain_t (n_uple lengths):lengths_(std::move(lengths)) {}
   domain_t (mini_vector<int,Rank> const & lengths):lengths_(lengths) {}
   domain_t (std::vector<std::size_t> const & l):lengths_() {
    if (!(l.size()==rank)) TRIQS_RUNTIME_ERROR << "cuboid domain_t construction : vector size incorrect : got "<<l.size() <<" while expected "<< rank;
    lengths_ = n_uple(l);
   }
   template <typename ... T> domain_t(size_t i0, T... t): lengths_(i0, t...){}

   size_t number_of_elements() const { return lengths_.product_of_elements();}
   bool operator==(domain_t const & X) const { return this->lengths_ == X.lengths_;}
   bool operator!=(domain_t const & X) const { return !(*this==X);}
   n_uple const & lengths() const { return lengths_;}

   /** Generates the value of the indices of a cuboid_domain.  */
   static constexpr ull_t iteration_order_default = permutations::identity(Rank);
   template <ull_t IterationOrder= iteration_order_default >
    class gal_generator {
     typedef index_value_type indices_type;
     const domain_t * dom;
     indices_type indices_tuple;
     bool atend;
     public:
     gal_generator (const domain_t & P, bool atEnd=false): dom(&P), atend(atEnd) {}
     bool operator==(const gal_generator & IT2) const { assert((IT2.dom == dom)); return ((IT2.atend==atend) );}
     bool operator!=(const gal_generator & IT2) const { return (!operator==(IT2));}
     indices_type const & operator *() const { return indices_tuple;}
     explicit operator bool () const { return !atend;}
     gal_generator & operator++(){ assert(!atend); atend = advance_impl(std::integral_constant<int,0>()); return *this;}
     private:
     template<int r> bool advance_impl(std::integral_constant<int,r>) {
      constexpr int p = permutations::apply(IterationOrder, r);
      if (indices_tuple[p] < dom->lengths()[p]-1) { ++(indices_tuple[p]); return false;}
      indices_tuple[p] = 0;
      return advance_impl(std::integral_constant<int,r+1>());
     }
     bool advance_impl(std::integral_constant<int,rank>) { return true;}
    };

   typedef gal_generator<> generator;

   generator begin() const { return generator(*this,false);}
   generator end() const   { return generator(*this,true);}
   /* End of generator */

   // Check that key in in the domain
   template<class KeyType> void assert_key_in_domain(KeyType const & key) const {
    std::stringstream fs;
    bool res = key_check_impl(std::integral_constant<int,0>(), key,this->lengths_,fs);
    if (!res) TRIQS_ARRAYS_KEY_ERROR << " key out of domain \n" <<fs.str() ;
   }
   template<int r,class KeyType>
    bool key_check_impl (std::integral_constant<int,r>, KeyType const & key, n_uple const & L, std::stringstream & fs ) const {
     //bool cond = (  ( size_t(key[r]) < L[r]));
     bool cond = (  ( size_t(std::get<r>(key)) < L[r]));
     //if (!cond) fs << "key ["<<r<<"] = "<< key[r] <<" is not within [0,"<<L[r]<<"[\n";
     if (!cond) fs << "key ["<<r<<"] = "<< std::get<r>(key) <<" is not within [0,"<<L[r]<<"[\n";
     return key_check_impl(std::integral_constant<int,r+1>(), key,L,fs) && cond;
    }
   template<class KeyType> bool key_check_impl (std::integral_constant<int,rank>, KeyType const &, n_uple const &, std::stringstream &) const { return true;}

   // Check that key in in the domain : variadic form. No need for speed optimisation here, it is just for debug
   template<typename ... Args> void assert_key_in_domain_v (Args const & ... args) const { assert_key_in_domain( std::make_tuple(args...));}

   friend std::ostream & operator<<(std::ostream & out, domain_t const & x){return out<<"Cuboid of rank "<<x.rank<<" and dimensions "<<x.lengths();}

   };
 
 // ------------------------- foreach -----------------------------------------------------

 //typedef size_t foreach_int_type;
 typedef std::ptrdiff_t foreach_int_type; 
 // better to be signed here : 1) on some machine/compiler, it is a lot faster !
 // When used with clef auto assign, e.g. A(i_,j_) = i -2*j, one needs signed arithmetics

#define AUX0(z,P,NNN) constexpr int p##P = mem_layout::memory_rank_to_index(TraversalOrder,NNN-P);
#define AUX1(z,P,unused) for (t[p##P]=0; t[p##P]< l[p##P]; ++t[p##P])
//#define AUX1(z,P,unused) for (foreach_int_type x##P=0; x##P< dom.lengths()[p##P]; ++x##P)
#define AUX3(z,p,unused) BOOST_PP_COMMA_IF(p) t[p]  
#define IMPL(z, RR, unused) \
 template<ull_t TraversalOrder, typename FntType> void foreach(domain_t<RR> const & dom, FntType F) {\
    BOOST_PP_REPEAT(RR,AUX0,BOOST_PP_DEC(RR))\
   mini_vector<foreach_int_type,RR> t;\
   const mini_vector<foreach_int_type,RR>  l(dom.lengths());\
   BOOST_PP_REPEAT(RR,AUX1,nil){ F(BOOST_PP_REPEAT(RR,AUX3,nil)); }}
 BOOST_PP_REPEAT_FROM_TO(1,ARRAY_NRANK_MAX , IMPL, nil);
#undef IMPL
#undef AUX0
#undef AUX1
#undef AUX3
}
/// ------------  Pretty Printing : specializing the default behaviour for d=1,2  -------------------------
namespace PrettyPrint_details {
 template<int R, typename A>
   struct print_impl {
    std::ostream & out; A const & a; 
    print_impl( std::ostream & out_, A const & a_) : out(out_), a(a_){}  
    template<typename A0, typename ... Args> void operator()(A0 const & a0, Args const & ... args) const { out << a(a0,args...)<< " ";}
    void print() const { out<<"["; 
     indexmaps::cuboid::foreach<permutations::identity(A::domain_type::rank)> (a.domain(), std::cref(*this)); //foreach(a, std::cref(*this)); 
     out<<"]"; }
   };

 template<typename A>
  struct print_impl <1,A> {
   std::ostream & out; A const & a; 
   print_impl( std::ostream & out_, A const & a_) : out(out_), a(a_){}  
   void print() const {  
    auto d = a.indexmap().domain();
    out<<"["; for (size_t i=0; i< d.lengths()[0]; ++i) out<<(i>0 ? ",": "")<<a(i); out<<"]";}
  };

 template<typename A>
  struct print_impl <2,A> {
   std::ostream & out; A const & a; 
   print_impl( std::ostream & out_, A const & a_) : out(out_), a(a_){}  
   void print() const {  
    auto d = a.indexmap().domain();
    out<<"\n[";
    for (size_t i=0; i< d.lengths()[0]; ++i) {
     out<<(i==0 ? "[" : " [");
     for (size_t j=0; j< d.lengths()[1]; ++j) out<<(j>0 ? ",": "")<<a(i,j);
     out<<"]"<<(i==d.lengths()[0]-1 ? "" :  "\n");
    }
    out<<"]";
   }
  };

}
template<typename A>
void pretty_print (std::ostream & out, A const & a ) { PrettyPrint_details::print_impl<A::domain_type::rank,A>(out,a).print();}

}}}
#endif
