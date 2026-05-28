# Copyright (c) 2015-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2022 Simons Foundation
# Copyright (c) 2015-2017 Igor Krivenko
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
# Authors: Michel Ferrero, Igor Krivenko, Nils Wentzell

"""Operators for commonly used observables."""

import numpy as np
from triqs.operators import Operator, n, c_dag, c
from triqs.operators.util.op_struct import get_mkind
from triqs.operators.util.U_matrix import spherical_to_cubic
from itertools import product

pauli_matrix = {'x' : np.array([[0,1],[1,0]]),
                'y' : np.array([[0,-1j],[1j,0]]),
                'z' : np.array([[1,0],[0,-1]]),
                '+' : np.array([[0,2],[0,0]]),
                '-' : np.array([[0,0],[2,0]])}

def check_backward_compat(fname, n_orb, orb_names):
    """Validate and normalize the orbital count for backward compatibility.

    Raises if the removed ``orb_names`` argument is supplied, and converts a
    list passed in place of ``n_orb`` into its length while emitting a
    ``UserWarning``.

    Parameters
    ----------
    fname : str
        Name of the calling function, used in the warning message.
    n_orb : int or list
        Number of orbitals. A list is accepted for backward compatibility and
        replaced by its length.
    orb_names : object
        Removed argument retained for error reporting. Must be ``None``;
        otherwise a ``RuntimeError`` is raised.

    Returns
    -------
    int
        The (normalized) number of orbitals.

    Examples
    --------
    >>> check_backward_compat('N_op', 5, None)
    5
    >>> check_backward_compat('N_op', ['s', 'p_x', 'p_y'], None)  # doctest: +SKIP
    3
    """
    if orb_names is not None:
        raise RuntimeError("Argument orb_names is no longer supported. Please provide n_orb instead.")
    if isinstance(n_orb, list):
        import warnings
        warnings.warn("Positional argument n_orb of function {} should provide the number of orbitals, not a list".format(fname))
        n_orb = len(n_orb)
    return n_orb

def N_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, orb_names = None):
    r"""Create the total particle-number operator.

    .. math:: \hat{N} = \sum_{i\sigma} \hat{c}_{i\sigma}^\dagger \hat{c}_{i\sigma}.

    Parameters
    ----------
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool, optional
        If ``True``, operators and blocks are labelled by ``('spin', 'orbital')``;
        otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict, optional
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks use the image of ``('spin', 'orbital')``
        under this map.

    Returns
    -------
    Operator
        The total particle-number operator :math:`\hat{N}`.
    """
    n_orb = check_backward_compat("N_op",n_orb,orb_names)
    mkind = get_mkind(off_diag,map_operator_structure)
    N = Operator()
    for sn, o in product(spin_names,range(n_orb)): N += n(*mkind(sn,o))
    return N

def S_op(component, spin_names, n_orb, off_diag = None, map_operator_structure = None, orb_names = None):
    r"""Create a component of the spin vector operator.

    .. math::
        \hat{S}_{x,y,z} = \frac{1}{2}\sum_{i\sigma\sigma'} \hat{c}^\dagger_{i\sigma} \mathbf{\tau}^{x,y,z}_{\sigma\sigma'} \hat{c}_{i\sigma'},
        \quad\hat{S}_\pm = \hat{S}_x \pm i \hat{S}_y.

    Parameters
    ----------
    component : str
        Component to be created, one of ``'x'``, ``'y'``, ``'z'``, ``'+'`` or
        ``'-'``.
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool, optional
        If ``True``, operators and blocks are labelled by ``('spin', 'orbital')``;
        otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict, optional
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks use the image of ``('spin', 'orbital')``
        under this map.

    Returns
    -------
    Operator
        The requested component of the spin vector operator.
    """
    n_orb = check_backward_compat("S_op",n_orb,orb_names)
    mkind  = get_mkind(off_diag,map_operator_structure)
    pm = pauli_matrix[component]

    S = Operator()
    spin_range = list(range(len(spin_names)))
    for n1, n2 in product(spin_range,spin_range):
        for o in range(n_orb):
            S += 0.5 * c_dag(*mkind(spin_names[n1],o)) * pm[n1,n2] * c(*mkind(spin_names[n2],o))
    return S

