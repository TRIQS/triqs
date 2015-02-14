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
#include <triqs/utility/report_stream.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/mpi/base.hpp>
#include <functional>
#include "./random_generator.hpp"
#include "./impl_tools.hpp"

namespace triqs { namespace mc_tools {

 // mini concept checking
 template<typename MCSignType, typename T, typename Enable=MCSignType> struct has_attempt : std::false_type {};
 template<typename MCSignType, typename T> struct has_attempt <MCSignType, T, decltype(MCSignType(std::declval<T>().attempt()))> : std::true_type {};

 template<typename MCSignType, typename T, typename Enable=MCSignType> struct has_accept : std::false_type {};
 template<typename MCSignType, typename T> struct has_accept <MCSignType,T, decltype(std::declval<T>().accept())> : std::true_type {};

 template<typename T, typename Enable=void> struct has_reject : std::false_type {};
 template<typename T> struct has_reject<T, decltype(std::declval<T>().reject())> : std::true_type {};

 //----------------------------------

 template<typename MCSignType>
  class move {

   std::shared_ptr<void> impl_;
   std::function<move()> clone_;
   size_t hash_;
   std::string type_name_;

   std::function<MCSignType()> attempt_, accept_;
   std::function<void()> reject_;
   std::function<void(h5::group, std::string const &)> h5_r, h5_w;

   uint64_t NProposed, Naccepted;
   double acceptance_rate_;

   template<typename MoveType>
    void construct_delegation (MoveType * p) {
     impl_= std::shared_ptr<MoveType> (p);
     hash_ = typeid(MoveType).hash_code();
     type_name_ =  typeid(MoveType).name();
     clone_   = [p](){return MoveType(*p);};
     attempt_ = [p](){return p->attempt();};
     accept_  = [p](){return p->accept();};
     reject_  = [p](){p->reject();};
     h5_r = make_h5_read(p); // make_h5_read in impl_tools
     h5_w = make_h5_write(p);
     NProposed=0;
     Naccepted=0;
     acceptance_rate_ =-1;
    }

   public :

   template<typename MoveType>
    move (MoveType && p ) {
     static_assert( is_move_constructible<MoveType>::value, "This move is not MoveConstructible");
     static_assert( has_attempt<MCSignType,MoveType>::value, "This move has no attempt method (or is has an incorrect signature) !");
     static_assert( has_accept<MCSignType,MoveType>::value, "This move has no accept method (or is has an incorrect signature) !");
     static_assert( has_reject<MoveType>::value, "This move has no reject method (or is has an incorrect signature) !");
     construct_delegation( new typename std::remove_reference<MoveType>::type(std::forward<MoveType>(p)));
    }

   // Close to value semantics. Everyone at the end call move = ...
   // no default constructor.
   move(move const &rhs) {*this = rhs;}
   move(move &rhs) {*this = rhs;} // to avoid clash with tempalte construction  !
   move(move && rhs) { *this = std::move(rhs);}
   move & operator = (move const & rhs) { *this = rhs.clone_(); return *this;}
   move & operator = (move && rhs) = default;

   MCSignType attempt(){ NProposed++; return attempt_();}
   MCSignType accept() { Naccepted++; return accept_(); }
   void reject() { reject_(); }

   double acceptance_rate() const { return acceptance_rate_;}
   uint64_t n_proposed_config () const { return NProposed;}
   uint64_t n_accepted_config () const { return Naccepted;}

   void collect_statistics(mpi::communicator const & c) {
    uint64_t nacc_tot = mpi::reduce(Naccepted, c);
    uint64_t nprop_tot = mpi::reduce(NProposed, c);
    acceptance_rate_ = nacc_tot/static_cast<double>(nprop_tot);
   }

   // true iif the stored object has type MoveType Cf hash_code doc.
   template<typename MoveType> bool has_type() const { return (typeid(MoveType).hash_code() == hash_); };

   template<typename MoveType> void check_type() const {
    if (!(has_type<MoveType>()))
     TRIQS_RUNTIME_ERROR << "Trying to retrieve a move of type "<< typeid(MoveType).name() << " from a move of type "<< type_name_;
   };

   // retrieve an object of the correct type
   template<typename MoveType> MoveType       & get()       { check_type<MoveType>(); return *(static_cast<MoveType *>(impl_.get())); }
   template<typename MoveType> MoveType const & get() const { check_type<MoveType>(); return *(static_cast<MoveType const *>(impl_.get())); }

