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
#ifndef TRIQS_ARRAYS_H5_STACK_H
#define TRIQS_ARRAYS_H5_STACK_H
#include "../array.hpp"
#include <triqs/h5.hpp>
#include "./simple_read_write.hpp"

namespace triqs { namespace arrays {  

 namespace h5_stack_details { // to be replaced by ellipsis 
  template<class T, size_t N, ull_t Opt> array_view<T,N-1,Opt> slice0( array<T,N,Opt> & A, size_t ind);
#define AUX(z,p,unused) BOOST_PP_COMMA_IF(p) range()
#define IMPL(z, NN, unused) \
  template<class T, ull_t Opt> array_view<T,BOOST_PP_INC(NN),Opt> slice0( array<T,BOOST_PP_INC(NN)+1,Opt> & A, size_t ind) {\
   return A(ind,BOOST_PP_REPEAT(BOOST_PP_INC(NN),AUX,nil));}
  BOOST_PP_REPEAT(ARRAY_NRANK_MAX , IMPL, nil);
#undef IMPL
#undef AUX
  template<class T, ull_t Opt> T & slice0( array<T,1,Opt> & A, size_t ind) { return A(ind);}

  template<typename B, typename Enable = void> struct get_value_type { typedef B type; static const size_t rank = 0;};
  template<typename B> struct get_value_type<B, typename boost::enable_if<is_amv_value_class<B> >::type > { 
   typedef typename B::value_type type;static const size_t rank = B::rank;};

 }

 /**
  *  \brief Hdf5 array stack
  *  \tparam  BaseElementType The type of the base element of the stack. Can be an array/matrix/vector or a scalar.
  *           If it in a scalar, the array_stack will write an hdf5 array of dimension 1
  *           If it in an array/matrix/vector, the array_stack will write an hdf5 array of dimension BaseElementType::rank +1 
  *
  */
 template< typename BaseElementType>
  class array_stack {
   typedef BaseElementType base_element_type;
   static_assert( (is_amv_value_class<BaseElementType>::value || is_scalar<BaseElementType>::value), "BaseElementType must be an array/matrix/vector or a simple number");
   typedef typename h5_stack_details::get_value_type<BaseElementType>::type T;
   static const size_t dim = h5_stack_details::get_value_type<BaseElementType>::rank; 
   static const bool base_is_array = dim >0;
   size_t bufsize_, step, _size; 
   static const bool T_is_complex = boost::is_complex<T>::value;
   static const unsigned int RANK = dim + 1 + (T_is_complex ? 1 : 0);
   utility::mini_vector<hsize_t,RANK> dims, offset, maxdims, dim_chunk, buffer_dim, zero;
   H5::DataSet dataset;
   array<T,dim+1> buffer;

   template <typename FileGroupType >
    void construct_delegate ( FileGroupType g, std::string const & name, mini_vector<size_t,dim> const & a_dims, size_t bufsize)  {
     mini_vector<hsize_t,RANK> dim_chunk;
     bufsize_ = bufsize; step = 0; _size =0; 
     for (size_t i =1; i<=dim; ++i) { dims[i] = a_dims[i-1];}
     if (T_is_complex) { dims[RANK-1] =2; }
     maxdims = dims; buffer_dim = dims; dim_chunk = dims;
     dims[0] = 0; maxdims[0] = H5S_UNLIMITED; dim_chunk[0]=1; buffer_dim[0] = bufsize_;
     mini_vector<size_t,dim+1> s; for (size_t i =0; i<=dim; ++i) {s[i] =  buffer_dim[i];} 
     buffer.resize(s);
     H5::DataSpace mspace1( RANK, dims.ptr(), maxdims.ptr());
     H5::DSetCreatPropList cparms; cparms.setChunk( RANK, dim_chunk.ptr() ); // Modify dataset creation properties, i.e. enable chunking.
     try { 
      dataset = g.create_dataset(name,h5::native_type_from_C(typename h5::remove_complex<T>::type()), mspace1, cparms );
      if (boost::is_complex<T>::value)  h5::write_string_attribute(&dataset,"__complex__","1");
     }
     TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
    }

   public :

   /** 
    * \brief Constructor 
    *  \param g The h5 file or group, of type FileGroupType
    *  \param name The name of the hdf5 array in the file/group where the stack will be stored
    *  \param base_element_shape The shape of the base array/matrix/vector [or mini_vector<size_t,0>() for a scalar]
    *  \param bufsize The size of the bufferThe name of the hdf5 array in the file/group where the stack will be stored
    *  \exception The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with stackstrace, cf doc). 
    */
   template <typename FileGroupType >
    array_stack( FileGroupType g, std::string const & name, mini_vector<size_t,dim> const & base_element_shape, size_t bufsize)  {
     construct_delegate ( g, name, base_element_shape, bufsize);
    }

   /** 
    * \brief Constructor : valid only if the base is a scalar
    *  \param g The h5 file or group, of type FileGroupType
    *  \param name The name of the hdf5 array in the file/group where the stack will be stored
    *  \param bufsize The size of the bufferThe name of the hdf5 array in the file/group where the stack will be stored
    *  \exception The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with stackstrace, cf doc). 
    */
   template <typename FileGroupType >
    array_stack( FileGroupType g, std::string const & name, size_t bufsize)  {
     static_assert( (is_scalar<BaseElementType>::value), "This constructor is only available for a scalar BaseElementType");
     construct_delegate ( g, name,mini_vector<size_t,0>() , bufsize);
    }

   ///
   ~array_stack() {flush();}

   /// The type of the base of the stack (a view or a reference)
   typedef typename boost::mpl::if_c< base_is_array , array_view<T,dim>, T &>::type slice_type;

   /**
    * \return A view (for an array/matrix/vector base) or a reference (for a scalar base) to the top of the stack
    *         i.e. the next element to be assigned to
    */
   slice_type operator() () { return h5_stack_details::slice0(buffer, step); } 

   /// Advance the stack by one
   void operator++() { ++step; ++_size; if (step==bufsize_) flush();  } 

   /// Flush the buffer to the disk. Automatically called at destruction. 
   void flush() { save_buffer(); step=0;}

   /** 
    * \brief Add a element onto the stack and advance it by one.
    * S << A is equivalent to S() = A; ++S;
    */ 
   template<class AType> void operator << ( AType const & A) { (*this)() = A; ++(*this);}
   
   /// Current size of the stack
   size_t size() const { return _size;}

   private:
   void save_buffer () {
    if (step==0) return;
    dims[0] += step;
    buffer_dim[0] = step; 
    dataset.extend(dims.ptr());
    H5::DataSpace fspace1 = dataset.getSpace (), mspace = h5_impl::data_space(buffer); 
    fspace1.selectHyperslab( H5S_SELECT_SET, buffer_dim.ptr(), offset.ptr() );
    mspace.selectHyperslab(  H5S_SELECT_SET, buffer_dim.ptr(), zero.ptr() );
    try { dataset.write( h5_impl::get_array_data_ptr(buffer), h5::data_type_memory<T>(), mspace, fspace1 ); }
    TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
    offset [0] += step;
   }
  };
}} // namespace
#endif

