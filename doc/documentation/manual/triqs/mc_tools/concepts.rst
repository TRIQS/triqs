.. _montecarloref:

Concepts
--------

Introduction & Motivations
**************************

* **Purpose**:

  The purpose of this little class is too facilitate the writing and maintenance
  of Metropolis MonteCarlo algorithms, with several **advantages** :

   * Easy to add new moves and measures.
   * For complex MC algorithms, with 6 or 7 different moves, it helps to clearly separate the code for each move.
   * Parallelism is automatic.
   * The random generator is a simple parameter, it can be chosen dynamically.

* **Principle**

  The `mc_generic` class is a generic version of the algorithms, with `moves` and `measures`.
  The user :

    - writes move classes, modelling the Move concept.
    - writes measure classes, modelling the Measure concepts.
    - register them in a mc_generic object MC
    - call `MC.warmup_and_accumulate(...)`  ... and that is (almost) it  !


The Move concept
****************

  ========================== =============================================================================================
  Elements                   Comment
  ========================== =============================================================================================
  * mc_sign_type attempt()   - First part of the Move.
                             - Returns the probability to accept the move. If :

                               - the move is :math:`x\rightarrow x'`, proposed with proba :math:`T_{x\rightarrow x'}`
                               - the probability of the :math:`x` config is denoted :math:`p_x`
                               - then attempt should return the Metropolis ratio:

                               .. math::
                                   \frac{p_{x'} T_{x'\rightarrow x}}{p_x T_{x\rightarrow x'}}

                               with the sign. (The sign will be separated from the absolute value and kept by the MonteCarlo class).
                               In other words: attempt() = move_sign * move_rate with abs(move_sign) = 1
  * mc_sign_type accept()    - Called iif the Move is accepted.
                             - Returns a number r such that :math:`|r| =1`
                             - There is a possibility to update the sign
                               here as well, so that the final sign is: move_sign * accept()
                             - CAREFUL that we need the ratio new_sign / old_sign here just like
                               we needed a ratio in attempt()
  * void reject()            - Called iif the Move is rejected (for cleaning).
  ========================== =============================================================================================


The Measure concept
*******************


  ==========================================================================  ============================================================
  Elements                                                                    Comment
  ==========================================================================  ============================================================
  * void accumulate(std::complex<double> sign)                                - Accumulation with the sign
  * void collect_results ( boost::mpi::communicator const & c)                - Collects the results over the communicator, and finalize
                                                                                the calculation (compute average, error).
  ==========================================================================  ============================================================
