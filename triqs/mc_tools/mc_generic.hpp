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
#ifndef TRIQS_TOOLS_MC_GENERIC_H
#define TRIQS_TOOLS_MC_GENERIC_H
#include <math.h>
#include <triqs/utility/timer.hpp>
#include <triqs/utility/report_stream.hpp>
#include <triqs/parameters/parameters.hpp>
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
    mc_generic(uint64_t N_Cycles, uint64_t Length_Cycle, uint64_t N_Warmup_Cycles, std::string Random_Name, int Random_Seed, int Verbosity,
      std::function<bool()> AfterCycleDuty = std::function<bool()>() ) :
     RandomGenerator(Random_Name, Random_Seed),
     AllMoves(RandomGenerator),
     AllMeasures(),AllMeasuresAux(),
     report(&std::cout, Verbosity),
     Length_MC_Cycle(Length_Cycle),
     NWarmIterations(N_Warmup_Cycles),
     NCycles(N_Cycles),
     after_cycle_duty(AfterCycleDuty),
     sign_av(0) {}

    /**
     * Constructor from a dictionnary
     * \param[in] P  dictionary parameters
     * \param[in] AfterCycleDuty  a function bool() to be called after each QMC cycle
     */
     mc_generic(utility::parameters const & P, std::function<bool()> AfterCycleDuty = std::function<bool()>() ) :
      RandomGenerator(std::string(P["random_name"]), long(P["random_seed"])),
      report(&std::cout,int(P["verbosity"])),
      AllMoves(RandomGenerator),
      AllMeasures(),AllMeasuresAux(),
      Length_MC_Cycle(long(P["length_cycle"])),     /// NOT NICE THIS EXPLICIT CAST : no unsigned in parameters, really ??
      NWarmIterations(long(P["n_warmup_cycles"])),
      NCycles(long(P["n_cycles"])),
      after_cycle_duty(AfterCycleDuty),
      sign_av(0) {}

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
    /*template<typename MeasureAuxType>
     MeasureAuxType * add_measure_aux(MeasureAuxType && M, std::string name) {
      static_assert( !std::is_pointer<MeasureAuxType>::value, "add_measure_aux in mc_generic takes ONLY values !");
      AllMeasuresAux.insert(std::forward<MeasureAuxType>(M), name);
     }
*/
   /// get the average sign (to be called after collect_results)
    MCSignType average_sign() const { return sign_av; }

    /// get the current percents done
    uint64_t percent() const { return done_percent; }

    // An access to the random number generator
    random_generator & rng(){ return RandomGenerator;}

    /// Start the Monte Carlo
    bool start(MCSignType sign_init, std::function<bool ()> const & stop_callback) {
     assert(stop_callback);
     Timer.start();
     sign = sign_init; done_percent = 0; nmeasures = 0;
     sum_sign = 0;
     bool stop_it=false, finished = false;
     uint64_t NCycles_tot = NCycles+ NWarmIterations;
     report << std::endl << std::flush;
     for (NC =0; !stop_it; ++NC) {
      for (uint64_t k=1; (k<=Length_MC_Cycle); k++) { MCStepType::do_it(AllMoves,RandomGenerator,sign); }
      if (after_cycle_duty) {after_cycle_duty();}
      if (thermalized()) {
       nmeasures++;
       sum_sign += sign;
       AllMeasuresAux.compute_all(); 
       AllMeasures.accumulate(sign);
      }
      // recompute fraction done
      uint64_t dp = uint64_t(floor( ( NC*100.0) / (NCycles_tot-1)));
      if (dp>done_percent)  { done_percent=dp; report << done_percent; report<<"%; "; report <<std::flush; }
      finished = ( (NC >= NCycles_tot -1) || converged () );
      stop_it = (stop_callback() || finished);
     }
     report << std::endl << std::endl << std::flush;
     Timer.stop();
     return finished;

    }

    /// Reduce the results of the measures, and reports some statistics
    void collect_results(boost::mpi::communicator const & c) {

     uint64_t nmeasures_tot;
     MCSignType sum_sign_tot;
     boost::mpi::reduce(c, nmeasures, nmeasures_tot, std::plus<uint64_t>(), 0);
     boost::mpi::reduce(c, sum_sign, sum_sign_tot, std::plus<MCSignType>(), 0);

     sign_av = sum_sign_tot / double(nmeasures_tot);

     report(3) << "[Node "<<c.rank()<<"] Acceptance rate for all moves:" << std::endl << std::endl;
     report(3) << "[Node "<<c.rank()<<"]"<< AllMoves.get_statistics(c) << std::endl;
     report(3) << "[Node "<<c.rank()<<"] Simulation lasted: " << double(Timer) << " seconds" << std::endl;
     report(3) << "[Node "<<c.rank()<<"] Number of measures: " << nmeasures  << std::endl;
     report(3) << "[Node "<<c.rank()<<"] Average sign: " << sum_sign / double(nmeasures) << std::endl << std::endl << std::flush;
  
     if (c.rank()==0) {
      report(2) << "Total number of measures: " << nmeasures_tot << std::endl;
      report(2) << "Average sign: " << sign_av << std::endl << std::endl << std::flush;
     }
      
    AllMeasures.collect_results(c);

    }

    // do not use direcly, use the free function it is simpler to call...
    template<typename MeasureType> MeasureType       & get_measure(std::string const & name)       { return AllMeasures.template get_measure<MeasureType> (name); }
    template<typename MeasureType> MeasureType const & get_measure(std::string const & name) const { return AllMeasures.template get_measure<MeasureType> (name); }
 
    template<typename MeasureAuxType> MeasureAuxType       * get_measure_aux(std::string const & name)       { return AllMeasuresAux.template get_measure_aux<MeasureAuxType> (name); }
    template<typename MeasureAuxType> MeasureAuxType const * get_measure_aux(std::string const & name) const { return AllMeasuresAux.template get_measure_aux<MeasureAuxType> (name); }

    template<typename MoveType> MoveType       & get_move (std::string const & name)       { return AllMoves.template get_move<MoveType> (name); }
    template<typename MoveType> MoveType const & get_move (std::string const & name) const { return AllMoves.template get_move<MoveType> (name); }

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
    measure_aux_set AllMeasuresAux;
    utility::report_stream report;
    uint64_t Length_MC_Cycle;/// Length of one Monte-Carlo cycle between 2 measures
    uint64_t NWarmIterations, NCycles;
    uint64_t nmeasures;
    MCSignType sum_sign;
    utility::timer Timer;
    std::function<bool()> after_cycle_duty;
    MCSignType sign, sign_av;
    uint64_t NC,done_percent;// NC = number of the cycle

    bool thermalized() const { return (NC>= NWarmIterations);}
    bool converged() const { return false;}
  };


 /// Retrieve a Measure given name and type. NB : the type is checked at runtime
 template<typename M,typename T1, typename T2> M       & get_measure(mc_generic<T1,T2> & s, std::string const & name)       { return s.template get_measure<M> (name); }
 template<typename M,typename T1, typename T2> M const & get_measure(mc_generic<T1,T2> const & s, std::string const & name) { return s.template get_measure<M> (name); }
 
 /// Retrieve a Measure given name and type. NB : the type is checked at runtime
 template<typename M,typename T1, typename T2> M       * get_measure_aux(mc_generic<T1,T2> & s, std::string const & name)       { return s.template get_measure_aux<M> (name); }
 template<typename M,typename T1, typename T2> M const * get_measure_aux(mc_generic<T1,T2> const & s, std::string const & name) { return s.template get_measure_aux<M> (name); }

 /// Retrieve a Move given name and type. NB : the type is checked at runtime
 template<typename M,typename T1, typename T2> M & get_move(mc_generic<T1,T2> & s, std::string const & name)             { return s.template get_move<M> (name); }
 template<typename M,typename T1, typename T2> M const & get_move(mc_generic<T1,T2> const & s, std::string const & name) { return s.template get_move<M> (name); }

}}// end namespace
#endif

