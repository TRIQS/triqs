.. index::
  single: Green's functions; block Green's function
  module: gf_imfreq
  module: gf_refreq
  module: gf_imtime
  module: gf_retime
  module: gf_legendre

.. _blockgreen:

The blocks: matrix-valued Green's functions
===============================================

In this section, we present the matrix-valued Green's functions,
i.e. the blocks of the full local Green's function.
They are available in various flavours:

.. toctree::
   :maxdepth: 1

  block/GfImTime
  block/GfImFreq
  block/GfReTime
  block/GfReFreq
  block/GfLegendre

They have many common properties, which we now present.


Operations
--------------------------------------------

Block Green's functions support various simple operations.

.. note::
   All these operations compute the array of data, but also, if present in the object, the high frequency expansion tail automatically.

* compound operators, `+=`, `-=`, `*=`, `\=`: the RHS can be a Green's function of the same type or an expression

* arithmetic operations: `+`, `-`, `*`, `/`, e.g.::

   g = g1 + 2*g2

* inversion, e.g.::

   inv = inverse(g)
   g2 = inverse(inverse(g) - sigma) # this is Dyson's equation

Slicing
-------

Just like numpy arrays, the Green's function can be sliced, *when the indices are integers* (otherwise it is meaningless).
The syntax is the regular python/numpy syntax, so a simple example will be enough here::

  >>> from triqs.gf import *
  >>> g = GfImFreq(indices = [1,2,3], beta = 50, n_points = 1000, name = "imp")
  >>> g[1:3:,1:3]
  gf_view

  >>> g[1,1]
  gf_view

  >>> g[2:3,2:3]
  gf_view


Bracket Accessor
----------------

The operator [] is further in charge of accessing the data of the Green function on the mesh.
It allows to acquire both the values the Green function and alter them.

To not collide with the slicing functionality described above, two wrapper classes Idx and Meshpoint are used.
The former allows to access the Green function at a given :ref:`mesh index <gf_subscript_index>`

.. code::

        from triqs.gf import Idx
        ...
        g[Idx(0)] = 1.0


Mesh points are acquired naturally when looping over the mesh of a Green function. For example, we can set
the values of the Green function using

.. code::

        for iw in g_iw.mesh():
            g[iw] = 1.0 / iw


Assignment: << or = operator
--------------------------------------------

Because python always uses documentation/manual/triqss, one cannot simply use the = operator
to assign some expression into a Green's function.
Therefore, we introduced the << operator ::

  g << RHS

This sets the data and tail of g with the RHS.

  * If RHS is Green's function, it must be of the same type and size must match
  * If RHS is a formal expression, it must be of the same size

To simplify the notation, in the case where one accesses the Green's function with a [ ] operation,
the = sign is possible and equivalent to the `<<` operator.

.. warning::
   Don't use the = operator without the brackets: it has its normal python meaning, i.e.
   reassigning the documentation/manual/triqs.

   Let us illustrate this issue on a simple example::

    from triqs.gf import *
    # Create the Matsubara-frequency Green's function
    g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")

    g    << inverse( Omega + 0.5 )   # correct
    g[1,1] = inverse( Omega + 0.5 )   # correct (it uses __setitem__).


   However, the following line is almost certainly NOT what you have in mind::

    g = inverse( Omega + 0.5 )

   * The documentation/manual/triqs g is reassigned to the object `inverse( Omega + 0.5 )`, which is not a block Green's function but a lazy expression.
   * The block created earlier is destroyed immediately.


Lazy expressions
----------------

To initialize the Green's function, one can use lazy_expression, made of Green's functions, `descriptors`
assembled with basic operations.

:ref:`descriptors<descriptors>` are abstract objects that do not contain data, but describe a simple function and
can be evaluated, can compute the high-frequency expansion, and so on. For example:

 * `Omega`: is the function :math:`f(\omega) = \omega`.
 * `SemiCircular(D)`: is a Green's function corresponding to free fermions with a semicircular density of states of half-bandwith `D`.
 * `Flat`: is a Green's function corresponding to fermions with a flat density of states of half-bandwidth `D`.

.. toctree::
   :maxdepth: 1

  descriptors

shelve / pickle
---------------

Green's functions are `picklable`, i.e. they support the standard python serialization techniques.

* It can be used with the `shelve <http://docs.python.org/library/shelve.html>`_ and `pickle <http://docs.python.org/library/pickle.html>`_  module::

     import shelve
     s = shelve.open('myfile','c')
     s['G'] = g  # g is stored in the file.

* It can be sent/broadcasted/reduced over mpi ::

     from triqs.utility import mpi
     mpi.send (g, destination)

.. warning::
   Shelve is not a portable format, it may change from python version to another (and it actually does).
   For portability, we recommend using the HDF5 interface for storing data on disks.


Plot options
------------

There is one important option that you will find very useful when plotting Green's functions, which we
saw already in the previous section:

* `mode` = 'R' or 'I' or 'S'

It tells the plotter, what part of the Green's function you want to plot. 'R' for the real part, 'I'
for the imaginary part, and 'S' for the spectral function, :math:`-1/\pi{\rm Im}G`. Of course,
depending on the type of Green's function under consideration, one or more of these options do not make a lot of sense, e.g.
calculating the spectral function of an imaginary-time Green's function is not useful.


Direct access to data points
-----------------------------------------------------------------------------

Data points can be accessed via the property ``data`` which returns an array object::

  g.data



