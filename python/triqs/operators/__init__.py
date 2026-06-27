# Copyright (c) 2013-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Michel Ferrero, Olivier Parcollet, Priyanka Seth, Nils Wentzell

"""Second-quantization operators and many-body operator algebra.

Provides the :class:`Operator` class together with the canonical factories
:func:`c` (annihilation), :func:`c_dag` (creation) and :func:`n` (number
operator), as well as the Hermitian-conjugate function :func:`dagger`. Higher-
level helpers — model Hamiltonians, observables, interaction tensors and
coefficient extractors — live in :mod:`triqs.operators.util`.

Examples
--------
Build many-body operators from the canonical factories and combine them with
the usual algebra (sums, products, multiplication by a scalar). The factory
arguments are arbitrary indices, e.g. a spin name and an orbital index:

>>> from triqs.operators import c, c_dag, n, dagger

A single-orbital Hubbard interaction (two spins):

>>> U = 4.0
>>> H = U * n('up', 0) * n('dn', 0)

Nearest-neighbour hopping on a two-site spinless chain:

>>> t = 1.0
>>> H = -t * (c_dag('s', 0) * c('s', 1) + c_dag('s', 1) * c('s', 0))

The number operator is ``n(*idx) == c_dag(*idx) * c(*idx)``, so the following
operator is identically zero:

>>> (n('up', 0) - c_dag('up', 0) * c('up', 0)).is_zero()
True

Operators can be Hermitian-conjugated with :func:`dagger`:

>>> op = c_dag('up', 0) * c('dn', 1)
>>> H = op + dagger(op)              # a Hermitian hopping term
"""

from .operators import *

__all__ = ['Operator', 'c', 'c_dag', 'n', 'dagger']

