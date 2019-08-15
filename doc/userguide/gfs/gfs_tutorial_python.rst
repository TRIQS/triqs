.. index:: Green's functions

Local Green's functions
========================================


In this first part, we present a couple of examples in the form of a tutorial.
The full reference is presented in the next sections.

A simple example
----------------

As a first example, we construct and plot the following Matsubara Green's function:

.. math::

  G_{11} (i \omega_n) = \frac{1}{i \omega_n + 0.5}

This is done with the code :

.. plot:: userguide/gfs/example.py
   :include-source:
   :scale: 50

In this very simple example, the Green's function is just a 1x1 block. Let's go through
the different steps of the example:

.. literalinclude:: example.py
   :lines: 1-2

This imports all the necessary classes to manipulate Green's functions. In this example it allows
to use ``GfImFreq``:

.. literalinclude:: example.py
   :lines: 4-5

This creates a **block** Green's function which has just one index
(1). ``beta`` is the inverse temperature, ``n_points`` is the number of
Matsubara frequencies. 

.. literalinclude:: example.py
   :lines: 6-6

This initializes the block with :math:`1/(i \omega_n + 0.5)`.
Two points are worth noting here: 

* The right hand side (RHS) of this statement is a *lazy* expression: its evaluation is delayed until 
  it is needed to fill the Green function.

* The funny *<<* operator means "set from". It fills the Green function with the evaluation of the expression 
  at the right. 
  
.. literalinclude:: example.py
   :lines: 9-10

These lines plot the block Green's function (both the real and imaginary parts) using
the matplotlib plotter. More details can be found in the section
:ref:`plotting`.

A slightly more complicated example
--------------------------------------------------

Let's turn to another example. This time we consider a problem of a d-impurity
level embedded in a flat conduction bath :math:`\Delta` of s-electrons. We want to
construct the corresponding 2x2 Green's function:

.. math::

  \hat{G}^\mathrm{s+d} (\omega_n) = \begin{pmatrix} \omega_n - \epsilon_d & V \\ V & \Delta^{-1} \end{pmatrix}^{-1}

This is done with the code: 

.. plot:: userguide/gfs/impinbath.py
   :include-source:
   :scale: 70

Again, the Green's function is just one block, but this time it is a 2x2 block with
off-diagonal elements. Another difference is that we use real-frequency Green's functions in
this example:

.. literalinclude:: impinbath.py
   :lines: 6-7

In this constructor for the block Green's function, we specify that there are two indices s and d.
Because it is a real-frequency Green's function we need to define the mesh on which it is
computed. This is done with the ``window`` and ``n_points`` options.

.. literalinclude:: impinbath.py
   :lines: 8-11

These lines initialize specific entries of the block Green's function. Note how the elements are
accessed with ``[index1,index2]``. These lines also show how to initialize a Green's function
to a constant or to the inverse of a Flat bath (constant spectral function on an interval :math:`[-D,D]`, with
:math:`D=1.0` in our example).

.. literalinclude:: impinbath.py
   :lines: 12-12

``invert()`` inverts the entire block Green's function (as a matrix).

.. literalinclude:: impinbath.py
   :lines: 14-17

Here, we isolate some elements of the blocks Green's function.  It is also the
first example of an operation on the block Green's function, which is
multiplied by a scalar. The last lines of the script just plot these new
objects.


