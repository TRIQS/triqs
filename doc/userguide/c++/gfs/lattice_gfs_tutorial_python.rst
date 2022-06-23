
Lattice Green's functions
======================================

.. warning::
   This part of the library is largely experimental and subject to API breaks


This notebook shows a basic example of manipulation of
:math:`G(\mathbf{k},\omega)` defined on a 2D Brillouin zone.

.. literalinclude:: lattice_gfs.py
   :lines: 1-7


Defining the meshes
-------------------

Here we define a Bravais lattice object corresponding to a
two-dimensional square lattice, as well as the corresponding Brillouin
zone and a mesh defined on it.

.. literalinclude:: lattice_gfs.py
   :lines: 9-16


Definition of the Green's function
----------------------------------

Here we define the Green's function as

.. math:: G(\mathbf{k},i\omega) = \frac{1}{\omega+i\eta - \varepsilon(\mathbf{k}) - \Sigma(\omega)}

where
:math:`\varepsilon(\mathbf{k}) = -2t\left(\cos(k_x)+\cos(k_y)\right)`
and :math:`\Sigma(\omega)` is the atomic-limit self-energy:

.. math:: \Sigma(\omega) = \frac{U^2}{4\omega}

.. literalinclude:: lattice_gfs.py
   :lines: 18-31

    


Various plots
-------------

We plot various slices of :math:`G(\mathbf{k},\omega)` corresponding to
:math:`U=0` and :math:`U=4`.


.. literalinclude:: lattice_gfs.py
   :lines: 34-72



.. plot:: userguide/gfs/lattice_gfs.py
   :scale: 70

