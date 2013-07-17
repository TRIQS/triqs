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
#ifndef TRIQS_ARRAYS_HDF5_ARRAY_PROXY_IMPL_H
#define TRIQS_ARRAYS_HDF5_ARRAY_PROXY_IMPL_H
//#define TRIQS_ARRAYS_DEBUG_H5_SLICE

#include "../indexmaps/cuboid/domain.hpp"
#include "./group_or_file.hpp"
#include "../impl/sliceable_object.hpp"
#include "../impl/tuple_tools.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace triqs { namespace arrays { 

 namespace h5 { template<int Rf> struct array_proxy_option {}; } 

 namespace Option { 
  template<class IM2, int Rf> struct Im_Opt_2_Opt<IM2, h5::array_proxy_option<Rf> > { typedef h5::array_proxy_option<Rf> type;}; 
 }

 namespace h5 {

  template<int Rank, int Rank_full>
   class index_system { 

    public :
     static const unsigned int rank= Rank, rank_full = Rank_full;
     typedef mini_vector<size_t, Rank>      v_type;
     typedef mini_vector<size_t, Rank_full> v_type_full;
     typedef indexmaps::cuboid::domain<Rank> domain_type;
     domain_type const & domain() const { return mydomain;}

     index_system (v_type_full const & total_lengths_) {
      total_lens_ = total_lengths_;  lens_= total_lengths_;
      for (size_t i =0; i<rank_full; ++i)  stri_[i] = 1; 
      for (size_t i =0; i<rank; ++i)  dims[i] = lens_[i]; 
      mydomain = domain_type (dims);
     }

     index_system (v_type const & dims_, v_type_full const & total_lengths_,  v_type_full const & lengths_, 
       v_type_full const & strides_, v_type_full const & offset_ ) {
      dims = dims_;total_lens_ = total_lengths_;  lens_= lengths_; stri_ = strides_; off_=offset_;
      mydomain = domain_type (dims);
     }

     index_system (DataSpace const & ds, bool is_complex) { 
      int rf = ds.getSimpleExtentNdims();
      if ( rf != rank_full  + (is_complex ? 1 : 0) ) TRIQS_RUNTIME_ERROR <<  "H5 : dimension error";
      //int ndims = ds.getSimpleExtentDims( &lens_[0], NULL);
      ds.getSimpleExtentDims( &lens_[0], NULL);
      for (size_t i =0; i<rank; ++i) { dims[i] = lens_[i]; stri_[i] = 1; off_[i]= 0; }
      total_lens_=dims;
      mydomain = domain_type (dims);
     }

     template<bool C> DataSpace dataspace() const { return h5::dataspace_from_LS<rank_full,C> (total_lens_, lens_,stri_,off_); }
     size_t size() const { size_t _size = 1; for (size_t i =0; i<rank; ++i) _size *= lens_[i]; return _size;}
     mini_vector<size_t, rank_full> total_lengths() const { return this->total_lens_;}
     mini_vector<size_t, rank_full> lengths() const { return this->lens_;}
     mini_vector<size_t, rank_full> strides() const { return this->stri_;}

    private: 
     mini_vector<hsize_t,rank_full> lens_, off_, stri_; 
     v_type dims;
     v_type_full total_lens_;
     domain_type mydomain;
   };

  namespace slicer_impl { 

   template <bool BC> inline void _check_BC ( int N, int ind, size_t B) { }
   template <> inline void _check_BC<true> (int N, int ind, size_t B) { 
    bool cond = (ind >= 0) && (ind < int(B)); // fix this int ...
    if (!cond) TRIQS_ARRAYS_KEY_ERROR << " index "<<N<<" is out of domain: \n    " << ind <<" is not within [0,"<< B <<"[\n";
   }

   template<int Rank_in, int Rank_out, int N,int P, int c, bool BoundCheck> struct slice_calc { 

    typedef mini_vector<size_t,Rank_in> const &  i_type;
    typedef mini_vector<size_t,Rank_in > &       o_type;
    typedef mini_vector<size_t,Rank_out > &      os_type; 

    template< typename ArgsTuple>
     static void invoke(i_type li, i_type si, os_type lo_c, o_type lo,  o_type so, o_type offset, ArgsTuple const & args ) {
      const int dP = boost::is_base_of<typename boost::tuples::element<N,ArgsTuple>::type, range >::type::value ;
      one_step(li[N], si[N],lo[N],so[N], offset[N] ,boost::tuples::get<N>(args));
      lo_c[P] = lo[N];
      slice_calc<Rank_in,Rank_out,N+1,P+dP,c-1, BoundCheck>::invoke(li,si,lo_c,lo,so, offset, args);
     }

    static void one_step(size_t li, size_t si, size_t & lo, size_t & so, size_t & offset, size_t R){
     _check_BC <BoundCheck> (N, R, li);
     offset = R;  lo =1; so= 1;
    }

    static void one_step(size_t li, size_t si, size_t & lo, size_t & so, size_t & offset, range R){
     _check_BC <BoundCheck> (N, R.first(),li);
     _check_BC <BoundCheck> (N, (R.last()==-1 ? li : R.last()) -1 ,li);
     lo  = ((R.last()==-1 ? li : R.last()) - R.first() + R.step()-1 )/R.step() ; //  python behaviour
     so  = R.step();  offset = R.first()  ;
    }
   };
   // stop the recursion
   template<int Ri, int Ro, int N, int P, bool BC> struct slice_calc <Ri,Ro,N,P,0,BC> : slice_calc<Ri,Ro,N,P,1,BC> {
    template<class T1,class T2,class T3,class T4,class T5,class T6, class T7> static void invoke(T1,T2,T3,T4,T5,T6,T7 ) {}
   };

  }//namespace slicer_impl 
 } // h5

 namespace indexmaps { 

  template<int R, int Rf, typename ArgsTuple> 
   struct slicer < h5::index_system<R,Rf>,  ArgsTuple>  { 

    static const unsigned int len = boost::tuples::length<ArgsTuple>::value;
    static_assert(len>=R, "Too few arguments in slice"); 
    static_assert(len<=R, "Too many arguments in slice");
    static_assert( (R==Rf), "Can not slice array_proxy twice (not implemented)");

    static const unsigned int R2 = R - TupleTools::CountHowManyInt<ArgsTuple>::value;
    typedef h5::index_system< R2, Rf> return_type; 

    static return_type invoke ( h5::index_system<R,R> const & X, ArgsTuple args) { 
     mini_vector<size_t, R2> newdims;
     mini_vector<size_t,R> newoffset, newlengths, newstrides;
     h5::slicer_impl::slice_calc<R,R2,0,0,R,true>::invoke(X.lengths(),X.strides(),newdims, newlengths,newstrides, newoffset, args);
#ifdef TRIQS_ARRAYS_DEBUG_H5_SLICE
     std::cerr<<"-----------------------------------------------"<<std::endl;
     std::cerr<<"H5 Slicing "<< X.lengths().to_string()<<X.strides().to_string()<<newlengths.to_string()<<newstrides.to_string()<< newoffset.to_string() << args<<std::endl;
#endif
     return return_type(newdims,X.total_lengths(), newlengths,newstrides,newoffset);
    };
   }; 

  template<int R, int Rf> struct slicer < h5::index_system<R,Rf>, boost::tuple<> >  { typedef h5::index_system< R,Rf>  return_type;}; 
 }

}}
#endif

