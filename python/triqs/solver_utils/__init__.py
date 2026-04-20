# Copyright (c) 2024-2026 Simons Foundation
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
#     https://www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

"""
Shared utilities for quantum impurity solvers.

Provides:
- SolverResults: Unified container for solver output
- tail_fit: High-frequency tail fitting for self-energy BlockGf
- make_gf_dlr_imfreq: Convert ImFreq Green's function to DLR ImFreq representation
"""

from .results import SolverResults
from .tail_fit import tail_fit
from .gf_utils import make_gf_dlr_imfreq

__all__ = ['SolverResults', 'tail_fit', 'make_gf_dlr_imfreq']
