/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_TOOLS_MC_PRECOMPUTATION_H
#define TRIQS_TOOLS_MC_PRECOMPUTATION_H

#include <functional>
#include <boost/mpi.hpp>
#include <map>
#include <triqs/utility/exceptions.hpp>
#include "./impl_tools.hpp"

namespace triqs { namespace mc_tools {

 // mini concept checking
 template<typename T, typename Enable=void> struct is_callable: std::false_type {};
 template<typename T> struct is_callable <T, decltype(std::declval<T>()())> : std::true_type {};


 //--------------------------------------------------------------------

  class measure_aux {

   std::shared_ptr<void> impl_;
   std::function<measure_aux()> clone_;
   size_t hash_;
   std::string type_name_;

   std::function<void ( ) > call_;

   public :

   template<typename MeasureAuxType>
    measure_aux (MeasureAuxType && p_in, bool) {
     static_assert( is_callable<MeasureAuxType>::value, "This measure_aux is not callable");
     MeasureAuxType *p = new typename std::remove_reference<MeasureAuxType>::type(std::forward<MeasureAuxType>(p_in));
     impl_= std::shared_ptr<void> (p);
     clone_ =  [p]() { return MeasureAuxType(*p);} ;
     hash_ = typeid(MeasureAuxType).hash_code();
     type_name_ =  typeid(MeasureAuxType).name();
     call_ = [p]() { (*p)();};
    }

   // Value semantics. Everyone at the end call move = ...
   measure_aux(measure_aux const &rhs)  = default; //{*this = rhs;}
   //measure_aux(measure_aux &rhs) {*this = rhs;} // or it will use the template  = bug
   measure_aux(measure_aux && rhs)  = default ; //{ *this = std::move(rhs);}
   measure_aux & operator = (measure_aux const & rhs) { *this = rhs.clone_(); return *this;}
   measure_aux & operator = (measure_aux && rhs) =default;

   void operator()(){ call_();}

   template<typename MeasureAuxType> bool has_type() const { return (typeid(MeasureAuxType).hash_code() == hash_); };
   template<typename MeasureAuxType> void check_type() const {
    if (!(has_type<MeasureAuxType>()))
     TRIQS_RUNTIME_ERROR << "Trying to retrieve a measure_aux of type "<< typeid(MeasureAuxType).name() << " from a measure_aux of type "<< type_name_;
   };

   template<typename MeasureAuxType> MeasureAuxType       * get()       { check_type<MeasureAuxType>(); return (static_cast<MeasureAuxType *>(impl_.get())); }
   template<typename MeasureAuxType> MeasureAuxType const * get() const { check_type<MeasureAuxType>(); return (static_cast<MeasureAuxType const *>(impl_.get())); }

  };

 //--------------------------------------------------------------------

  class measure_aux_set  {
   typedef measure_aux measure_aux_type;
   std::map<std::string, measure_aux> m_map;
   public :

   measure_aux_set(){}

   measure_aux_set(measure_aux_set const &) = default;
   measure_aux_set(measure_aux_set &&) = default;

   measure_aux_set& operator = (measure_aux_set const &) = default;
   measure_aux_set& operator = (measure_aux_set &&) = default;

   /**
    * Register the auxiliary M with a name
    */
   /*template<typename MeasureAuxType>
    MeasureAuxType * insert (MeasureAuxType && M, std::string const & name) {
     if (has(name)) TRIQS_RUNTIME_ERROR << "Auxiliary measure "<<name<<" already exists";
     auto r = m_map.insert(std::make_pair(name, measure_aux_type (std::forward<MeasureAuxType>(M))));
     return r.first;
    }

    template<typename MeasureAuxType>
    MeasureAuxType * retrieve (std::string const & name) {
     if (!has(name)) TRIQS_RUNTIME_ERROR << "Auxiliary measure "<<name<<" does not exist";
     return (&get_measure_aux<MeasureAuxType>(name));
    }
*/
   bool has(std::string const & name) const { return m_map.find(name) != m_map.end(); }

   void compute_all () { for (auto & nmp : m_map) nmp.second(); }

   std::vector<std::string> names() const {
    std::vector<std::string> res;
    for (auto const & nmp : m_map) res.push_back(nmp.first);
    return res;
   }

   // access to the measure_aux, given its type, with dynamical type check
   template<typename MeasureAuxType>
    MeasureAuxType * get_measure_aux(std::string const & name) {
     auto it = m_map.find (name);
     if (it == m_map.end()) { auto r = m_map.insert(std::make_pair(name, measure_aux_type(MeasureAuxType()))); return get_measure_aux<MeasureAuxType>(name);} 
     return it->second.template get<MeasureAuxType>();
    }

   /*template<typename MeasureAuxType>
    MeasureAuxType const & get_measure_aux(std::string const & name) const {
     auto it = m_map.find (name);
     if (it == m_map.end()) TRIQS_RUNTIME_ERROR << " Measure " << name << " unknown";
     return it->template get<MeasureAuxType>();
    }
    */
  };

}}// end namespace
#endif

