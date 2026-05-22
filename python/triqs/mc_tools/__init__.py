# Copyright (c) 2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014 Centre national de la recherche scientifique (CNRS)
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
# Authors: Olivier Parcollet, Nils Wentzell


r"""
Monte Carlo helpers from the TRIQS MC tools framework.

This module exposes the random number generator utilities used by
Monte Carlo simulations in TRIQS. The MC simulation driver and the
move / measurement abstractions are not available from Python.

It is imported with the command::

  >>> from triqs.mc_tools import RandomGenerator, random_generator_names_list
"""

from .random_generator import RandomGenerator, random_generator_names_list, random_generator_names

__all__ = ['RandomGenerator', 'random_generator_names_list', 'random_generator_names']

