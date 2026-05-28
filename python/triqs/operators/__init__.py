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
"""

from .operators import *

__all__ = ['Operator', 'c', 'c_dag', 'n', 'dagger']

