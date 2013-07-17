
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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

#ifndef MC_MEASURE1_H
#define MC_MEASURE1_H

#include <boost/mpi.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/concept_check.hpp>
#include <map>
#include <triqs/utility/exceptions.hpp>

namespace triqs { 
namespace mc_tools { 
 namespace mpi=boost::mpi;
 using std::size_t;

 //--------------------------------------------------------------------

 namespace details {

  template<typename MCSignType>  
   class measure_base {
    public:
     virtual ~measure_base(){}
     virtual void accumulate(MCSignType signe)=0;
     virtual void collect_results(mpi::communicator const & c)=0;
     virtual uint64_t count() const=0;
   };

  //--------------------------------------------------------------------

  template <class X, typename MCSignType> struct IsMeasure {
   BOOST_CONCEPT_USAGE(IsMeasure)
   {
    i.accumulate(MCSignType(1.0));
    i.collect_results (*c);
   }
   private:
   X i;  boost::mpi::communicator const * c;
  };

  //--------------------------------------------------------------------

  template<typename MCSignType, typename MeasureType>
   class measure_impl : public measure_base<MCSignType> { 
    BOOST_CONCEPT_ASSERT((IsMeasure<MeasureType,MCSignType>));
    void operator=(measure_impl const &); // forbidden
    protected:
    boost::shared_ptr<MeasureType> ptr;
    uint64_t count_;
    public:
    measure_impl(MeasureType * _ptr):measure_base<MCSignType>(), ptr(_ptr),count_(0)  {}
    measure_impl(boost::shared_ptr<MeasureType> sptr) : measure_base<MCSignType>(), ptr(sptr),count_(0)  {}
    measure_impl( measure_impl const & X):measure_base<MCSignType>(X), ptr( new MeasureType (*X.ptr.get())),count_(X.count_) {}
    virtual ~measure_impl(){}
    virtual void accumulate(MCSignType signe){ count_++; ptr->accumulate(signe); }
    virtual void collect_results(mpi::communicator const & c) { ptr->collect_results (c); }
    virtual uint64_t count() const { return count_;}
   };

 }//details namespace

 //--------------------------------------------------------------------

 template<typename MCSignType>  
  class mcmeasure  { 
   boost::shared_ptr< details::measure_base<MCSignType> > impl_;
   public :

   template<typename MeasureType>
   mcmeasure ( MeasureType * p) : impl_( new  details::measure_impl<MCSignType,MeasureType> (p)) {}

   template<typename MeasureType>
   mcmeasure ( boost::shared_ptr<MeasureType> sptr) : impl_( new  details::measure_impl<MCSignType,MeasureType> (sptr)) {}

   mcmeasure() {} 
   void accumulate(MCSignType const & signe) { assert(impl_); impl_->accumulate(signe);}
   void collect_results(boost::mpi::communicator const & communicator) { assert(impl_); impl_-> collect_results(communicator);}
   uint64_t count() const { assert(impl_); return impl_->count();}
   // Recover the measure in its original type 
   /* template <typename T> T const & get() const { assert(impl_);
      details::measure_impl<MCSignType,T> * res = dynamic_cast< details::measure_impl <MCSignType,T> *> (impl_.get());
      if (!res) TRIQS_RUNTIME_ERROR<<"Internal error in measure_set : Measure  is not of the required type";
      return res.get();
      } */
  };

 //--------------------------------------------------------------------

 template<typename MCSignType>
  class measure_set : public std::map<std::string, mcmeasure<MCSignType> > {
   typedef std::map<std::string, mcmeasure<MCSignType> > BaseType;
   typedef mcmeasure<MCSignType> measure_type;
  public : 
   typedef typename BaseType::iterator iterator;
   typedef typename BaseType::const_iterator const_iterator;
 
   /**
    * Register the Measure M with a name
    * WARNING : the pointer is deleted automatically by the class at destruction. 
    */
   template<typename T>
    void insert (T * && M, std::string const & name) {
    if (has(name)) TRIQS_RUNTIME_ERROR <<"measure_set : insert : measure '"<<name<<"' already inserted";
    BaseType::insert(std::make_pair(name, measure_type (M)));
    } 

   template<typename T>
   void insert(boost::shared_ptr<T> ptr, std::string const & name) {
    if (has(name)) TRIQS_RUNTIME_ERROR <<"measure_set : insert : measure '"<<name<<"' already inserted";
    BaseType::insert(std::make_pair(name, measure_type(ptr)));
   }

   bool has(std::string const & name) const { return BaseType::find(name) != BaseType::end(); }

   measure_type & operator[](std::string const & name) {
    if (!has(name))
     throw std::out_of_range("No result found with the name: " + name);
    return BaseType::find(name)->second;
   }

   measure_type const & operator[](std::string const & name) const {
    if (!has(name))
     throw std::out_of_range("No result found with the name: " + name);
    return BaseType::find(name)->second;
   }

   ///
   void accumulate( MCSignType & signe) { for (iterator it= this->begin(); it != this->end(); ++it) it->second.accumulate(signe); }

   ///
   //size_t size() const {return name_to_mes.size(); } 

   ///
   std::vector<std::string> names() const { 
    std::vector<std::string> res; 
    for (const_iterator it= this->begin(); it !=this->end(); ++it) res.push_back(it->first); 
    return res;
   }

   // gather result for all measure, on communicator c
   void collect_results (boost::mpi::communicator const & c ) {
    for (typename BaseType::iterator it = this->begin(); it != this->end(); ++it) it->second.collect_results(c);
   }

  };

}}// end namespace
#endif