   // redirect the h5 call to the object lambda, if it not empty (i.e. if the underlying object can be called with h5_read/write
   friend void h5_write (h5::group g, std::string const & name, move const & m){ if (m.h5_w) m.h5_w(g,name);};
   friend void h5_read  (h5::group g, std::string const & name, move & m)      { if (m.h5_r) m.h5_r(g,name);};
  };

 //--------------------------------------------------------------------

 /// A vector of (moves, proposition_probability), which is also a move itself
 template<typename MCSignType>
  class move_set  {
   std::vector<move<MCSignType> > move_vec;
   std::vector<std::string> names_;
   move<MCSignType> * current;
   size_t current_move_number;
   random_generator * RNG;
   std::vector<double> Proba_Moves, Proba_Moves_Acc_Sum;
   MCSignType try_sign_ratio;
   uint64_t debug_counter;
   public:

   ///
   move_set(random_generator & R): RNG(&R) { Proba_Moves.push_back(0); debug_counter=0;}

   ///
   move_set(move_set const &) = default;
   move_set(move_set &&) = default;
   move_set& operator = (move_set const &) = default;
   move_set& operator = (move_set &&) = default;
   
   /**
    * Add move M with its probability of being proposed.
    * NB : the proposition_probability needs to be >0 but does not need to be
    * normalized. Normalization is automatically done with all the added moves
    * before starting the run
    */
   template <typename MoveType>
    void add (MoveType && M, std::string name, double proposition_probability) {
     move_vec.emplace_back(std::forward<MoveType>(M));
     assert(proposition_probability >=0);
     Proba_Moves.push_back(proposition_probability);
     names_.push_back(name);
     normaliseProba();// ready to run after each add !
    }

    private:
    bool attempt_treat_infinite_ratio(std::complex<double>, double &) { return true; }

    bool attempt_treat_infinite_ratio(double rate_ratio, double &abs_rate_ratio) {
     bool is_inf = std::isinf(rate_ratio);
     if (is_inf) {                                          // in case the ratio is infinite
      abs_rate_ratio = 100;                                 // 1.e30; // >1 for metropolis
      try_sign_ratio = (std::signbit(rate_ratio) ? -1 : 1); // signbit -> true iif the number is negative
     }
     return !is_inf;
    }

   public:

   /**
    *  - Picks up one of the move at random (weighted by their proposition probability),
    *  - Call attempt method of that move
    *  - Returns the metropolis ratio R (see move concept).
    *    The sign ratio returned by the try method of the move is kept.
    */
   double attempt() {
    assert( Proba_Moves_Acc_Sum.size()>0);
    // Choice of move with its probability
    double proba = (*RNG)(); assert(proba>=0);
    current_move_number =0; while (proba >= Proba_Moves_Acc_Sum[current_move_number] ) { current_move_number++;}
    assert(current_move_number>0); assert(current_move_number<=move_vec.size());
    current_move_number--;
    current =  & move_vec[current_move_number];
#ifdef TRIQS_TOOLS_MC_DEBUG
    std::cerr << "*******************************************************"<< std::endl;
    std::cerr << "move number : " << debug_counter++ << std::endl;
    std::cerr << "Name of the proposed move: " << name_of_currently_selected() << std::endl;
    std::cerr <<"  Proposition probability = "<<proba<<std::endl;
#endif
    MCSignType rate_ratio = current->attempt();
    double abs_rate_ratio;
    if (attempt_treat_infinite_ratio(rate_ratio, abs_rate_ratio)) { // in case the ratio is infinite
     if (!std::isfinite(std::abs(rate_ratio)))
      TRIQS_RUNTIME_ERROR << "Monte Carlo Error : the rate ("<<rate_ratio<<") is not finite in move " << name_of_currently_selected();
     abs_rate_ratio = std::abs(rate_ratio);
#ifdef TRIQS_TOOLS_MC_DEBUG
    std::cerr << " Metropolis ratio " << rate_ratio << ". Abs(Metropolis ratio) " << abs_rate_ratio << std::endl;
#endif
    assert((abs_rate_ratio >= 0));
    try_sign_ratio = (abs_rate_ratio > 1.e-14 ? rate_ratio / abs_rate_ratio : 1); // keep the sign
   }
   return abs_rate_ratio;
  }

