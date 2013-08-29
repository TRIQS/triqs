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
#ifndef TRIQS_ARRAYS_HDF5_ARRAY_PROXY_H
#define TRIQS_ARRAYS_HDF5_ARRAY_PROXY_H
#include "./array_proxy_impl.hpp"

namespace triqs { namespace arrays { 
 namespace h5 { 

  /// The array proxy
  template<typename ValueType, int Rank, int Rank_f = Rank >
   class array_proxy : TRIQS_CONCEPT_TAG_NAME(ImmutableArray), // WRONG ! IT does not yet implement [ ] 
   public sliceable_object < ValueType,
   h5::index_system<Rank,Rank_f>, 
   array_proxy_option<Rank_f>,
   Tag::h5_array_proxy, 
   ViewFactory,
   indexmaps::slicer,
   array_proxy<ValueType,Rank, Rank_f> >
  {
   public :    
    typedef ValueType value_type;
    typedef std::pair< boost::shared_ptr<H5::CommonFG> ,std::string> storage_type; 
    static const bool T_is_complex = boost::is_complex<ValueType>::value;
    typedef index_system< Rank, Rank_f> indexmap_type;
    static const unsigned int rank = Rank;

    /// Opens a proxy on an existing array. The dataset must exists
    template< class FileGroupType >
     array_proxy ( FileGroupType file_group, std::string const & name) :
      indexmap_ ( indexmap_type(file_group.openDataSet( name.c_str() ).getSpace(),T_is_complex) ) { 
       if (!h5::exists(file_group, name)) TRIQS_RUNTIME_ERROR<< " h5 : no dataset"<< name << " in file "; 
       storage_ = std::make_pair( boost::make_shared<FileGroupType>(file_group),name);
       DataSet dataset = file_group.openDataSet( name.c_str() );
       try { if (T_is_complex) write_string_attribute(&dataset,"__complex__","1"); } 
       catch (...) {} // catch if the attribute already exists...
      }

    /// Constructs a proxy on a new data set of the dimension of the domain D.  The data must not exist. 
    template< class FileGroupType, class LengthType >
     array_proxy ( FileGroupType file_group, std::string const & name_, LengthType L, bool overwrite = false) :
      indexmap_ ( indexmap_type (L) ) 
   { 
    if (h5::exists(file_group, name_)) {
     if (overwrite) file_group.unlink(name_.c_str());  
     else TRIQS_RUNTIME_ERROR<< " h5 : dataset"<< name_ << " already exists in the file "; 
    }
    storage_ = std::make_pair( boost::make_shared<FileGroupType>(file_group),name_);
    DataSpace ds  = indexmap_.template dataspace<T_is_complex>(); //(indexmap_type::rank_full, &indexmap_.lengths()[0], &indexmap_.strides()[0]  );
    DataSet dataset = file_group.createDataSet( name_.c_str(), data_type_file(ValueType()), ds);
    if (T_is_complex) write_string_attribute(&dataset,"__complex__","1");
   }

    /// Shallow copy
    array_proxy(const array_proxy & X):indexmap_(X.indexmap()),storage_(X.storage_){}

    /// for slice construction
    array_proxy (const indexmap_type & IM, const storage_type & ST): indexmap_(IM),storage_(ST){ }

   public:
    indexmap_type const & indexmap() const {return indexmap_;}
    storage_type const & storage() const {return storage_;}
    storage_type & storage() {return storage_;}
    const H5::CommonFG * file_group() const { return storage_.first.get();}
    std::string const & name() const { return storage_.second;}

    typedef typename indexmap_type::domain_type domain_type; 
    domain_type const & domain() const { return indexmap_.domain();}

    typedef typename domain_type::index_value_type shape_type;
    shape_type const & shape() const { return domain().lengths();}

    size_t num_elements() const { return domain().number_of_elements();}
    bool is_empty() const { return this->num_elements()==0;}

    template< typename ISP>// put in the file...
     void operator=(ISP const & X) { 
      try { 
       BOOST_AUTO(C,  make_const_cache(X, Option::C()));
       //typename result_of::cache<false,Tag::C, ISP >::type C(X);
       DataSet dataset = file_group()->openDataSet( name().c_str() );
       dataset.write( h5::data(C.view()), h5::data_type_mem(C.view()),h5::data_space(C.view()),indexmap().template dataspace<T_is_complex>()); 
      } 
      TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
     }

    template<typename LHS> // from the file to the array or the array_view... 
     friend void triqs_arrays_assign_delegation (LHS & lhs, array_proxy const & rhs)  {
      static_assert((is_amv_value_or_view_class<LHS>::value), "LHS is not a value or a view class "); 
      h5::resize_or_check(lhs,  rhs.indexmap().domain().lengths());
      try { 
       DataSet dataset = rhs.file_group()->openDataSet( rhs.name().c_str() );
       BOOST_AUTO(C,  make_cache(lhs, Option::C() ));
       //typename result_of::cache<true,Tag::C, LHS >::type C(lhs);
       dataset.read( h5::data(C.view()), h5::data_type_mem(C.view()), h5::data_space(C.view()),
	 rhs.indexmap().template dataspace<T_is_complex>() ); 
      }
      TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
     }

   protected:
    indexmap_type indexmap_;
    storage_type storage_;

  };
 }// namesapce h5

 template < class V, int R, int Rf > 
  struct ViewFactory< V, R, h5::array_proxy_option<Rf>, Tag::h5_array_proxy > { typedef h5::array_proxy <V,R,Rf> type; };
}}
#endif

