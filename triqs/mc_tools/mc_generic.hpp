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
#include <triqs/utility/first_include.hpp>
#include <math.h>
#include <triqs/utility/timer.hpp>
#include <triqs/utility/report_stream.hpp>
#include <triqs/utility/signal_handler.hpp>
#include <triqs/mpi/base.hpp>
#include "./mc_measure_aux_set.hpp"
#include "./mc_measure_set.hpp"
#include "./mc_move_set.hpp"
#include "./mc_basic_step.hpp"
#include "./random_generator.hpp"

namespace triqs { namespace mc_tools {

 /**
  * \brief Generic Monte Carlo class.
  */
 template<typename MCSignType, typename MCStepType = Step::Metropolis<MCSignType> >
  class mc_generic {

   public:

    /**
     * Constructor from a set of parameters
     */
   mc_generic(uint64_t n_cycles, uint64_t length_cycle, uint64_t n_warmup_cycles, std::string random_name, int random_seed,
              int verbosity, std::function<void()> AfterCycleDuty = std::function<void()>())
      : RandomGenerator(random_name, random_seed)
      , AllMoves(RandomGenerator)
      , AllMeasures()
      , AllMeasuresAux()
      , report(&std::cout, verbosity)
      , Length_MC_Cycle(length_cycle)
      , NWarmIterations(n_warmup_cycles)
      , NCycles(n_cycles)
      , after_cycle_duty(AfterCycleDuty)
      , sign_av(0) {}

    /// Set function after_cycle_duty
    void set_after_cycle_duty(std::function<void()> AfterCycleDuty){ after_cycle_duty = AfterCycleDuty; }

    /**
     * Register move M with its probability of being proposed.
     * NB : the proposition_probability needs to be >0 but does not need to be
     * normalized. Normalization is automatically done with all the added moves
     * before starting the run
     */
    template <typename MoveType>
     void add_move (MoveType && M, std::string name, double proposition_probability = 1.0) {
      static_assert( !std::is_pointer<MoveType>::value, "add_move in mc_generic takes ONLY values !");
      AllMoves.add(std::forward<MoveType>(M), name, proposition_probability);
     }

    /**
     * Register the Measure M
     */
    template<typename MeasureType>
     void add_measure (MeasureType && M, std::string name) {
      static_assert( !std::is_pointer<MeasureType>::value, "add_measure in mc_generic takes ONLY values !");
      AllMeasures.insert(std::forward<MeasureType>(M), name);
     }

     /**
     * Register the precomputation 
     */
     template <typename MeasureAuxType> void add_measure_aux(std::shared_ptr<MeasureAuxType> p) { AllMeasuresAux.emplace_back(p); }

     /// get the average sign (to be called after collect_results)
    MCSignType average_sign() const { return sign_av; }

    /// get the current percents done
    uint64_t percent() const { return done_percent; }

    // An access to the random number generator
    random_generator & rng(){ return RandomGenerator;}

    // An access to the current cycle number 
    int current_cycle_number() const { return NC; }

    bool thermalized() const { return (NC>= NWarmIterations);}
    bool converged() const { return false;}

