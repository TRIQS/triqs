/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by H. Hafermann, O. Parcollet
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
#ifndef TRIQS_UTILITY_PARAMS_H
#define TRIQS_UTILITY_PARAMS_H
#include "./opaque_object_h5.hpp"
#include "./defaults.hpp"
namespace triqs { namespace utility {
 /**
  * Class for storing program parameters.
  * Parameters can be added to and extracted from the parameter object using the element access operator [].
  * Each element is stored by means of an object of type _object, which also stores the original type (all
  * integral types are collapsed to long and char* is collapsed to std::string).
  * When accessing elements, a typecheck is performed. Typecasts are allowed and are similar to the C++ rules
  * for casts. If the lvalue has type double, a cast from any integral type is allowed. If the lvalue has
  * arithmetic type, a boost::lexical_cast is attempted.
  * The class is boost-serializable and implements hdf5 I/O operations.
  */
 class parameters {
  public :
   parameters() {};
   parameters (parameters const & other) = default;
   parameters (parameters && other) noexcept { swap(*this,other);}
   parameters & operator =  (parameters const & other)  = default;
   parameters & operator =  (parameters && other) noexcept { swap(*this,other); return *this;}
   friend void swap(parameters & a, parameters &b) noexcept { swap(a.object_map,b.object_map);} 

  private:
   typedef std::map<std::string, _object> map_t;
   map_t object_map;
   std::map<std::string, std::string> documentation;

   friend class boost::serialization::access;
   template<class Archive>
    void serialize(Archive & ar, const unsigned int version) { ar & boost::serialization::make_nvp("object_map",object_map); }

  public:

   typedef map_t::const_iterator const_iterator;
   typedef map_t::iterator iterator;
   const_iterator begin() const { return object_map.begin();}
   const_iterator end()  const { return object_map.end();}
   iterator begin() { return object_map.begin();}
   iterator end() { return object_map.end();}

   bool has_key(std::string const & k) const { return object_map.find(k) != object_map.end();}

   ///
   _object & operator[](std::string const & key) { 
     //std::cout << key << std::endl << std::flush;
     auto & r = object_map[key];
     if (r.name()=="") r.set_name(key); // in case the object has just been created, set its name
     return r;
   }

   ///
   _object const & operator[](std::string const & key) const {
    auto it = object_map.find(key);
    if ( it== object_map.end()) TRIQS_RUNTIME_ERROR<<"Parameters : the key : "<< key<< " does not exists";
    return it->second;
   }

   friend std::string get_triqs_hdf5_data_scheme(parameters const&) { return "";}
   
   ///write contents to an hdf5 archive
   friend void h5_write ( h5::group F, std::string const & subgroup_name, parameters const & p){
    auto gr = F.create_group(subgroup_name);
    for (auto & pvp : p.object_map) h5_write(gr, pvp.first, pvp.second);
   }

   ///read from an hdf5 archive
   friend void h5_read ( h5::group F, std::string const & subgroup_name, parameters & p);

   friend std::ostream & operator << (std::ostream & out, parameters const & p) {
    out<< "{";
    for (auto & pvp : p.object_map) out<< pvp.first << " : " << pvp.second<< ", ";
    return out<<"}";
   }

   /**
    * Register a type for conversion, serialization and h5 read/write.
    * Note : can be called multiple times (no effect for second and later call).
    * Note : this is automatically called when putting an object in parameters
    */
   template<typename T> static void register_type() { _object::register_type<T>::invoke();}

   /** 
    * Update with another parameter set.
    * If a key is present in other and not in this, add parameter to this.
    * If a key is present in both, overwrite parameter in this without any check (Python-like behaviour).
    */
   void update(parameters const & pdef);

   /// Flags controlling the update_with_default function
   //static constexpr ull_t strict_type_check = 1ull;              // Type check is strict. Always true now
   static constexpr ull_t reject_key_without_default = 1ull<<2;

   /** 
    * Update with a default parameters set pdef.
    * If a key is a default parameter in pdef and not in this, add the pdef parameter and value to this.
    * If a key is a required parameter in pdef and not in this, then raise triqs::runtime_error exception.
    * If a key is present in both, do no change it in this, but check that type are the same.
    * If a key is present in this, and not in pdef, and reject_key_without_default is passed, then raise triqs::runtime_error exception.
    */
   void update(parameter_defaults const & pdef, ull_t flag =0);

 };

}}
#endif
