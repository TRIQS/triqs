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
#pragma once
#include <triqs/mpi/base.hpp>
#include <triqs/utility/exceptions.hpp>
#include <functional>
#include <map>
#include "./impl_tools.hpp"

namespace triqs { namespace mc_tools {

 // mini concept checking
 template<typename MCSignType, typename T, typename Enable=void> struct has_accumulate : std::false_type {};
 template<typename MCSignType, typename T> struct has_accumulate <MCSignType, T, decltype(std::declval<T>().accumulate(MCSignType()))> : std::true_type {};

 template<typename T, typename Enable=void> struct has_collect_result : std::false_type {};
 template<typename T> struct has_collect_result < T, decltype(std::declval<T>().collect_results(std::declval<triqs::mpi::communicator>()))> : std::true_type {};

 //--------------------------------------------------------------------

 template<typename MCSignType>
  class measure {

   std::shared_ptr<void> impl_;
   std::function<measure()> clone_;
   size_t hash_;
   std::string type_name_;

   std::function<void (MCSignType const & ) > accumulate_;
   std::function<void (mpi::communicator const & )> collect_results_;
   std::function<void(h5::group, std::string const &)> h5_r, h5_w;

   uint64_t count_;

   template<typename MeasureType>
    void construct_delegation (MeasureType * p) {
     impl_= std::shared_ptr<MeasureType> (p);
     clone_ =  [p]() { return MeasureType(*p);} ;
     hash_ = typeid(MeasureType).hash_code();
     type_name_ =  typeid(MeasureType).name();
     accumulate_ = [p](MCSignType const & x) { p->accumulate(x);};
     count_ = 0;
     collect_results_ = [p] (mpi::communicator const & c) { p->collect_results(c);};
     h5_r = make_h5_read(p);
     h5_w = make_h5_write(p);
    }

   public :

   template<typename MeasureType>
    measure (MeasureType && p) {
     static_assert( is_move_constructible<MeasureType>::value, "This measure is not MoveConstructible");
     static_assert( has_accumulate<MCSignType,MeasureType>::value, " This measure has no accumulate method !");
     static_assert( has_collect_result<MeasureType>::value, " This measure has no collect_results method !");
     construct_delegation( new typename std::remove_reference<MeasureType>::type(std::forward<MeasureType>(p)));
    }

   // Value semantics. Everyone at the end call move = ...
   measure(measure const &rhs) {*this = rhs;}
   measure(measure &rhs) {*this = rhs;} // or it will use the template  = bug
   measure(measure && rhs) { *this = std::move(rhs);}
   measure & operator = (measure const & rhs) { *this = rhs.clone_(); return *this;}
   measure & operator = (measure && rhs) =default;

   void accumulate(MCSignType signe){ assert(impl_); count_++; accumulate_(signe); }
   void collect_results (mpi::communicator const & c ) { collect_results_(c);}

   uint64_t count() const { return count_;}

   template<typename MeasureType> bool has_type() const { return (typeid(MeasureType).hash_code() == hash_); };
   template<typename MeasureType> void check_type() const {
    if (!(has_type<MeasureType>()))
     TRIQS_RUNTIME_ERROR << "Trying to retrieve a measure of type "<< typeid(MeasureType).name() << " from a measure of type "<< type_name_;
   };

   template<typename MeasureType> MeasureType       & get()       { check_type<MeasureType>(); return *(static_cast<MeasureType *>(impl_.get())); }
   template<typename MeasureType> MeasureType const & get() const { check_type<MeasureType>(); return *(static_cast<MeasureType const *>(impl_.get())); }

   friend void h5_write (h5::group g, std::string const & name, measure const & m){ if (m.h5_w) m.h5_w(g,name);};
   friend void h5_read  (h5::group g, std::string const & name, measure & m)      { if (m.h5_r) m.h5_r(g,name);};
  };

 //--------------------------------------------------------------------

 template<typename MCSignType>
  class measure_set  {

   typedef measure<MCSignType> measure_type;
   std::map<std::string, measure<MCSignType>> m_map;
   public :

   measure_set(){}

   measure_set(measure_set const &) = default;
   measure_set(measure_set &&) = default;

   measure_set& operator = (measure_set const &) = default;
   measure_set& operator = (measure_set &&) = default;

   /**
    * Register the Measure M with a name
    */
   template<typename MeasureType>
    void insert (MeasureType && M, std::string const & name) {
     if (has(name)) TRIQS_RUNTIME_ERROR <<"measure_set : insert : measure '"<<name<<"' already inserted";
     m_map.insert(std::make_pair(name, measure_type (std::forward<MeasureType>(M))));
     // not implemented on gcc 4.6's stdlibc++ ?
     // m_map.emplace(name, measure_type (std::forward<MeasureType>(M)));
    }

   bool has(std::string const & name) const { return m_map.find(name) != m_map.end(); }

   void accumulate(MCSignType const & signe) { for (auto & nmp : m_map) nmp.second.accumulate(signe); }

   std::vector<std::string> names() const {
    std::vector<std::string> res;
    for (auto & nmp : m_map) res.push_back(nmp.first);
    return res;
   }

   // gather result for all measure, on communicator c
   void collect_results (mpi::communicator const & c ) { for (auto & nmp : m_map) nmp.second.collect_results(c); }

   // HDF5 interface
   friend void h5_write (h5::group g, std::string const & name, measure_set const & ms){
    auto gr = g.create_group(name);
    for (auto & p : ms.m_map) h5_write(gr,p.first, p.second);
   }

   friend void h5_read (h5::group g, std::string const & name, measure_set & ms){
    auto gr = g.open_group(name);
    for (auto & p : ms.m_map) h5_read(gr,p.first, p.second);
   }

   // access to the measure, given its type, with dynamical type check
   template<typename MeasureType>
    MeasureType & get_measure(std::string const & name) {
     auto it = m_map.find (name);
     if (it == m_map.end()) TRIQS_RUNTIME_ERROR << " Measure " << name << " unknown";
     return it->template get<MeasureType>();
    }

   template<typename MeasureType>
    MeasureType const & get_measure(std::string const & name) const {
     auto it = m_map.find (name);
     if (it == m_map.end()) TRIQS_RUNTIME_ERROR << " Measure " << name << " unknown";
     return it->template get<MeasureType>();
    }
  };

}}// end namespace

