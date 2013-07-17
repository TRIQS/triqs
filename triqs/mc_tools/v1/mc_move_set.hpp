
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

#ifndef TRIQS_TOOLS_MC_MOVE_SET1_H
#define TRIQS_TOOLS_MC_MOVE_SET1_H

#include <boost/mpi.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "random_generator.hpp"
#include <boost/concept_check.hpp>
#include <triqs/utility/report_stream.hpp>

namespace triqs { namespace mc_tools { 
 namespace mpi=boost::mpi;
 template<typename MCSignType> class move_set;
 namespace details { 

  template<typename MCSignType>
   struct move_base { 
    virtual ~move_base(){}
    virtual MCSignType Try()=0;
    virtual MCSignType Accept()=0;
    virtual void Reject()=0;
    virtual double acceptance_probability(mpi::communicator const & c) const=0;// on master,0 on nodes
    virtual void print (triqs::utility::report_stream &, mpi::communicator const &, std::string name, std::string decal) const =0;
   };

  //--------------------------------------------------------------------

  template <class X, typename Y> struct IsMove {
   BOOST_CONCEPT_USAGE(IsMove)
   {
    Y r = i.Try();      
    r = i.Accept();
    i.Reject();
   }
   private:
   X i;
  };

  //--------------------------------------------------------------------

  template<typename MoveType>
   void print_move (MoveType const &,triqs::utility::report_stream &,mpi::communicator const &,std::string name,std::string decal){}

  template<typename MCSignType>
   void print_move (move_set<MCSignType> const & M, triqs::utility::report_stream & report, mpi::communicator const & c, std::string name, std::string decal);

  //--------------------------------------------------------------------

  template<typename MCSignType, typename MoveType>
   class move_impl : boost::noncopyable, public move_base<MCSignType> { 
    BOOST_CONCEPT_ASSERT((IsMove<MoveType, MCSignType>));
    private:
    boost::shared_ptr<MoveType> ptr; // ptr to the move
    uint64_t NProposed,NAccepted; // Statistics
    void operator=(move_impl const &); // forbidden
    public:
    move_impl(MoveType * move_ptr):move_base<MCSignType>(),ptr(move_ptr),NProposed(0),NAccepted(0) {}
    move_impl(boost::shared_ptr<MoveType> sptr):move_base<MCSignType>(),ptr(sptr),NProposed(0),NAccepted(0) {}
    move_impl(move_impl const & M):
     move_base<MCSignType>(),ptr(new MoveType (*M.ptr.get())),
     NProposed(M.NProposed),NAccepted(M.NAccepted){}
    virtual ~move_impl(){}
    virtual MCSignType Try(){ NProposed++; return ptr->Try();}
    virtual MCSignType Accept() { NAccepted++; return  ptr->Accept(); }
    virtual void Reject() { ptr->Reject();}
    const MoveType * get() const { return ptr.get();}
    /// Acceptance Probability on the master and 0 on the nodes
    virtual double acceptance_probability(mpi::communicator const & c) const { 
     uint64_t nacc_tot=0, nprop_tot=1;
     mpi::reduce(c, NAccepted, nacc_tot, std::plus<uint64_t>(), 0);
     mpi::reduce(c, NProposed, nprop_tot, std::plus<uint64_t>(), 0);
     return nacc_tot/static_cast<double>(nprop_tot);
    }
    virtual void print (triqs::utility::report_stream & report, mpi::communicator const & c, std::string name, std::string decal) const { 
     report<< decal <<"Acceptance probability of move : "<<name<<"  :  "<<acceptance_probability(c)<<std::endl;
     print_move(*get(),report,c,name,decal);
    }
   };
 }

 //--------------------------------------------------------------------

 /** 
  *   A vector of (moves, PropositionProbability), which is also a move itself
  */
 template<typename MCSignType>
  class move_set  {
   boost::ptr_vector<details::move_base<MCSignType> > moves_; 
   std::vector<std::string> names_;
   details::move_base<MCSignType> * current;
   size_t current_move_number;
   random_generator & RNG;
   std::vector<double> Proba_Moves, Proba_Moves_Acc_Sum;  
   public:   

   ///
   move_set(random_generator & R): RNG(R) { Proba_Moves.push_back(0); }

   /** 
    * Add move M with its probability of being proposed.
    * NB : the PropositionProbability needs to be >0 but does not need to be 
    * normalized. Normalization is automatically done with all the added moves
    * before starting the run
    *
    * WARNING : the pointer is deleted automatically by the MC class at destruction. 
    */
   template <typename MoveType>
    void add (MoveType * && M, std::string name, double PropositionProbability) {
     moves_.push_back(new details::move_impl<MCSignType,MoveType>(M));
     assert(PropositionProbability >=0);
     Proba_Moves.push_back(PropositionProbability);
     names_.push_back(name);
     normaliseProba();// ready to run after each add !
    }

   template <typename MoveType>
    void add (boost::shared_ptr<MoveType> sptr, std::string name, double PropositionProbability) {
     moves_.push_back(new details::move_impl<MCSignType,MoveType>(sptr));
     assert(PropositionProbability >=0);
     Proba_Moves.push_back(PropositionProbability);
     names_.push_back(name);
     normaliseProba();// ready to run after each add !
    }