    /**
     * Start the Monte Carlo
     *
     * @param sign_init The initial value of the sign (usually 1)
     * @param stop_callback A function () -> bool that is called after each cycle
     *                      to determine if the computation should continue.
     *                      Typically the time limit
     * @return 0 if the computation has run until the end.
     *         1 if it has been stopped by stop_callback
     *         2 if it has been  topped by receiving a signal
     */
    int start(MCSignType sign_init, std::function<bool ()> stop_callback) {
     Timer.start();
     triqs::signal_handler::start();
     sign = sign_init; done_percent = 0; nmeasures = 0;
     sum_sign = 0;
     bool stop_it=false, finished = false;
     uint64_t NCycles_tot = NCycles+ NWarmIterations;
     report << std::endl << std::flush;
     for (NC = 0; !stop_it; ++NC) {
      for (uint64_t k=1; (k<=Length_MC_Cycle); k++) {
       if (triqs::signal_handler::received()) goto _final;
       MCStepType::do_it(AllMoves, RandomGenerator, sign);
      }
      if (after_cycle_duty) {after_cycle_duty();}
      if (thermalized()) {
       nmeasures++;
       sum_sign += sign;
       for (auto &x : AllMeasuresAux) x();
       AllMeasures.accumulate(sign);
      }
      // recompute fraction done
     _final:
      uint64_t dp = uint64_t(floor( ( NC*100.0) / (NCycles_tot-1)));
      if (dp>done_percent)  { done_percent=dp; report << done_percent; report<<"%; "; report <<std::flush; }
      finished = ( (NC >= NCycles_tot -1) || converged () );
      stop_it = (stop_callback() || triqs::signal_handler::received() || finished);
     }
     int status = (finished ? 0 : (triqs::signal_handler::received() ? 2 : 1));
     Timer.stop();
     if (status == 1) report << "mc_generic stops because of stop_callback";
     if (status == 2) report << "mc_generic stops because of a signal";
     report << std::endl << std::endl << std::flush;
     triqs::signal_handler::stop();
     return status;
    }

    /// Reduce the results of the measures, and reports some statistics
    void collect_results(mpi::communicator const & c) {

     uint64_t nmeasures_tot = mpi::reduce(nmeasures,c);

     report(3) << "[Node "<<c.rank()<<"] Acceptance rate for all moves:\n" << AllMoves.get_statistics(c);
     report(3) << "[Node "<<c.rank()<<"] Simulation lasted: " << double(Timer) << " seconds" << std::endl;
     report(3) << "[Node "<<c.rank()<<"] Number of measures: " << nmeasures  << std::endl;

     if (c.rank() == 0) report(2) << "Total number of measures: " << nmeasures_tot << std::endl;
     mpi::broadcast(sign_av, c);
     AllMeasures.collect_results(c);
    }

    /// HDF5 interface
    friend void h5_write (h5::group g, std::string const & name, mc_generic const & mc){
     auto gr = g.create_group(name);
     h5_write(gr,"moves", mc.AllMoves);
     h5_write(gr,"measures", mc.AllMeasures);
     h5_write(gr,"length_monte_carlo_cycle", mc.Length_MC_Cycle);
     h5_write(gr,"number_cycle_requested", mc.NCycles);
     h5_write(gr,"number_warming_cycle_requested", mc.NWarmIterations);
     h5_write(gr,"number_cycle_done", mc.NC);
     h5_write(gr,"number_measure_done", mc.nmeasures);
     h5_write(gr,"sign", mc.sign);
     h5_write(gr,"sum_sign", mc.sum_sign);
    }

    /// HDF5 interface
    friend void h5_read (h5::group g, std::string const & name, mc_generic & mc){
     auto gr = g.open_group(name);
     h5_read(gr,"moves", mc.AllMoves);
     h5_read(gr,"measures", mc.AllMeasures);
     h5_read(gr,"length_monte_carlo_cycle", mc.Length_MC_Cycle);
     h5_read(gr,"number_cycle_requested", mc.NCycles);
     h5_read(gr,"number_warming_cycle_requested", mc.NWarmIterations);
     h5_read(gr,"number_cycle_done", mc.NC);
     h5_read(gr,"number_measure_done", mc.nmeasures);
     h5_read(gr,"sign", mc.sign);
     h5_read(gr,"sum_sign", mc.sum_sign);
    }

   private:
    random_generator RandomGenerator;
    move_set<MCSignType> AllMoves;
    measure_set<MCSignType> AllMeasures;
    std::vector<measure_aux> AllMeasuresAux;
    utility::report_stream report;
    uint64_t Length_MC_Cycle;/// Length of one Monte-Carlo cycle between 2 measures
    uint64_t NWarmIterations, NCycles;
    uint64_t nmeasures;
    MCSignType sum_sign;
    utility::timer Timer;
    std::function<void()> after_cycle_duty;
    MCSignType sign, sign_av;
    uint64_t NC,done_percent;// NC = number of the cycle
  };

}}// end namespace

