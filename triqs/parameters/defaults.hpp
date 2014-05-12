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
#ifndef TRIQS_UTILITY_PARAMS_DEFAULT_H
#define TRIQS_UTILITY_PARAMS_DEFAULT_H
#include "./opaque_object_h5.hpp"
namespace triqs { namespace utility {
 /**
  * Class to handle required and optional program parameters and default values for optional parameters.
  * Required parameters need to be provided by the user. Parameters that are optional need not be provided
  * by the user because sensible default values can be given. The former are inserted through the required
  * method, the latter by the optional method. An object of type parameters can be updated from an object
  * of type parameter_defaults (see parameters.hpp).
  */
 class parameter_defaults {
  public :
   parameter_defaults() {};
   parameter_defaults (parameter_defaults const & other) = default;
   parameter_defaults (parameter_defaults && other) { swap(*this,other);}
   parameter_defaults & operator =  (parameter_defaults const & other)  = default;
   parameter_defaults & operator =  (parameter_defaults && other) noexcept { swap(*this,other); return *this;}
   friend void swap(parameter_defaults & a, parameter_defaults &b) 
   { swap(a.object_map,b.object_map); swap(a.documentation, b.documentation);swap(a.is_optional, b.is_optional);  }

  private:
   typedef std::map<std::string, _object> map_t;
   map_t object_map;
   std::map<std::string, std::string> documentation;
   std::map<std::string, bool > is_optional;

   friend class boost::serialization::access;
   template<class Archive>
    void serialize(Archive & ar, const unsigned int version) { ar & TRIQS_MAKE_NVP("object_map",object_map); }

   template<typename T> parameter_defaults & insert(std::string const & key, T && def_val, std::string const & doc, bool opt) {
    object_map[key] = std::forward<T>(def_val);
    documentation[key] = doc;
    is_optional[key] = opt;
    return *this;
   }

   template<typename T> const T getter(std::map<std::string,T> const & m, std::string const & key) const {
    auto it = m.find(key); assert(it !=m.end()); return it->second;
   }

  public:

   typedef map_t::const_iterator const_iterator;
   typedef map_t::iterator iterator;
   const_iterator begin() const { return object_map.begin();}
   const_iterator end()  const { return object_map.end();}
   iterator begin() { return object_map.begin();}
   iterator end() { return object_map.end();}

   bool has_key(std::string const & key) const { return object_map.find(key) != object_map.end();}
   bool is_required(std::string const & key) const { return (has_key(key) && (! getter(this->is_optional,key)));}
   std::string doc(std::string const & key) const { return (has_key(key) ? getter(this->documentation,key) : "");}

   /// inserter for optional parameters;
   /// calls can be chained for multiple parameters
   template <typename T> parameter_defaults &optional(std::string const &key, T &&def_val, std::string const &doc) {
    insert(key, std::forward<T>(def_val), doc, true);
    return *this;
   }

   /// inserter for required parameters;
   /// calls can be chained for multiple parameters
   template <typename T> parameter_defaults &required(std::string const &key, T &&def_val, std::string const &doc) {
    insert(key, std::forward<T>(def_val), doc, false);
    return *this;
   }

   ///parameters class-like element access
   _object const & operator[](std::string const & key) const {
    auto it = object_map.find(key);
    if ( it== object_map.end()) TRIQS_RUNTIME_ERROR<<"Key : "<< key<< " not found";
    return it->second;
   }

   ///generate help in form of a table of strings containing a list of required and optional parameters
   std::vector<std::vector<std::string>> generate_help() const;

   ///print a formatted table of required and optional parameters
   friend std::ostream & operator << (std::ostream & out, parameter_defaults const & p);

 };

}}
#endif
