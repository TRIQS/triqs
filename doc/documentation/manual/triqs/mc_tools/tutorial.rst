.. highlight:: c

Tutorial
---------

In order to have a first overview of the main features of the ``mc_generic``
class, let's start with a concrete Monte Carlo code. We will consider maybe the
simplest problem ever: a single spin in a magnetic field :math:`h` at a
temperature :math:`1/\beta`. The Hamiltonian is simply:

.. math::

  \mathcal{H} = - h (n_\uparrow - n_\downarrow).

You want to compute the magnetization of this single spin. From statistical
mechanics it is clearly just

.. math::

  m = \frac{\exp(\beta h) - \exp(-\beta h)}{\exp(\beta h) + \exp(-\beta h)}


The C++ code for this problem
*****************************

Let's see how we can get this result from a Monte Carlo simulation. Here is
a code that would do the job. Note that we put everything in one file here,
but obviously you would usually want to cut this into pieces for clarity:

.. literalinclude:: overview_ex.cpp
   :language: cpp

.. literalinclude:: overview_ex.output

Let's go through the different parts of this code. First we look
at ``main()``.


Initializing the MPI
********************

As you will see, the Monte Carlo class is completely MPI ready.  The first two
lines of the ``main()`` just initialize the MPI environment and declare a
communicator. The default communicator is ``WORLD`` which means that all the
nodes will be involved in the calculation::

    triqs::mpi::environment env(argc, argv);
    triqs::mpi::communicator world;


Constructing the Monte Carlo simulation
***************************************

The lines that follow, define the parameters of the Monte
Carlo simulation and construct a Monte Carlo object
called ``SpinMC``::

    int n_cycles = 5000000;
    int length_cycle = 10;
    int n_warmup_cycles = 10000;
    std::string random_name = "";
    int random_seed = 374982 + world.rank() * 273894;
    int verbosity = (world.rank() == 0 ? 2: 0);

    triqs::mc_tools::mc_generic<double> SpinMC(random_name, random_seed, initial_sign, verbosity);

The ``SpinMC`` is an instance of the ``mc_generic`` class. First of all, note
that you need to include the header ``<triqs/mc_tools/mc_generic.hpp>`` in
order to access the ``mc_generic`` class. The ``mc_generic`` class is a
template on the type of the Monte Carlo sign. Usually this will be either a
``double`` or a ``complex<double>``.

The first two arguments define the random number generator by giving its name in
``random_name`` (an empty string means the default generator, i.e. the Mersenne
Twister) and the random seed in ``random_seed``. As you see the seed is
different for all node with the use of ``world.rank()``.

The third argument is the sign of the very first *configuration* of the
simulation. Because the ``accept`` method only returns a ratio, this initial
sign is used to determine the sign of all generated configurations.

Finally, the last parameter sets the verbosity level. 0 means no output, 1 will
output the progress level for the current node and 2 additionally shows some
statistics about the simulation when you call ``collect_results``. As you see,
we have put ``verbosity`` to 2 only for the master node and 0 for all the other
ones. This way the information will be printed only by the master.

Moves and measures
******************

At this stage the basic structure of the Monte Carlo is in ``SpinMC``. But we
now need to tell it what moves must be tried and what measures must be made.
This is done with:

.. literalinclude:: overview_ex.cpp
   :language: cpp
   :lines: 76-77