   //-----------------
   /**
    *  - Picks up one of the move at random (weighted by their proposition probability), 
    *  - Call Try method of that move
    *  - Returns the metropolis ratio R (see move concept). 
    *    The sign ratio returned by the try method of the move is kept.
    */
   double Try() {
    assert( Proba_Moves_Acc_Sum.size()>0);
    // Choice of move with its probability
    double proba = RNG();assert(proba>=0);
    //std::cerr<<" Size of proba_moves_acc"<< Proba_Moves_Acc_Sum.size()<<std::endl;
    // for (unsigned int u = 0; u<Proba_Moves_Acc_Sum.size(); ++u) std::cerr<<"PROBA-- "<<u << "   "<<Proba_Moves_Acc_Sum[u]<<std::endl;
    current_move_number =0; while (proba >= Proba_Moves_Acc_Sum[current_move_number] ) { current_move_number++;}
    //std::cerr << "curren move #"<<current_move_number<< "  "<<proba<<std::endl;
    assert(current_move_number>0); assert(current_move_number<=this->size());
    current_move_number--;
    current =  &moves_[current_move_number];
#ifdef DEBUG 
    std::cerr << "*******************************************************"<< std::endl;
    std::cerr << "Name of the proposed move: " << name_of_currently_selected() << std::endl;
    std::cerr <<"  Proposition probability = "<<proba<<std::endl;
#endif
    MCSignType rate_ratio = current->Try();
    if (!std::isfinite(std::abs(rate_ratio))) 
     TRIQS_RUNTIME_ERROR<<"Monte Carlo Error : the rate is not finite in move "<<name_of_currently_selected();
    double abs_rate_ratio = std::abs(rate_ratio);
#ifdef DEBUG
    std::cerr << " Metropolis ratio " << rate_ratio<<". Abs(Metropolis ratio) " <<abs_rate_ratio << std::endl;
#endif
    assert ((abs_rate_ratio>=0)); 
    try_sign_ratio = ( abs_rate_ratio> 1.e-14 ? rate_ratio/abs_rate_ratio : 1); // keep the sign
    return abs_rate_ratio;
   }

   //-----------------
   /**
    *  Accept the move previously selected and tried.
    *  Returns the Sign computed as, if M is the move : 
    *  Sign = sign (M.Try()) * M.Accept()
    */
   MCSignType Accept() { 
    MCSignType accept_sign_ratio =  current->Accept();
    // just make sure that accept_sign_ratio is a sign!
    assert(std::abs(std::abs(accept_sign_ratio)-1.0) < 1.e-10);
#ifdef DEBUG
    std::cerr.setf(std::ios::scientific, std::ios::floatfield);
    std::cerr<<" ... Move accepted"<<std::endl;
    std::cerr<<"   try_sign_ratio = "  << try_sign_ratio   <<std::endl;
    std::cerr<<"   accept_sign_ratio = "<< accept_sign_ratio <<std::endl;
    std::cerr<<"   their product  =  "<< try_sign_ratio* accept_sign_ratio <<std::endl;
#endif
    return try_sign_ratio * accept_sign_ratio;
   }

   //-----------------

   /**  Reject the move
    *   Call the Reject() method of the move previously selected
    */
   void Reject() { 
#ifdef DEBUG
    std::cerr<<" ... Move rejected"<<std::endl;
#endif
    current->Reject();
   } 

   //-----------------
   /// Number of moves registered in the move_set
   size_t size() const  { return moves_.size();}

   //-----------------
   /// Pretty printing of the acceptance probability of the moves. 
   void print (triqs::utility::report_stream & report, mpi::communicator const & c, std::string name="", std::string decal="") const { 
    report <<decal<<"Move set : "<<name <<std::endl;
    for (unsigned int u =0; u< size(); ++u)
     moves_[u].print(report,c,names_[u],decal+std::string("  ")); 
   }

   protected:

   MCSignType try_sign_ratio;

   void normaliseProba() { 
    if (moves_.size() ==0)  TRIQS_RUNTIME_ERROR<<" no moves registered";
    // Computes the normalised accumulated probability
    double acc = 0; 
    Proba_Moves_Acc_Sum.clear();
    for (unsigned int u = 0; u<Proba_Moves.size(); ++u) acc+=Proba_Moves[u];assert(acc>0);
    for (unsigned int u = 0; u<Proba_Moves.size(); ++u) Proba_Moves_Acc_Sum.push_back(Proba_Moves[u]/acc);
    // to accumulation 
    for (unsigned int u = 1; u<Proba_Moves_Acc_Sum.size(); ++u) Proba_Moves_Acc_Sum[u] += Proba_Moves_Acc_Sum[u-1];
    assert(std::abs(Proba_Moves_Acc_Sum[Proba_Moves_Acc_Sum.size()-1] -1)<1.e-13);
    //std::cerr<<" NOam Size of proba_moves_acc"<< Proba_Moves_Acc_Sum.size()<<std::endl;
    //for (unsigned int u = 0; u<Proba_Moves_Acc_Sum.size(); ++u) std::cerr<<"PROBA "<<u <<Proba_Moves_Acc_Sum[u]<<std::endl;
    Proba_Moves_Acc_Sum[Proba_Moves_Acc_Sum.size()-1] += 0.001; 
    // I shift the last proba acc so that even if random number in onecycle is 1 it is below that bound
    assert(Proba_Moves_Acc_Sum.size()==this->size()+1);
   }

   /// for debug only
   std::string name_of_currently_selected() const { return names_[current_move_number];} 

  };// class move_set

 namespace details { // specialization for pretty print of move_set.
  template<typename MCSignType>
   inline void print_move (move_set<MCSignType> const & M, triqs::utility::report_stream & report, mpi::communicator const & c, std::string name, std::string decal) {
    M.print(report,c,name,decal); 
   }
 }



}
}// end namespace
#endif

