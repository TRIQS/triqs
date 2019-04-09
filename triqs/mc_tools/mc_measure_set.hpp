/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by M. Ferrero, O. Parcollet
 * Copyright (C) 2018- The Simons Foundation, Author: H. UR Strand
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
#include <mpi/mpi.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/timer.hpp>
#include <functional>
#include <map>
#include "./impl_tools.hpp"

namespace triqs {
  namespace mc_tools {

    // similar technique as move, cf move_set.
    template <typename MCSignType> class measure {

      std::shared_ptr<void> impl_;
      std::function<void(MCSignType const &)> accumulate_;
      std::function<void(mpi::communicator const &)> collect_results_;
      std::function<void(h5::group, std::string const &)> h5_r, h5_w;

      uint64_t count_;
      bool enable_timer;
      utility::timer Timer;

      public:
      template <typename MeasureType> measure(bool, MeasureType &&m, bool enable_timer) : enable_timer(enable_timer) {
        static_assert(std::is_move_constructible<MeasureType>::value, "This measure is not MoveConstructible");
        static_assert(has_accumulate<MCSignType, MeasureType>::value, " This measure has no accumulate method !");
        static_assert(has_collect_result<MeasureType>::value, " This measure has no collect_results method !");
        using m_t        = std14::decay_t<MeasureType>;
        m_t *p           = new m_t(std::forward<MeasureType>(m));
        impl_            = std::shared_ptr<m_t>(p);
        accumulate_      = [p](MCSignType const &x) { p->accumulate(x); };
        count_           = 0;
        collect_results_ = [p](mpi::communicator const &c) { p->collect_results(c); };
        h5_r             = make_h5_read(p);
        h5_w             = make_h5_write(p);
      }

      //
      measure(measure const &rhs) = delete;
      measure(measure &&rhs)      = default;
      measure &operator=(measure const &rhs) = delete;
      measure &operator=(measure &&rhs) = default;

      void accumulate(MCSignType signe) {
        assert(impl_);
        count_++;
        if(enable_timer) Timer.start();
        accumulate_(signe);
        if(enable_timer) Timer.stop();
      }
      void collect_results(mpi::communicator const &c) {
        if(enable_timer) Timer.start();
        collect_results_(c);
        if(enable_timer) Timer.stop();
      }

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
      using measure_ptr_t = typename m_map_t::const_iterator;

      measure_set()                       = default;
      measure_set(measure_set const &rhs) = delete;
      measure_set(measure_set &&rhs)      = default;
      measure_set &operator=(measure_set const &rhs) = delete;
      measure_set &operator=(measure_set &&rhs) = default;

      /**
    * Register the Measure M with a name
    */
      template <typename MeasureType> measure_ptr_t insert(MeasureType &&M, std::string const &name, bool enable_timer) {
        if (has(name)) TRIQS_RUNTIME_ERROR << "measure_set : insert : measure '" << name << "' already inserted";
        // workaround for all gcc
        // m_map.insert(std::make_pair(name, measure_type(true, std::forward<MeasureType>(M))));
        auto iter_b = m_map.emplace(name, measure_type(true, std::forward<MeasureType>(M), enable_timer));
        return iter_b.first;
      }

      /**
    * Remove the measure m.
    */
      void remove(measure_ptr_t const &m) { m_map.erase(m); }

      /// Does qmc have a measure named name
      bool has(std::string const &name) const { return m_map.find(name) != m_map.end(); }

      ///
      void accumulate(MCSignType const &signe) {
        for (auto &nmp : m_map) nmp.second.accumulate(signe);
      }

      std::vector<std::string> names() const {
        std::vector<std::string> res;
        for (auto &nmp : m_map) res.push_back(nmp.first);
        return res;
      }

      /// Pretty print the timings of all measures
      std::string get_timings() const {
        std::ostringstream s;
        double total_time = 0;
        int wsec          = 10;
        size_t wlab       = 18; // measure label width, find max length
        for (auto &nmp : m_map) wlab = wlab > nmp.first.size() ? wlab : nmp.first.size();
        s << std::left << std::setw(wlab) << "Measure"
          << " | " << std::setw(wsec) << "seconds" << std::endl;
        ;
        for (auto &nmp : m_map) {
          s << std::left << std::setw(wlab) << nmp.first << " | " << std::setw(wsec) << nmp.second.duration() << "\n";
          total_time += nmp.second.duration();
        }
        s << std::left << std::setw(wlab) << "Total measure time"
          << " | " << std::setw(wsec) << total_time << "\n";
        return s.str();
      }

      // gather result for all measure, on communicator c
      void collect_results(mpi::communicator const &c) {
        for (auto &nmp : m_map) nmp.second.collect_results(c);
      }

      // HDF5 interface
      friend void h5_write(h5::group g, std::string const &name, measure_set const &ms) {
        auto gr = g.create_group(name);
        for (auto &p : ms.m_map) h5_write(gr, p.first, p.second);
      }

      friend void h5_read(h5::group g, std::string const &name, measure_set &ms) {
        auto gr = g.open_group(name);
        for (auto &p : ms.m_map) h5_read(gr, p.first, p.second);
      }
    };

  } // namespace mc_tools
} // namespace triqs