def S2_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, orb_names = None):
    r"""Create the square of the total spin operator.

    .. math:: \hat{S}^2 = \hat{S}_x^2 + \hat{S}_y^2 + \hat{S}_z^2.

    Parameters
    ----------
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool, optional
        If ``True``, operators and blocks are labelled by ``('spin', 'orbital')``;
        otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict, optional
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks use the image of ``('spin', 'orbital')``
        under this map.

    Returns
    -------
    Operator
        The square of the total spin operator :math:`\hat{S}^2`.
    """
    n_orb = check_backward_compat("S2_op",n_orb,orb_names)
    Sz, Sp, Sm = [S_op(k,spin_names,n_orb,off_diag,map_operator_structure) for k in ('z','+','-')]
    return Sz*Sz + 0.5*(Sp*Sm + Sm*Sp)

def L_op(component, spin_names, n_orb, off_diag = None, map_operator_structure = None, basis='spherical', T=None, orb_names = None):
    r"""Create a component of the orbital angular-momentum vector operator.

    .. math::
        \hat{L}_{z,+,-} &= \sum_{ii'\sigma} \hat{c}^\dagger_{i\sigma} L^{z,+,-}_{ii'} \hat{c}_{i'\sigma},\\
        \hat{L}_x &= \frac{1}{2}(\hat{L}_+ + \hat{L}_-),\ \hat{L}_y = \frac{1}{2i}(\hat{L}_+ - \hat{L}_-),\\
        L^z_{ii'} &= i\delta_{i,i'}, \
        L^+_{ii'} = \delta_{i,i'+1}\sqrt{l(l+1)-i'(i'+1)}, \
        L^+_{ii'} = \delta_{i,i'-1}\sqrt{l(l+1)-i'(i'-1)}.

    Parameters
    ----------
    component : str
        Component to be created, one of ``'x'``, ``'y'``, ``'z'``, ``'+'`` or
        ``'-'``.
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool, optional
        If ``True``, operators and blocks are labelled by ``('spin', 'orbital')``;
        otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict, optional
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks use the image of ``('spin', 'orbital')``
        under this map.
    basis : str, optional
        Basis in which the interaction matrix is computed. One of:

        - ``'spherical'``: spherical harmonics (default),
        - ``'cubic'``: cubic harmonics (valid only for integer orbital momenta,
          i.e. odd ``n_orb``),
        - ``'other'``: arbitrary basis specified by the transformation matrix
          :math:`T`.
    T : numpy.ndarray, optional
        Transformation matrix for the basis change. Required when
        ``basis='other'``.

    Returns
    -------
    Operator
        The requested component of the orbital angular-momentum vector
        operator.
    """
    n_orb = check_backward_compat("L_op",n_orb,orb_names)
    l = (n_orb-1)/2.0
    L_melem_dict = {'z' : lambda m,mp: m if np.isclose(m,mp) else 0,
                    '+' : lambda m,mp: np.sqrt(l*(l+1)-mp*(mp+1)) if np.isclose(m,mp+1) else 0,
                    '-' : lambda m,mp: np.sqrt(l*(l+1)-mp*(mp-1)) if np.isclose(m,mp-1) else 0,
                    'x' : lambda m,mp: 0.5*(L_melem_dict['+'](m,mp) + L_melem_dict['-'](m,mp)),
                    'y' : lambda m,mp: -0.5j*(L_melem_dict['+'](m,mp) - L_melem_dict['-'](m,mp))}
    L_melem = L_melem_dict[component]
    orb_range = list(range(int(2*l+1)))
    L_matrix = np.array([[L_melem(o1-l,o2-l) for o2 in orb_range] for o1 in orb_range])

    # Transform from spherical basis if needed
    if basis == "cubic":
        if not np.isclose(np.mod(l,1),0):
            raise ValueError("L_op: cubic basis is only defined for the integer orbital momenta.")
        T = spherical_to_cubic(int(l))
    if basis == "other" and T is None: raise ValueError("L_op: provide T for other bases.")
    if T is not None: L_matrix = np.einsum("ij,jk,kl",np.conj(T),L_matrix,np.transpose(T))

    mkind = get_mkind(off_diag,map_operator_structure)
    L = Operator()
    for sn in spin_names:
        for o1, o2 in product(orb_range,orb_range):
            L += c_dag(*mkind(sn,o1)) * L_matrix[o1,o2] * c(*mkind(sn,o2))
    return L

