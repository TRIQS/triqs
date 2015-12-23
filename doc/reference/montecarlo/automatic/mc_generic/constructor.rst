..
   Generated automatically using the command :
   c++2doc.py all_triqs.hpp
   /Users/parcolle/triqs/BUILD/triqs/INSTALL_DIR/include/triqs/mc_tools/mc_generic.hpp

.. highlight:: c


.. _mc_generic_constructor:

constructor
=============

**Synopsis**:

.. code-block:: c

    mc_generic (uint64_t n_cycles, uint64_t length_cycle, uint64_t n_warmup_cycles,
   std::string random_name, int random_seed, int verbosity, bool debug = false,
   triqs::mc_tools::mc_type mode) ;

Constructor

Parameters
-------------

* **n_cycles**:        Number of QMC cycles (measures are done after each cycle).

* **length_cycle**:    Number of QMC move attempts in one cycle

* **n_warmup_cycles**: Number of cycles to warm up, before measuring.

* **random_name**:     Name of the random generator (cf doc).

* **random_seed**:     Seed for the random generator

* **verbosity**:       Verbosity level. 0 : None, ... TBA

* **debug**:           Debug mode

* **mode**:            mc_type: At present only mc_type::Metropolis

Return value
--------------

None