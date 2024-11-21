// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include <h5/h5.hpp>
#include <triqs/utility/report_stream.hpp>
#include <triqs/utility/exceptions.hpp>
#include <mpi/mpi.hpp>
#include <functional>
#include "./random_generator.hpp"
#include "./mc_move.hpp"

namespace triqs::mc_tools {

  /// A vector of (moves, proposition_probability), which is also a move itself
  template <typename MCSignType> class move_set {
    std::vector<move<MCSignType>> move_vec;
    std::vector<std::string> names_;
    move<MCSignType> *current;
    size_t current_move_number;
    random_generator *RNG;
    std::vector<double> Proba_Moves, Proba_Moves_Acc_Sum;
    MCSignType try_sign_ratio;
    uint64_t debug_counter;

#ifdef TRIQS_MCTOOLS_DEBUG
    static constexpr bool debug = true;
#else
    static constexpr bool debug = false;
#endif

    public:
    /// Need a random_generator for attempt, see below...
    move_set(random_generator &R) : RNG(&R) {
      Proba_Moves.push_back(0);
      debug_counter = 0;
    }

    move_set(move_set const &rhs)            = delete;
    move_set(move_set &&rhs)                 = default;
    move_set &operator=(move_set const &rhs) = delete;
    move_set &operator=(move_set &&rhs)      = default;

    /**
   * Add move M with its probability of being proposed.
   * NB : the proposition_probability needs to be >0 but does not need to be
   * normalized. Normalization is automatically done with all the added moves
   * before starting the run
   */
    template <typename MoveType> void add(MoveType &&M, std::string name, double proposition_probability) {
      move_vec.emplace_back(true, std::forward<MoveType>(M));
      assert(proposition_probability >= 0);
      Proba_Moves.push_back(proposition_probability);
      names_.push_back(name);
      normaliseProba(); // ready to run after each add !
    }

    private:
    bool attempt_treat_infinite_ratio(std::complex<double>, double &) { return true; }

    bool attempt_treat_infinite_ratio(double rate_ratio, double &abs_rate_ratio) {
      bool is_inf = std::isinf(rate_ratio);
      if (is_inf) {                                           // in case the ratio is infinite
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
      assert(Proba_Moves_Acc_Sum.size() > 0);
      if (move_vec.size() == 0) TRIQS_RUNTIME_ERROR << "ERROR in attempting Monte-Carlo Move: No move was registered!";
      // Choice of move with its probability
      double proba = (*RNG)();
      assert(proba >= 0);
      current_move_number = 0;
      while (proba >= Proba_Moves_Acc_Sum[current_move_number]) { current_move_number++; }
      assert(current_move_number > 0);
      assert(current_move_number <= move_vec.size());
      current_move_number--;
      current = &move_vec[current_move_number];
      if (debug) {
        std::cerr << "*******************************************************" << std::endl;
        std::cerr << "move number : " << debug_counter++ << std::endl;
        std::cerr << "Name of the proposed move: " << name_of_currently_selected() << std::endl;
        std::cerr << "  Proposition probability = " << proba << std::endl;
      }
      MCSignType rate_ratio = current->attempt();
      double abs_rate_ratio;
      if (attempt_treat_infinite_ratio(rate_ratio, abs_rate_ratio)) { // in case the ratio is infinite
        if (!std::isfinite(std::abs(rate_ratio)))
          TRIQS_RUNTIME_ERROR << "Monte Carlo Error : the rate (" << rate_ratio << ") is not finite in move " << name_of_currently_selected();
        abs_rate_ratio = std::abs(rate_ratio);
        if (debug) std::cerr << " Metropolis ratio " << rate_ratio << ". Abs(Metropolis ratio) " << abs_rate_ratio << std::endl;
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
      MCSignType accept_sign_ratio = current->accept();
      // just make sure that accept_sign_ratio is a sign!
      if (debug) {
        if (std::abs(std::abs(accept_sign_ratio) - 1.0) > 1.e-10) TRIQS_RUNTIME_ERROR << "|sign| !=1 !!!";
        std::cerr.setf(std::ios::scientific, std::ios::floatfield);
        std::cerr << " ... Move accepted" << std::endl;
        std::cerr << "   try_sign_ratio = " << try_sign_ratio << std::endl;
        std::cerr << "   accept_sign_ratio = " << accept_sign_ratio << std::endl;
        std::cerr << "   their product  =  " << try_sign_ratio * accept_sign_ratio << std::endl;
      }
      return try_sign_ratio * accept_sign_ratio;
    }

    /**  reject the move :  Call the reject() method of the move previously selected
  */
    void reject() {
      if (debug) std::cerr << " ... Move rejected" << std::endl;
      current->reject();
    }

    ///
    void clear_statistics() {
      for (auto &m : move_vec) {
        m.clear_statistics();
        auto ms = m.as_move_set();
        if (ms) ms->clear_statistics();
      }
    }

    ///
    void collect_statistics(mpi::communicator const &c) {
      for (auto &m : move_vec) m.collect_statistics(c);
    }

    /// Acceptance rate of all moves as a map name:string -> acceptance_rate:double
    std::map<std::string, double> get_acceptance_rates() const {
      std::map<std::string, double> r;
      for (unsigned int u = 0; u < move_vec.size(); ++u) {
        r.insert({names_[u], move_vec[u].acceptance_rate()});
        auto ms = move_vec[u].as_move_set();
        if (ms) { // if it is a move set, flatten the result
          auto ar = ms->get_acceptance_rates();
          r.insert(ar.begin(), ar.end());
        }
      }
      return r;
    }

    /// Pretty printing of the acceptance probability of the moves.
    std::string get_statistics(std::string decal = "") const {
      std::ostringstream s;
      for (unsigned int u = 0; u < move_vec.size(); ++u) {
        auto ms = move_vec[u].as_move_set();
        s << decal << "Move " << (ms ? "set " : " ") << names_[u] << ": " << move_vec[u].acceptance_rate() << "\n";
        if (ms) s << ms->get_statistics(decal + "  ");
      }
      return s.str();
    }

    private:
    void normaliseProba() { // Computes the normalised accumulated probability
      if (move_vec.size() == 0) TRIQS_RUNTIME_ERROR << " no moves registered";
      double acc = 0;
      Proba_Moves_Acc_Sum.clear();
      for (unsigned int u = 0; u < Proba_Moves.size(); ++u) acc += Proba_Moves[u];
      assert(acc > 0);
      for (unsigned int u = 0; u < Proba_Moves.size(); ++u) Proba_Moves_Acc_Sum.push_back(Proba_Moves[u] / acc);
      for (unsigned int u = 1; u < Proba_Moves_Acc_Sum.size(); ++u) Proba_Moves_Acc_Sum[u] += Proba_Moves_Acc_Sum[u - 1];
      assert(std::abs(Proba_Moves_Acc_Sum[Proba_Moves_Acc_Sum.size() - 1] - 1) < 1.e-13);
      Proba_Moves_Acc_Sum[Proba_Moves_Acc_Sum.size() - 1] += 0.001;
      // I shift the last proba acc so that even if random number in onecycle is 1 it is below that bound
      assert(Proba_Moves_Acc_Sum.size() == move_vec.size() + 1);
    }

    std::string name_of_currently_selected() const { return names_[current_move_number]; }

    public:
    // HDF5 interface
    friend void h5_write(h5::group g, std::string const &name, move_set const &ms) {
      auto gr = g.create_group(name);
      for (size_t u = 0; u < ms.move_vec.size(); ++u) h5_write(gr, ms.names_[u], ms.move_vec[u]);
    }

    friend void h5_read(h5::group g, std::string const &name, move_set &ms) {
      auto gr = g.open_group(name);
      for (size_t u = 0; u < ms.move_vec.size(); ++u) h5_read(gr, ms.names_[u], ms.move_vec[u]);
    }

  }; // class move_set

} // namespace triqs::mc_tools