def L2_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, basis='spherical', T=None, orb_names = None):
    r"""Create the square of the orbital angular-momentum operator.

    .. math:: \hat{L}^2 = \hat{L}_x^2 + \hat{L}_y^2 + \hat{L}_z^2.

    Parameters
    ----------
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool, optional
        If ``True``, operators and blocks are labelled by ``('spin', 'orbital')``;
        otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict, optional
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks use the image of ``('spin', 'orbital')``
        under this map.
    basis : str, optional
        Basis in which the interaction matrix is computed. One of:

        - ``'spherical'``: spherical harmonics (default),
        - ``'cubic'``: cubic harmonics (valid only for integer orbital momenta,
          i.e. odd ``n_orb``),
        - ``'other'``: arbitrary basis specified by the transformation matrix
          :math:`T`.
    T : numpy.ndarray, optional
        Transformation matrix for the basis change. Required when
        ``basis='other'``.

    Returns
    -------
    Operator
        The square of the orbital angular-momentum operator :math:`\hat{L}^2`.
    """
    n_orb = check_backward_compat("L2_op",n_orb,orb_names)
    Lz, Lp, Lm = [L_op(k,spin_names,n_orb,off_diag, map_operator_structure, basis, T) for k in ('z','+','-')]
    return Lz*Lz + 0.5*(Lp*Lm + Lm*Lp)

def LS_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, basis='spherical', T=None, orb_names = None):
    r"""Create the spin-orbital coupling operator.

    .. math:: \mathbf{\hat{L}\cdot\hat{S}} = \hat{L}_x \hat{S}_x  + \hat{L}_y \hat{S}_y + \hat{L}_z \hat{S}_z.

    Parameters
    ----------
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool, optional
        If ``True``, operators and blocks are labelled by ``('spin', 'orbital')``;
        otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict, optional
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks use the image of ``('spin', 'orbital')``
        under this map.
    basis : str, optional
        Basis in which the interaction matrix is computed. One of:

        - ``'spherical'``: spherical harmonics (default),
        - ``'cubic'``: cubic harmonics (valid only for integer orbital momenta,
          i.e. odd ``n_orb``),
        - ``'other'``: arbitrary basis specified by the transformation matrix
          :math:`T`.
    T : numpy.ndarray, optional
        Transformation matrix for the basis change. Required when
        ``basis='other'``.

    Returns
    -------
    Operator
        The spin-orbital coupling operator
        :math:`\mathbf{\hat{L}\cdot\hat{S}}`.
    """
    n_orb = check_backward_compat("LS_op",n_orb,orb_names)
    Sz, Sp, Sm = [S_op(k,spin_names,n_orb,off_diag,map_operator_structure) for k in ('z','+','-')]
    Lz, Lp, Lm = [L_op(k,spin_names,n_orb,off_diag,map_operator_structure, basis, T) for k in ('z','+','-')]
    return Lz*Sz + 0.5*(Lp*Sm + Lm*Sp)
