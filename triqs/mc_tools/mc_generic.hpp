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
#include "./random_generator.hpp"

namespace triqs { namespace mc_tools {

 /// mc_type
 enum class mc_type {Metropolis}; // HeatBath, ...

 /**
  * \brief Generic Monte Carlo class.
  * 
  * TBR
  * @include triqs/mc_tools.hpp
  */
 template <typename MCSignType> class mc_generic {

   public:

    /**
     * Constructor 
     *
     * @param n_cycles        Number of QMC cycles (measures are done after each cycle).
     * @param length_cycle    Number of QMC move attempts in one cycle
     * @param n_warmup_cycles Number of cycles to warm up, before measuring.
     * @param random_name     Name of the random generator (cf doc).
     * @param random_seed     Seed for the random generator
     * @param verbosity       Verbosity level. 0 : None, ... TBA
     * @param debug           Debug mode 
     * @param mode            mc_type: At present only mc_type::Metropolis   
     */
   mc_generic(uint64_t n_cycles, uint64_t length_cycle, uint64_t n_warmup_cycles, std::string random_name, int random_seed,
              int verbosity, bool debug = false, mc_type mode = mc_type::Metropolis)
      : RandomGenerator(random_name, random_seed)
      , AllMoves(RandomGenerator, debug)
      , AllMeasures()
      , AllMeasuresAux()
      , report(&std::cout, verbosity)
      , Length_MC_Cycle(length_cycle)
      , NWarmIterations(n_warmup_cycles)
      , NCycles(n_cycles)
      , sign_av(0)
      , debug(debug)
      , mode(mode) {}

    /**
     * Register a move 
     *
     * If the move m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.
     *
     * @tparam MoveType                Type of the move. Must model Move concept
     * @param m                        The move. Must model Move concept.
     * @param name                     Name of the move
     * @param proposition_probability  Probability that the move will be proposed. Precondition : >0 
     *                                 NB it but does not need to be normalized. 
     *                                 Normalization is automatically done with all the added moves before starting the run.
     */
    template <typename MoveType>
     void add_move (MoveType && m, std::string name, double proposition_probability = 1.0) {
      static_assert( !std::is_pointer<MoveType>::value, "add_move in mc_generic takes ONLY values !");
      AllMoves.add(std::forward<MoveType>(m), name, proposition_probability);
     }

     /**
      * Register a measure
      *
      * If the measure m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.
      *
      * @param M                        The measure. Must model Measure concept
      * @param name                     Name of the measure
      *
      */
     template <typename MeasureType> void add_measure(MeasureType &&m, std::string name) {
      static_assert(!std::is_pointer<MeasureType>::value, "add_measure in mc_generic takes ONLY values !");
      AllMeasures.insert(std::forward<MeasureType>(m), name);
     }

     /**
      * Register a common part for several measure [EXPERIMENTAL: API WILL CHANGE]
      */
     template <typename MeasureAuxType> void add_measure_aux(std::shared_ptr<MeasureAuxType> p) {
      AllMeasuresAux.emplace_back(p);
     }

     /** 
      * Sets a function called after each cycle
      *
      * @param f The function be called.
      */
     void set_after_cycle_duty(std::function<void()> f) { after_cycle_duty =f; }

     // get the average sign (to be called after collect_results)
     TRIQS_DEPRECATED("average_sign is very misleading. Remove it from your code") MCSignType average_sign() const {
      return sign_av;
     }
  
    /**
     * Starts the Monte Carlo
     *
     * @param sign_init The initial value of the sign (usually 1)
     * @param stop_callback A callback function () -> bool. It is called after each cycle
     *                      to and the computation stops when it returns true.
     *                      Typically used to set up the time limit, cf doc.
     * @return 
     *    =  =============================================
     *    0  if the computation has run until the end
     *    1  if it has been stopped by stop_callback
     *    2  if it has been  topped by receiving a signal
     *    =  =============================================
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
      // Metropolis loop. Switch here for HeatBath, etc...
      for (uint64_t k = 1; (k <= Length_MC_Cycle); k++) {
       if (triqs::signal_handler::received()) goto _final;
       double r = AllMoves.attempt();
       if (RandomGenerator() < std::min(1.0, r)) {
        if (debug) std::cerr << " Move accepted " << std::endl;
        sign *= AllMoves.accept();
        if (debug) std::cerr << " New sign = " << sign << std::endl;
       } else {
        if (debug) std::cerr << " Move rejected " << std::endl;
        AllMoves.reject();
       }
       ++config_id;
      }
      if (after_cycle_duty) {after_cycle_duty();}
      if (is_thermalized()) {
       nmeasures++;
       sum_sign += sign;
       for (auto &x : AllMeasuresAux) x();
       AllMeasures.accumulate(sign);
      }
      // recompute fraction done
     _final:
      uint64_t dp = uint64_t(floor( ( NC*100.0) / (NCycles_tot-1)));
      if (dp>done_percent)  { done_percent=dp; report << done_percent; report<<"%; "; report <<std::flush; }
      finished = ( (NC >= NCycles_tot -1) || is_converged () );
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
     AllMeasures.collect_results(c);
     AllMoves.collect_statistics(c);
     uint64_t nmeasures_tot = mpi::reduce(nmeasures,c);
     
     report(3) << "[Node "<<c.rank()<<"] Acceptance rate for all moves:\n" << AllMoves.get_statistics();
     report(3) << "[Node "<<c.rank()<<"] Simulation lasted: " << double(Timer) << " seconds" << std::endl;
     report(3) << "[Node "<<c.rank()<<"] Number of measures: " << nmeasures  << std::endl;
     if (c.rank() == 0) report(2) << "Total number of measures: " << nmeasures_tot << std::endl;
     mpi::broadcast(sign_av, c);
    }

    /**
     * The acceptance rates of all move
     *
     * @return map : name_of_the_move -> acceptance rate of this move
     */
    std::map<std::string, double> get_acceptance_rates() const { return AllMoves.get_acceptance_rates(); }
 
    /** 
     *  The current percents done
     */  
    uint64_t get_percent() const { return done_percent; }

    /** 
     * An access to the random number generator
     */
    random_generator & get_rng(){ return RandomGenerator;}

    // backward compatibility only
    TRIQS_DEPRECATED("TRIQS API change: please use get_rng instead of rng. This function WILL be removed in future releases.")
    random_generator &rng() { return RandomGenerator; }

    /** 
     * The current cycle number 
     */
    int get_current_cycle_number() const { return NC; }

    /** 
     * The current number of the visited configuration. Updated after each accept/reject.
     */
    int get_config_id () const { return config_id; }

    /** 
     * Is the qmc thermalized, i.e. has it run more than n_warmup_cycles given at construction
     */
    bool is_thermalized() const { return (NC >= NWarmIterations); }

    private: // future use
    bool is_converged() const { return false; }

    public:
    
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
    bool debug;
    mc_type mode;
    uint64_t config_id = 0;
  };

}}// end namespace

