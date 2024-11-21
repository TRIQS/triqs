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

namespace triqs::mc_tools {

  template <typename MCSignType> class move_set;

  // Type erasure for any object modeling the Move concept
  template <typename MCSignType> class move {

    // using a simple erasure pattern.
    // i) Take the method as lambda, capturing the object
    // ii) keep it in a shared_ptr (impl_).
    // iii) restore almost Regular Type semantics, except default construction.
    //      in particular copy DOES copy ...

    std::shared_ptr<void> impl_;
    std::function<MCSignType()> attempt_, accept_;
    std::function<void()> reject_;
    std::function<void(mpi::communicator const &)> collect_statistics_;
    std::function<void(h5::group, std::string const &)> h5_r, h5_w;

    uint64_t NProposed, Naccepted;
    double acceptance_rate_;
    bool is_move_set_; // need to remember if the move was a move_set for printing details later.

#ifdef TRIQS_MCTOOLS_DEBUG
    static constexpr bool debug = true;
#else
    static constexpr bool debug = false;
#endif

    public:
    /// Construct from any m modeling MoveType. bool is here to disambiguate with basic copy/move construction.
    template <typename MoveType> move(bool, MoveType &&m) {
      static_assert(std::is_move_constructible<MoveType>::value, "This move is not MoveConstructible");
      static_assert(requires { m.attempt(); }, "This move has no attempt method (or is has an incorrect signature) !");
      static_assert(requires { m.accept(); }, "This move has no accept method (or is has an incorrect signature) !");
      static_assert(requires { m.reject(); }, "This move has no reject method (or is has an incorrect signature) !");
      using m_t           = std::decay_t<MoveType>;
      m_t *p              = new m_t(std::forward<MoveType>(m)); // moving or copying
      impl_               = std::shared_ptr<m_t>(p);
      attempt_            = [p]() { return p->attempt(); };
      accept_             = [p]() { return p->accept(); };
      reject_             = [p]() { p->reject(); };
      collect_statistics_ = [p](mpi::communicator c) {
        if constexpr (requires { p->collect_statistics(c); })
          p->collect_statistics(c);
        else
          (void)p; // suppress clang -Wunused-lambda-capture warning
      };
      h5_r = [p](h5::group g, std::string const &name) {
        if constexpr (requires { h5_read(g, name, *p); })
          h5_read(g, name, *p);
        else
          (void)p; // suppress clang -Wunused-lambda-capture warning
      };
      h5_w = [p](h5::group g, std::string const &name) {
        if constexpr (requires { h5_write(g, name, *p); })
          h5_write(g, name, *p);
        else
          (void)p; // suppress clang -Wunused-lambda-capture warning
      };
      NProposed        = 0;
      Naccepted        = 0;
      acceptance_rate_ = -1;
      is_move_set_     = std::is_same<MoveType, move_set<MCSignType>>::value;
    }

    // no default constructor.
    move(move const &rhs)            = delete;
    move(move &&rhs)                 = default;
    move &operator=(move const &rhs) = delete;
    move &operator=(move &&rhs)      = default;

    MCSignType attempt() {
      NProposed++;
      return attempt_();
    }
    MCSignType accept() {
      Naccepted++;
      return accept_();
    }
    void reject() { reject_(); }

    double acceptance_rate() const { return acceptance_rate_; }
    uint64_t n_proposed_config() const { return NProposed; }
    uint64_t n_accepted_config() const { return Naccepted; }

    void clear_statistics() {
      NProposed        = 0;
      Naccepted        = 0;
      acceptance_rate_ = -1;
    }

    void collect_statistics(mpi::communicator const &c) {
      uint64_t nacc_tot  = mpi::all_reduce(Naccepted, c);
      uint64_t nprop_tot = mpi::all_reduce(NProposed, c);
      acceptance_rate_   = nacc_tot / static_cast<double>(nprop_tot);
      if (collect_statistics_) collect_statistics_(c);
    }

    move_set<MCSignType> *as_move_set() const { return is_move_set_ ? static_cast<move_set<MCSignType> *>(impl_.get()) : nullptr; }

    // redirect the h5 call to the object lambda, if it not empty (i.e. if the underlying object can be called with h5_read/write
    friend void h5_write(h5::group g, std::string const &name, move const &m) {
      if (m.h5_w) m.h5_w(g, name);
    };
    friend void h5_read(h5::group g, std::string const &name, move &m) {
      if (m.h5_r) m.h5_r(g, name);
    };
  };

} // namespace triqs::mc_tools
