r"""Experimental TRIQS features.

.. warning::

   This is a staging area for new, **unstable** functionality. Everything
   exposed here is experimental: the API may change or be removed without
   notice and without a deprecation cycle.

The package currently groups its features into two submodules:

- :mod:`~triqs.experimental.lattice` -- lattice tools (tight-binding
  Hamiltonians, Brillouin-zone integration, local Green's functions and
  Wannier90 loaders).
- :mod:`~triqs.experimental.utility` -- generic numerical helpers (root
  finding and adaptive integration).
"""

from . import lattice
from . import utility

__all__ = ["lattice", "utility"]
