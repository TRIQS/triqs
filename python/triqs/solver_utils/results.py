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

from __future__ import annotations
from dataclasses import dataclass
from typing import Any

import numpy as np

from triqs.gfs import BlockGf


@dataclass
class SolverResults:
    """Container for quantum impurity solver results.

    All fields are optional to support both raw output (postprocess=None)
    and post-processed results with computed self-energies.

    Attributes
    ----------
    G_iw : BlockGf, optional
        Green's function in Matsubara frequencies.
    G_tau : BlockGf, optional
        Green's function in imaginary time.
    G_l : BlockGf, optional
        Green's function in Legendre basis.
    G_tau_dlr : BlockGf, optional
        Green's function in DLR imaginary time.
    Sigma_iw : BlockGf, optional
        Self-energy in Matsubara frequencies (full: dynamic + Hartree-Fock).
    Sigma_dynamic : BlockGf, optional
        Dynamic part of self-energy (Sigma - Sigma_HF).
    Sigma_dlr : BlockGf, optional
        Self-energy in DLR representation.
    Sigma_HartreeFock : list[np.ndarray], optional
        Hartree-Fock (static) contribution to self-energy per block.
    Sigma_moments : dict, optional
        High-frequency moments of self-energy per block.
    density_matrix : Any, optional
        Density matrix from solver (if measured).
    Solver : Any, optional
        Reference to the underlying solver object for advanced access.
    """
    G_iw: BlockGf | None = None
    G_tau: BlockGf | None = None
    G_l: BlockGf | None = None
    G_tau_dlr: BlockGf | None = None
    Sigma_iw: BlockGf | None = None
    Sigma_dynamic: BlockGf | None = None
    Sigma_dlr: BlockGf | None = None
    Sigma_HartreeFock: list[np.ndarray] | None = None
    Sigma_moments: dict | None = None
    density_matrix: Any | None = None
    Solver: Any | None = None

    def __getitem__(self, key: str) -> Any:
        """Allow dict-style access: result['G_iw']."""
        if hasattr(self, key):
            return getattr(self, key)
        raise KeyError(f"SolverResults has no field '{key}'")

    def __contains__(self, key: str) -> bool:
        """Allow 'G_iw' in result checks."""
        return hasattr(self, key) and getattr(self, key) is not None

    def keys(self):
        """Return names of non-None fields."""
        return [f for f in self.__dataclass_fields__ if getattr(self, f) is not None]

    def __repr__(self) -> str:
        fields = [f for f in self.__dataclass_fields__ if getattr(self, f) is not None]
        return f"SolverResults({', '.join(fields)})"
