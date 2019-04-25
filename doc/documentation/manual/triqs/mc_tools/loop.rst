.. _montecarloloop:

Introduction
-------------

The Monte Carlo loop
*********************

The ``mc_generic`` class is an implementation of the Monte Carlo loop.  Its
goal is to propose and then accept or reject changes to a configuration
according to this loop:

.. image:: loop.png
   :width: 700
   :align: center

As shown in the figure, after a first initialization, the loop starts by
proposing an update. In the following, we generically refer to this proposal as
a proposed **move**. The move proposes a modification of the state of the
system, which we call the **configuration** of the system. After having
computed the transition probabilities between this proposed configuration and
the old one, as well as their probability density, we compute an acceptance
probability for the move. Based on this probability, the move is either
**accepted** or **rejected**. If it is rejected, nothing happens and we remain
in the same configuration. If it is accepted, the configuration is updated.

This procedure is the heart of the Monte Carlo algorithm and is repeated
at every Monte Carlo **step** (meaning one loop). Measurements are not
made at every step, to allow for some decorrelation between measured
configurations. Thus, measurements are made every :math:`L` steps. We
say that these :math:`L` steps form a **cycle** and :math:`L` is the
length of a cycle.

At the very beginning of the simulation, one usually allows for :math:`W`
**warmup** (thermalization) cycles. This means that there will be no measurements
during these first :math:`W` cycles. After that, we define :math:`N`, the
number of cycles that will be done until the end of the simulation.

At the end of the simulation, the code will have done:

  * :math:`N` measurements
  * :math:`N + W` cycles
  * :math:`(N + W) \times L` steps


C++ variable names
******************

In the C++ examples, these variables will be called:

  * ``n_cycles`` :math:`= N`
  * ``length_cycle`` :math:`= L`
  * ``n_warmup_cycle`` :math:`= W`

You will also have to use these names if you will construct an ``mc_generic``
instance from a dictonary (see full documentation/manual/triqs below).

Monte Carlo loop and connection with moves and measures
*******************************************************

We will cover this in more details, but let us already mention here that the
``mc_generic`` class only implements the Monte Carlo loop. It doesn't need (and
in fact doesn't) know anything about what the configuration is or what the
moves and measurements really do. All it does, is to use external classes which
take care of making the moves. It just expects back a Metropolis ratio so that
it can decide wether the move should be accepted or rejected. Once this choice
is made, it tells the external class which again does we is needed if the move
is accepted or rejected. The same is true for measurements which are external
classes called by the loop.  This will become clearer with an example in the
following section.

.. note::

  Above, we described the Metropolis algorithm. A different accept/reject
  scheme could be used but the mechanism remains the same.
