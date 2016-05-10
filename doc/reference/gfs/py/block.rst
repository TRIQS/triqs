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
--------  

Just like numpy arrays, the Green's function can be sliced, *when the indices are integers* (otherwise it is meaningless).
The syntax is the regular python/numpy syntax, so a simple example will be enough here::

  >>> from pytriqs.gf.local import *
  >>> g = GfImFreq(indices = [1,2,3], beta = 50, n_points = 1000, name = "imp")
  >>> g[1:3:,1:3]
  gf_view

  >>> g[1,1]
  gf_view

  >>> g[2:3,2:3]
  gf_view


Assignment: << or = operator
--------------------------------------------

Because python always uses references, one cannot simply use the = operator
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
   reassigning the reference.

   Let us illustrate this issue on a simple example::

    from pytriqs.gf.local import *
    # Create the Matsubara-frequency Green's function 
    g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")

    g    << inverse( Omega + 0.5 )   # correct 
    g[1,1] = inverse( Omega + 0.5 )   # correct (it uses __setitem__).


   However, the following line is almost certainly NOT what you have in mind::

    g = inverse( Omega + 0.5 )  

   * The reference g is reassigned to the object `inverse( Omega + 0.5 )`, which is not a block Green's function but a lazy expression. 
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

     from pytriqs.utility import mpi
     mpi.send (g, destination)

.. warning::
   Shelve is not a portable format, it may change from python version to another (and it actually does).
   For portability, we recommend using the HDF5 interface for storing data on disks.


Plot options
------------

There is one important option that you will find very useful when plotting Green's functions, which we 
saw already in the previous section:

* `RI` = 'R' or 'I' or 'S'

It tells the plotter, what part of the Green's function you want to plot. 'R' for the real part, 'I'
for the imaginary part, and 'S' for the spectral function, :math:`-1/\pi{\rm Im}G`. Of course, 
depending on the type of Green's function under consideration, one or more of these options do not make a lot of sense, e.g. 
calculating the spectral function of an imaginary-time Green's function is not useful.


Direct access to data points and tails [not for the Legendre version]
-----------------------------------------------------------------------------

Data points can be accessed via the properties ``data`` and ``tail`` respectively.
``data`` returns an array object and so does ``tail[i]``::

  g.data

.. warning::
  
  Be careful when manipulating data directly to keep consistency between
  the function and the tail. 
  Basic operations do this automatically, so use them as much as possible.


.. _greentails:

Direct access to the tails
--------------------------

All block Green's function come together with a **Tail** object that describes its
large-frequency behavior. In other words, for large :math:`|z|`, the Green's function
behaves like

.. math::

  g(z) \sim ... + M_{-1} z + M_0 + \frac{M_1}{z} + \frac{M_2}{z^2} + ...

where :math:`M_i` are matrices with the same dimensions as :math:`g`. 

* Tails can be accessed with the ``tail`` property. Moreover, in order
  to have access to :math:`M_i`, one uses the bracket. For example::

   >>> g = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 1000, name = "egBlock")
   >>> g << 2.0
   gf_view

   >>> print g.tail[0]
   [[ 2.+0.j  0.+0.j]
    [ 0.+0.j  2.+0.j]]

  Here ``g.tail[0]`` is a diagonal matrix with 2 on the diagonal, corresponding to :math:`M_0`.

* Some operations (sum over frequencies, Fourier) uses these tails to regulate the sum, 
  so it is necessary to always keep the consistency between the array of data and the tail expansion.

* Fortunately, in all basic operations on the blocks, these tails are computed automatically.
  For example, when adding two Green functions, the tails are added, and so on.

* However, if you modify the ``data`` or the ``tail`` manually, you lose this guarantee.
  So you have to set the tail properly yourself (or be sure that you will not need it later).
  For example::

   g = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 1000, name = "egBlock")
   g << Function(lambda x: 3/x)
   g.tail.zero()
   g.tail[1] = numpy.array( [[3.0,0.0], [0.0,3.0]] )

  The third line sets all the :math:`M_i` to zero, while the fourth puts :math:`M_1 = diag(3)`. With
  the tail set correctly, this Green's function can be used safely. 
  
.. warning::
  The library will not be able detect tails that are incorrectly set.
  Calculations *may* be wrong in this case.

