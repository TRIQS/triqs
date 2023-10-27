// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2017 Hugo U.R. Strand
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
// Authors: Michel Ferrero, Henri Menke, Olivier Parcollet, Hugo U.R. Strand, Nils Wentzell

#pragma once
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/timer.hpp>
#include <functional>
#include <map>
#include <cassert>
#include <iomanip>

namespace triqs {
  namespace mc_tools {

    // similar technique as move, cf move_set.
    template <typename MCSignType> class measure {

      std::shared_ptr<void> impl_;
      std::function<void(MCSignType const &)> accumulate_;
      std::function<void(mpi::communicator const &)> collect_results_;
      std::function<std::string()> report_;
      std::function<void(h5::group, std::string const &)> h5_r, h5_w;

      uint64_t count_;
      bool enable_timer;
      utility::timer Timer;

      public:
      bool report_measure;

      template <typename MeasureType>
      measure(bool, MeasureType &&m, bool enable_timer, bool report_measure) : enable_timer(enable_timer), report_measure(report_measure) {
        static_assert(std::is_move_constructible<MeasureType>::value, "This measure is not MoveConstructible");
        static_assert(
           requires { m.accumulate(std::declval<MCSignType>()); }, " This measure has no accumulate method !");
        static_assert(
           requires { m.collect_results(std::declval<mpi::communicator>()); }, " This measure has no collect_results method !");
        using m_t        = std::decay_t<MeasureType>;
        m_t *p           = new m_t(std::forward<MeasureType>(m));
        impl_            = std::shared_ptr<m_t>(p);
        accumulate_      = [p](MCSignType const &x) { p->accumulate(x); };
        count_           = 0;
        collect_results_ = [p](mpi::communicator const &c) { p->collect_results(c); };
        report_          = [p]() -> std::string {
          if constexpr (requires { p->report(); }) {
            return p->report();
          } else {
            (void)p; // suppress clang -Wunused-lambda-capture warning
            TRIQS_RUNTIME_ERROR << "This measure does not implement the report function";
          }
        };
        h5_r = [p](h5::group g, std::string const &name) {
          if constexpr (requires { h5_read(g, name, *p); }) h5_read(g, name, *p);
          else (void)p; // suppress clang -Wunused-lambda-capture warning
        };
        h5_w = [p](h5::group g, std::string const &name) {
          if constexpr (requires { h5_write(g, name, *p); }) h5_write(g, name, *p);
          else (void)p; // suppress clang -Wunused-lambda-capture warning
        };
      }

      //
      measure(measure const &rhs)            = delete;
      measure(measure &&rhs)                 = default;
      measure &operator=(measure const &rhs) = delete;
      measure &operator=(measure &&rhs)      = default;

      void accumulate(MCSignType signe) {
        assert(impl_);
        count_++;
        if (enable_timer) Timer.start();
        accumulate_(signe);
        if (enable_timer) Timer.stop();
      }
      void collect_results(mpi::communicator const &c) {
        if (enable_timer) Timer.start();
        collect_results_(c);
        if (enable_timer) Timer.stop();
      }
      std::string report() const { return report_(); }

      uint64_t count() const { return count_; }
      double duration() const { return double(Timer); }

      friend void h5_write(h5::group g, std::string const &name, measure const &m) {
        if (m.h5_w) m.h5_w(g, name);
      };
      friend void h5_read(h5::group g, std::string const &name, measure &m) {
        if (m.h5_r) m.h5_r(g, name);
      };
    };

    //--------------------------------------------------------------------

    template <typename MCSignType> class measure_set {

      using measure_type = measure<MCSignType>;
      using m_map_t      = std::map<std::string, measure<MCSignType>>;
      m_map_t m_map;

      public:
      using measure_itr_t = typename m_map_t::iterator;

      measure_set()                                  = default;
      measure_set(measure_set const &rhs)            = delete;
      measure_set(measure_set &&rhs)                 = default;
      measure_set &operator=(measure_set const &rhs) = delete;
      measure_set &operator=(measure_set &&rhs)      = default;

      /**
    * Register the Measure M with a name
    */
      template <typename MeasureType> measure_itr_t insert(MeasureType &&M, std::string const &name, bool enable_timer, bool report_measure) {
        if (has(name)) TRIQS_RUNTIME_ERROR << "measure_set : insert : measure '" << name << "' already inserted";
        // workaround for all gcc
        // m_map.insert(std::make_pair(name, measure_type(true, std::forward<MeasureType>(M))));
        auto [itr, was_inserted] = m_map.emplace(name, measure_type(true, std::forward<MeasureType>(M), enable_timer, report_measure));
        return itr;
      }

      /**
       * Remove the measure m.
       */
      void remove(measure_itr_t const &m) { m_map.erase(m); }

      /**
       * Removes all measures
       */
      void clear() { m_map.clear(); }

      /// Does qmc have a measure named name
      bool has(std::string const &name) const { return m_map.find(name) != m_map.end(); }

      ///
      void accumulate(MCSignType const &sign) {
        for (auto &[name, m] : m_map) m.accumulate(sign);
      }

      std::vector<std::string> names() const {
        std::vector<std::string> res;
        for (auto &[name, m] : m_map) res.push_back(name);
        return res;
      }

      std::string report() const {
        std::string result{};
        for (auto &[name, m] : m_map) {
          if (m.report_measure) result += m.report() + "\n";
        }
        return result;
      }

      /// Pretty print the timings of all measures
      std::string get_timings() const {
        std::ostringstream s;
        double total_time = 0;
        int wsec          = 10;
        size_t wlab       = 18; // measure label width, find max length
        for (auto &[name, m] : m_map) wlab = wlab > name.size() ? wlab : name.size();
        s << std::left << std::setw(wlab) << "Measure"
          << " | " << std::setw(wsec) << "seconds" << std::endl;
        ;
        for (auto &[name, m] : m_map) {
          s << std::left << std::setw(wlab) << name << " | " << std::setw(wsec) << m.duration() << "\n";
          total_time += m.duration();
        }
        s << std::left << std::setw(wlab) << "Total measure time"
          << " | " << std::setw(wsec) << total_time << "\n";
        return s.str();
      }

      // gather result for all measure, on communicator c
      void collect_results(mpi::communicator const &c) {
        for (auto &[name, m] : m_map) m.collect_results(c);
      }

      // HDF5 interface
      friend void h5_write(h5::group g, std::string const &key, measure_set const &ms) {
        auto gr = g.create_group(key);
        for (auto &[name, m] : ms.m_map) h5_write(gr, name, m);
      }

      friend void h5_read(h5::group g, std::string const &key, measure_set &ms) {
        auto gr = g.open_group(key);
        for (auto &[name, m] : ms.m_map) h5_read(gr, name, m);
      }
    };

  } // namespace mc_tools
} // namespace triqs