The method ``add_move`` expects a move and a name, while
``add_measure`` expects a measure and a name. The name can be
anything, but different measures must have different names. In this example,
the move is an instance of the ``flip`` class and the measure an instance of
the ``compute_m`` class. These classes have been defined in the beginning of
the code and they have no direct connection with the ``mc_generic`` class (e.g.
they don't have inheritance links with ``mc_generic``).  Actually you are
almost completely free to design these classes as you want, **as long as they
satisfy the correct concept**.

The move
**********

Let's go back to the beginning of the code and have a look at the ``flip``
class which proposed a flip of the spin. The class is very short.  It has a
constructor which might define some class variables. But more importantly, it
has three member functions that any move **must** have: ``attempt``, ``accept`` and
``reject``:

.. literalinclude:: overview_ex.cpp
   :language: cpp
   :lines: 14-25

The ``attempt`` method is called by the Monte Carlo loop in order to try a new
move. The Monte Carlo class doesn't care about what this trial is. All that
matters for the loop is the Metropolis ratio describing the transition to a new
proposed configuration. It is precisely this ratio that the ``attempt`` method is
expected to return:

.. math::

  T = \frac{P_{y,x} \rho(y)}{P_{x,y}\rho(x)}

In our example this ratio is

.. math::

  T = \frac{e^{\beta h -\sigma }}{e^{\beta h \sigma}} = e^{ - 2 \beta h \sigma }

With this ratio, the Monte Carlo loop decides whether this proposed move should
be rejected, or accepted. If the move is accepted, the Monte Carlo calls the
``accept`` method of the move, otherwise it calls the ``reject`` method.  The
``accept`` method should always return 1.0 unless you want to correct the sign
only when moves are accepted for performance reasons (this rather special case
is described in the :ref:`full documentation/manual/triqs <montecarloref>`).  Note that the
return type of ``attempt`` and ``accept`` has to be the same as the template of the
Monte Carlo class.  In our example, nothing has to be done if the move is
rejected. If it is accepted, the spin should be flipped.

The measure
************

Just in the same way, the measures are expected to satisfy a concept.
Let's look at ``compute_m``.

.. literalinclude:: overview_ex.cpp
   :language: cpp
   :lines: 28-45

Here only two methods are expected, ``accumulate`` and ``collect_results``.
The method ``accumulate`` is called every ``length_cycle`` Monte Carlo loops.
It takes one argument which is the current sign in the Monte Carlo simulation.
Here, we sum the sign in ``Z`` (the partition function) and the magnetization
in ``M``. The other method ``collect_results`` is usually called just once at
the very end of the simulation, see below. It is meant to do the final
operations that are needed to have your result. Here it just needs to divide
``M`` by ``Z`` and prints the result on the screen. Note that, it takes the MPI
communicator as an argument, meaning that you can easily do MPI operations
here.  This makes sense because the accumulation will have taken place
independently on all nodes and this is the good moment to gather the
information from all the nodes. This is why you see reduce operations on the
master node here.


Starting the Monte Carlo simulation
***********************************

Well, at this stage we're ready to launch our simulation. The moves
and measures have been specified, so all you need to do now is start
the simulation with::

    SpinMC.warmup_and_accumulate(n_warmup_cycles, n_cycles, length_cycle, triqs::utility::clock_callback(600));

The ``warmup`` method takes several arguments.

The first three parameters determine the warmup length, number of measurements
and the length of the Monte Carlo cycles.  The definition of these variables
has been detailed earlier in :ref:`montecarloloop`.

The last argument is used to decide if the simulation must be stopped for some
reason before it reaches the full number of cycles ``n_cycles``. For example,
you might be running your code on a cluster that only allows for 1 hour
simulations. In that case, you would want your simulation to stop, say after 55
minutes, even if it didn't manage to do the ``n_cycles`` cycles.

In practice, the second argument is a ``std::function<bool ()>`` which is
called at the end of every cycle. If it returns 0 the simulation goes on, if it
returns 1 the simulation stops. In this example, we used a function
``clock_callback(600)`` which starts returning 1 after 600 seconds.  It is
defined in the header :file:`<triqs/utility/callbacks.hpp>`.  This way the
simulation will last at most 10 minutes.

Note that the simulation would end cleanly. The rest of the code can safely
gather results from the statistics that has been accumulated, even if there
have been less than ``n_cycles`` cycles.


End of the simulation - gathering results
*****************************************

When the simulation is over, it is time to gather the results.  This is done by
calling::

    SpinMC.collect_results(world);

In practice this method goes through all the measurements that have been added
to the simulation and calls their ``collect_results`` member. As described
above, this does the final computations needed to get the result you are
interested in. It usually also saves or prints these results.


Writing your own Monte Carlo simulation
***************************************

I hope that this simple example gave you an idea about how to use the
``mc_generic`` class. In the next chapter we will address some more advanced
issues, but you should already be able to implement a Monte Carlo simulation of
your own. Maybe the only point that we haven't addressed and which is useful,
is how to generate random numbers. Actually, as soon as you have generated an
instance of a ``mc_generic`` class, like ``SpinMC`` above, you automatically
have an acces to a random number generator with::

  triqs::mc_tools::random_generator RNG = SpinMC.get_rng();

``RNG`` is an instance of a ``random_generator``. If you want to
generate a ``double`` number on the interval :math:`[0,1[`, you just have to
call ``RNG()``. By providing an argument to ``RNG`` you can generate integer
and real numbers on different intervals. This is described in detail in the
section :ref:`Random number generator <random>`.

That's it! Why don't you try to write your own Monte Carlo describing an
:ref:`Ising chain in a field <isingex>`? You will find the solution
in :ref:`this section <ising_solution>`.