   /**
    *  accept the move previously selected and tried.
    *  Returns the Sign computed as, if M is the move :
    *  Sign = sign (M.attempt()) * M.accept()
    */
   MCSignType accept() {
    MCSignType accept_sign_ratio =  current->accept();
    // just make sure that accept_sign_ratio is a sign!
    assert(std::abs(std::abs(accept_sign_ratio)-1.0) < 1.e-10);
#ifdef TRIQS_TOOLS_MC_DEBUG
    std::cerr.setf(std::ios::scientific, std::ios::floatfield);
    std::cerr<<" ... Move accepted"<<std::endl;
    std::cerr<<"   try_sign_ratio = "  << try_sign_ratio   <<std::endl;
    std::cerr<<"   accept_sign_ratio = "<< accept_sign_ratio <<std::endl;
    std::cerr<<"   their product  =  "<< try_sign_ratio* accept_sign_ratio <<std::endl;
#endif
    return try_sign_ratio * accept_sign_ratio;
   }

   /**  reject the move :  Call the reject() method of the move previously selected
   */
   void reject() {
#ifdef TRIQS_TOOLS_MC_DEBUG
    std::cerr<<" ... Move rejected"<<std::endl;
#endif
    current->reject();
   }

   /// Pretty printing of the acceptance probability of the moves.
   std::string get_statistics(mpi::communicator const & c, int shift = 0) {
    std::ostringstream s;
    for (unsigned int u =0; u< move_vec.size(); ++u) {
     move_vec[u].collect_statistics(c);
     for(int i=0; i<shift; i++) s << " ";
     if (move_vec[u].template has_type<move_set>()) {
      auto & ms = move_vec[u].template get<move_set>();
      s << "Move set " << names_[u] << ": " << move_vec[u].acceptance_rate() << "\n";
      s << ms.get_statistics(c,shift+2);
     } else {
      s << "Move " << names_[u] << ": " << move_vec[u].acceptance_rate() << "\n";
     }
    }
    return s.str();
   }

   private:

   void normaliseProba() { // Computes the normalised accumulated probability
    if (move_vec.size() ==0)  TRIQS_RUNTIME_ERROR<<" no moves registered";
    double acc = 0;
    Proba_Moves_Acc_Sum.clear();
    for (unsigned int u = 0; u<Proba_Moves.size(); ++u) acc+=Proba_Moves[u];assert(acc>0);
    for (unsigned int u = 0; u<Proba_Moves.size(); ++u) Proba_Moves_Acc_Sum.push_back(Proba_Moves[u]/acc);
    for (unsigned int u = 1; u<Proba_Moves_Acc_Sum.size(); ++u) Proba_Moves_Acc_Sum[u] += Proba_Moves_Acc_Sum[u-1];
    assert(std::abs(Proba_Moves_Acc_Sum[Proba_Moves_Acc_Sum.size()-1] -1)<1.e-13);
    Proba_Moves_Acc_Sum[Proba_Moves_Acc_Sum.size()-1] += 0.001;
    // I shift the last proba acc so that even if random number in onecycle is 1 it is below that bound
    assert(Proba_Moves_Acc_Sum.size()==move_vec.size()+1);
   }

   std::string name_of_currently_selected() const { return names_[current_move_number];}

   public :

   // HDF5 interface
   friend void h5_write (h5::group g, std::string const & name, move_set const & ms){
    auto gr = g.create_group(name);
    for (size_t u=0; u<ms.move_vec.size(); ++u) h5_write(gr,ms.names_[u],ms.move_vec[u]);
   }

   friend void h5_read (h5::group g, std::string const & name, move_set & ms){
    auto gr = g.open_group(name);
    for (size_t u=0; u<ms.move_vec.size(); ++u) h5_read(gr,ms.names_[u],ms.move_vec[u]);
   }

   // access to the move, given its type, with dynamical type check
   template<typename MoveType>
    MoveType & get_move(std::string const & name) {
     int u=0; for (;u<names_.size();++u) { if (names_[u] == name) break;}
     if (u == names_.size()) TRIQS_RUNTIME_ERROR << " Move " << name << " unknown";
     return move_vec[u].template get<MoveType>();
    }

   template<typename MoveType>
    MoveType const & get_move(std::string const & name) const {
     int u=0; for (;u<names_.size();++u) { if (names_[u] == name) break;}
     if (u == names_.size()) TRIQS_RUNTIME_ERROR << " Move " << name << " unknown";
     return move_vec[u].template get<MoveType>();
    }

  };// class move_set

}}// end namespace

