# Copyright (c) 2015-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
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
# Authors: Michel Ferrero, Alexander Hampel, Igor Krivenko, Harry LaBollita, Michael, Priyanka Seth, Nils Wentzell

"""Functions to construct Coulomb tensors."""

from math import sqrt
from scipy.special import factorial as fact
from itertools import product
import numpy as np


def U_matrix_slater(l, radial_integrals=None, U_int=None, J_hund=None, basis='spherical', T=None):
    r"""Calculate the full four-index Slater interaction tensor.

    .. math:: U^{spherical}_{m1 m2 m3 m4} = \sum_{k=0}^{2l} F_k \alpha(l, k, m1, m2, m3, m4)

    where :math:`F_k` (i.e. :math:`F_0, F_2, F_4, ...`) are radial Slater
    integrals and :math:`\alpha(l, k, m1, m2, m3, m4)` are the angular
    Racah-Wigner coefficients for a spherically symmetric interaction tensor.
    Either pass the radial integrals :math:`F_k` directly, or pass ``U_int``
    and ``J_hund`` — they are then converted via
    :func:`U_J_to_radial_integrals`.

    The convention for the :math:`U`-matrix is fixed by the Hamiltonian

    .. math:: \hat{H} = \frac{1}{2} \sum_{ijkl,\sigma \sigma'} U_{ijkl} \hat{c}_{i \sigma}^\dagger \hat{c}_{j \sigma'}^\dagger \hat{c}_{l \sigma'} \hat{c}_{k \sigma}.

    Parameters
    ----------
    l : int
        Angular momentum of the shell being treated (``l=2`` for a ``d`` shell,
        ``l=3`` for an ``f`` shell).
    radial_integrals : list of float, optional
        Slater integrals ``[F0, F2, F4, ...]``. Must be provided if both
        ``U_int`` and ``J_hund`` are missing. Used in preference to
        ``U_int``/``J_hund`` if all three are supplied.
    U_int : float, optional
        Value of the screened Hubbard interaction. Must be provided if
        ``radial_integrals`` is not given.
    J_hund : float, optional
        Value of the Hund's coupling. Must be provided if ``radial_integrals``
        is not given.
    basis : {'spherical', 'cubic', 'other'}, optional
        Basis in which the interaction matrix is computed: spherical harmonics
        (default), cubic harmonics, or an arbitrary basis specified by the
        transformation matrix :math:`T`.
    T : ndarray, optional
        Transformation matrix for the basis change. Required when
        ``basis='other'``; see Notes for the convention.

    Returns
    -------
    numpy.ndarray
        The four-index interaction matrix in the chosen basis.

    Notes
    -----
    The transformation matrix :math:`T` is defined so that new creation
    operators :math:`\hat{b}^\dagger` are related to the old ones
    :math:`\hat{c}^\dagger` via

    .. math:: \hat{b}_{i \sigma}^\dagger = \sum_j T_{ij} \hat{c}^\dagger_{j \sigma}.
    """

    # Check all necessary information is present and consistent
    if radial_integrals is None and (U_int is None and J_hund is None):
        raise ValueError("U_matrix: provide either the radial_integrals or U_int and J_hund.")
    if radial_integrals is None and (U_int is not None and J_hund is not None):
        radial_integrals = U_J_to_radial_integrals(l, U_int, J_hund)
    if radial_integrals is not None and (U_int is not None and J_hund is not None):
        if len(radial_integrals)-1 != l:
            raise ValueError("U_matrix: inconsistency in l and number of radial_integrals provided.")
        if (radial_integrals - U_J_to_radial_integrals(l, U_int, J_hund)).any() != 0.0:
            print("Warning: U_matrix: radial_integrals provided do not match U_int and J_hund. Using radial_integrals to calculate U_matrix.")

    # Full interaction matrix
    # Basis of spherical harmonics Y_{-2}, Y_{-1}, Y_{0}, Y_{1}, Y_{2}
    # U^{spherical}_{m1 m2 m3 m4} = \sum_{k=0}^{2l} F_k \alpha(l, k, m1, m2, m3, m4)
    U_matrix = np.zeros((2*l+1,2*l+1,2*l+1,2*l+1),dtype=float)

    m_range = list(range(-l,l+1))
    for n, F in enumerate(radial_integrals):
        k = 2*n
        for m1, m2, m3, m4 in product(m_range,m_range,m_range,m_range):
            U_matrix[m1+l,m2+l,m3+l,m4+l] += F * angular_matrix_element(l,k,m1,m2,m3,m4)

    # Transform from spherical basis if needed
    if basis == "cubic": T = spherical_to_cubic(l)
    if basis == "other" and T is None:
        raise ValueError("U_matrix: provide T for other bases.")
    if T is not None: U_matrix = transform_U_matrix(U_matrix, T)

    return U_matrix


def reduce_4index_to_2index(U_4index):
    """Reduce the four-index U matrix to two-index matrices for parallel and anti-parallel spins.

    Parameters
    ----------
    U_4index : numpy.ndarray
        The four-index interaction matrix.

    Returns
    -------
    U : numpy.ndarray
        The two-index interaction matrix for parallel spins.
    Uprime : numpy.ndarray
        The two-index interaction matrix for anti-parallel spins.
    """

    size = len(U_4index) # 2l+1
    U  = np.zeros((size,size),dtype=float)      # matrix for same spin
    Uprime = np.zeros((size,size),dtype=float)  # matrix for opposite spin

    m_range = list(range(size))
    for m,mp in product(m_range,m_range):
        U[m,mp]  = U_4index[m,mp,m,mp].real - U_4index[m,mp,mp,m].real
        Uprime[m,mp] = U_4index[m,mp,m,mp].real

    return U, Uprime

def U_matrix_kanamori(n_orb, U_int, J_hund, Up_int=None, full_Uijkl=False, Jc_hund=None):
    r"""Calculate the Kanamori interaction matrix (or full four-index tensor).

    The two-index matrix for parallel spins is

    .. math:: U_{m m'}^{\sigma \sigma} \equiv U_{m m' m m'} - J_{m m'}

    with

    .. math:: J_{m m'} \equiv U_{m m' m' m},

    and the two-index matrix for anti-parallel spins is

    .. math:: U_{m m'}^{\sigma \bar{\sigma}} \equiv U_{m m' m m'}.

    If ``full_Uijkl=True``, the full four-index tensor is returned instead:

    .. math:: U_{m m m m} = U, \\
              U_{m m' m m'} = U', \\
              U_{m m' m' m} = J, \\
              U_{m m m' m'} = J_C,

    with :math:`m \neq m'`.

    Parameters
    ----------
    n_orb : int
        Number of orbitals in the basis.
    U_int : float
        Value of the screened Hubbard interaction.
    J_hund : float
        Value of the Hund's coupling.
    Up_int : float, optional
        Value of the screened :math:`U'` parameter. Defaults to
        ``U_int - 2 * J_hund`` (fully rotationally-invariant form).
    full_Uijkl : bool, optional
        If ``True``, return the full four-index :math:`U_{ijkl}` tensor instead
        of the two-index matrices. Default ``False``.
    Jc_hund : float, optional
        Used only when ``full_Uijkl=True``. Defaults to ``J_hund``.

    Returns
    -------
    U : numpy.ndarray
        Two-index interaction matrix for parallel spins (or the full four-index
        :math:`U_{ijkl}` tensor when ``full_Uijkl=True``).
    Uprime : numpy.ndarray
        Two-index interaction matrix for anti-parallel spins. Only returned
        when ``full_Uijkl=False``.
    """

    # Jc_hund can only be used if the full tensor is returned
    if Jc_hund is not None and not full_Uijkl:
        raise ValueError('Jc_hund can only be specified if the full four index tensor is returned')

    if Up_int is None:
        Up_int = U_int-2*J_hund
    if Jc_hund is None:
        Jc_hund = J_hund

    m_range = range(n_orb)

    if not full_Uijkl:
        U = np.zeros((n_orb, n_orb), dtype=float)      # matrix for same spin
        Uprime = np.zeros((n_orb, n_orb), dtype=float)  # matrix for opposite spin

        for m, mp in product(m_range, m_range):
            if m == mp:
                Uprime[m, mp] = U_int
            else:
                U[m, mp] = Up_int - J_hund
                Uprime[m, mp] = Up_int

        return U, Uprime
    else:
        U_kan = np.zeros((n_orb, n_orb, n_orb, n_orb))

        for i, j, k, l in product(m_range, m_range, m_range, m_range):
            if i == j == k == l:  # Uiiii
                U_kan[i, j, k, l] = U_int
            elif i == k and j == l:  # Uijij
                U_kan[i, j, k, l] = Up_int
            elif i == l and j == k:  # Uijji
                U_kan[i, j, k, l] = J_hund
            elif i == j and k == l:  # Uiijj
                U_kan[i, j, k, l] = Jc_hund
        return U_kan

def t2g_submatrix(U, convention='triqs'):
    """Extract the ``t2g`` submatrix of the full ``d``-manifold two- or four-index U matrix.

    Parameters
    ----------
    U : numpy.ndarray
        Two- or four-index interaction matrix in the full ``d``-manifold.
    convention : str, optional
        Basis convention. One of:

        - ``'triqs'``: basis ordered as ``("xy", "yz", "z^2", "xz", "x^2-y^2")`` (default),
        - ``'vasp'``: same as ``'triqs'``,
        - ``'wien2k'``: basis ordered as ``("z^2", "x^2-y^2", "xy", "yz", "xz")``,
        - ``'wannier90'``: basis ordered as ``("z^2", "xz", "yz", "x^2-y^2", "xy")``,
        - ``'qe'``: same as ``'wannier90'``.

    Returns
    -------
    numpy.ndarray
        The ``t2g`` component of the interaction matrix.
    """
    if convention == 'wien2k':
        return subarray(U, len(U.shape)*[(2,3,4)])
    elif convention == 'wannier90' or convention == 'qe':
        return subarray(U, len(U.shape)*[(1,2,4)])
    elif convention == 'triqs' or convention=='vasp':
        return subarray(U, len(U.shape)*[(0,1,3)])
    else:
        raise ValueError("Unknown convention: "+str(convention))

def eg_submatrix(U, convention='triqs'):
    """Extract the ``eg`` submatrix of the full ``d``-manifold two- or four-index U matrix.

    Parameters
    ----------
    U : numpy.ndarray
        Two- or four-index interaction matrix in the full ``d``-manifold.
    convention : str, optional
        Basis convention. One of:

        - ``'triqs'``: basis ordered as ``("xy", "yz", "z^2", "xz", "x^2-y^2")`` (default),
        - ``'vasp'``: same as ``'triqs'``,
        - ``'wien2k'``: basis ordered as ``("z^2", "x^2-y^2", "xy", "yz", "xz")``,
        - ``'wannier90'``: basis ordered as ``("z^2", "xz", "yz", "x^2-y^2", "xy")``,
        - ``'qe'``: same as ``'wannier90'``.

    Returns
    -------
    numpy.ndarray
        The ``eg`` component of the interaction matrix.
    """
    if convention == 'wien2k':
        return subarray(U, len(U.shape)*[(0,1)])
    elif convention == 'wannier90' or convention == 'qe':
        return subarray(U, len(U.shape)*[(0,3)])
    elif convention == 'triqs' or convention=='vasp':
        return subarray(U, len(U.shape)*[(2,4)])

    else:
        raise ValueError("Unknown convention: "+str(convention))


def transform_U_matrix(U_matrix, T):
    r"""Transform a four-index interaction matrix into another basis.

    The transformation matrix :math:`T` is defined such that new creation
    operators :math:`\hat{b}^\dagger` are related to the old ones
    :math:`\hat{c}^\dagger` as

    .. math:: \hat{b}_{i \sigma}^\dagger = \sum_j T_{ij} \hat{c}^\dagger_{j \sigma}.

    Parameters
    ----------
    U_matrix : numpy.ndarray
        The four-index interaction matrix in the original basis.
    T : numpy.ndarray
        Transformation matrix for the basis change.

    Returns
    -------
    numpy.ndarray
        The four-index interaction matrix in the new basis.
    """
    return np.einsum("ij,kl,jlmo,mn,op",np.conj(T),np.conj(T),U_matrix,np.transpose(T),np.transpose(T))

def spherical_to_cubic(l, convention='triqs'):
    """Get the spherical-to-cubic harmonics transformation matrix.

    Parameters
    ----------
    l : integer
        Angular momentum of shell being treated (l=2 for d shell, l=3 for f shell).
    convention : string, optional
                 The basis convention. For l=2 (d shell) the cubic orderings are:

                 - 'triqs': ("xy","yz","z^2","xz","x^2-y^2"),
                 - 'vasp': same as 'triqs',
                 - 'wien2k': ("z^2","x^2-y^2","xy","yz","xz"),
                 - 'wannier90': ("z^2","xz","yz","x^2-y^2","xy"),
                 - 'qe': same as 'wannier90'.

                 For l=3 (f shell) the cubic orderings are:

                 - 'triqs': ("x(x^2-3y^2)","z(x^2-y^2)","xz^2","z^3","yz^2","xyz","y(3x^2-y^2)"),
                 - 'vasp': ("y(3x^2-y^2)","xyz","yz^2","z^3","xz^2","z(x^2-y^2)","x(x^2-3y^2)"),
                 - 'wannier90': ("z^3","xz^2","yz^2","z(x^2-y^2)","xyz","x(x^2-3y^2)","y(3x^2-y^2)"),
                 - 'qe': same as 'wannier90',
                 - 'wien2k': not supported for l=3 (dmftproj uses site-symmetry-specific
                   transformations; see dmftproj/SRC_templates/case.cf_f_mm2).

    Returns
    -------
    numpy.ndarray
        The spherical-to-cubic harmonics transformation matrix.
    """

    if not convention in ('wien2k','wannier90', 'triqs', 'vasp', 'qe'):
        raise ValueError("Unknown convention: "+str(convention))

    size = 2*l+1
    T = np.zeros((size,size),dtype=complex)
    if convention == 'wien2k' and l == 3:
        raise ValueError("spherical_to_cubic: 'wien2k' convention not implemented for l=3 "
                         "(dmftproj uses point-group-specific transformations; see "
                         "dmftproj/SRC_templates/case.cf_f_mm2).")
    if l == 0:
        cubic_names = ("s")
        T[0,0] = 1.0
    elif l == 1:
        if convention == 'wannier90' or convention == 'qe':
            cubic_names = ("z","x","y")
            T[1,0] = 1.0/sqrt(2);   T[1,2] = -1.0/sqrt(2)
            T[2,0] = 1j/sqrt(2);    T[2,2] = 1j/sqrt(2)
            T[0,1] = 1.0
        elif convention == 'triqs' or convention == 'vasp' or convention == 'wien2k':
            cubic_names = ("x","y","z")
            T[0,0] = 1.0/sqrt(2);   T[0,2] = -1.0/sqrt(2)
            T[1,0] = 1j/sqrt(2);    T[1,2] = 1j/sqrt(2)
            T[2,1] = 1.0
    elif l == 2:
        if convention == 'wien2k':
            # projectors created in Wien2k are defined in the complex spherical
            # basis (see dmftproj manual Sec. 1.3.3).
            cubic_names = ("z^2","x^2-y^2","xy","yz","xz")
            T[0,2] = 1.0
            T[1,0] = 1.0/sqrt(2);   T[1,4] = 1.0/sqrt(2)
            T[2,0] =-1.0/sqrt(2);   T[2,4] = 1.0/sqrt(2)
            T[3,1] = 1.0/sqrt(2);   T[3,3] =-1.0/sqrt(2)
            T[4,1] = 1.0/sqrt(2);   T[4,3] = 1.0/sqrt(2)
        elif convention == 'wannier90' or convention == 'qe':
            cubic_names = ("z^2", "xz", "yz", "x^2-y^2", "xy")
            T[0,2] = 1.0;           T[1,1] = 1.0/sqrt(2);
            T[1,3] =-1.0/sqrt(2);   T[2,1] = 1j/sqrt(2);
            T[2,3] = 1j/sqrt(2);    T[3,0] = 1.0/sqrt(2);
            T[3,4] = 1.0/sqrt(2);   T[4,0] = 1j/sqrt(2);
            T[4,4] = -1j/sqrt(2);
        elif convention == 'triqs' or convention == 'vasp':
            cubic_names = ("xy","yz","z^2","xz","x^2-y^2")
            T[0,0] = 1j/sqrt(2);    T[0,4] = -1j/sqrt(2)
            T[1,1] = 1j/sqrt(2);    T[1,3] = 1j/sqrt(2)
            T[2,2] = 1.0
            T[3,1] = 1.0/sqrt(2);   T[3,3] = -1.0/sqrt(2)
            T[4,0] = 1.0/sqrt(2);   T[4,4] = 1.0/sqrt(2)
    elif l == 3:
        # l=3 cubic orderings used here:
        # triqs:     ("x(x^2-3y^2)", "z(x^2-y^2)", "xz^2", "z^3", "yz^2", "xyz", "y(3x^2-y^2)")
        # vasp:      ("y(3x^2-y^2)", "xyz", "yz^2", "z^3", "xz^2", "z(x^2-y^2)", "x(x^2-3y^2)")
        # wannier90: ("z^3", "xz^2", "yz^2", "z(x^2-y^2)", "xyz", "x(x^2-3y^2)", "y(3x^2-y^2)")
        # qe: same as wannier90
        T_triqs = np.zeros((size,size),dtype=complex)
        T_triqs[0,0] = 1.0/sqrt(2);   T_triqs[0,6] = -1.0/sqrt(2)
        T_triqs[1,1] = 1.0/sqrt(2);   T_triqs[1,5] =  1.0/sqrt(2)
        T_triqs[2,2] = 1.0/sqrt(2);   T_triqs[2,4] = -1.0/sqrt(2)
        T_triqs[3,3] = 1.0
        T_triqs[4,2] = 1j/sqrt(2);    T_triqs[4,4] =  1j/sqrt(2)
        T_triqs[5,1] = 1j/sqrt(2);    T_triqs[5,5] = -1j/sqrt(2)
        T_triqs[6,0] = 1j/sqrt(2);    T_triqs[6,6] =  1j/sqrt(2)

        if convention == 'triqs':
            T = T_triqs
        elif convention == 'vasp':
            row_permutation = (6, 5, 4, 3, 2, 1, 0)
            T = T_triqs[row_permutation, :]
        elif convention in ('wannier90', 'qe'):
            row_permutation = (3, 2, 4, 1, 5, 0, 6)
            T = T_triqs[row_permutation, :]
    else: raise ValueError("spherical_to_cubic: implemented only for l=0,1,2,3")

    return T

def cubic_names(l):
    """Get the names of the cubic harmonics for the given shell.

    Parameters
    ----------
    l : int or str
        Angular momentum of the shell being treated. Also accepts the strings
        ``'s'``, ``'p'``, ``'d'``, ``'f'``, ``'t2g'`` and ``'eg'``.

    Returns
    -------
    tuple of str
        Names of the cubic-harmonic orbitals for the requested shell.
    """
    if l == 0 or l == 's':
        return ("s")
    elif l == 1 or l == 'p':
        return ("x","y","z")
    elif l == 2 or l == 'd':
        return ("xy","yz","z^2","xz","x^2-y^2")
    elif l == 't2g':
        return ("xy","yz","xz")
    elif l == 'eg':
        return ("z^2","x^2-y^2")
    elif l == 3 or l == 'f':
        return ("x(x^2-3y^2)","z(x^2-y^2)","xz^2","z^3","yz^2","xyz","y(3x^2-y^2)")
    else: raise ValueError("cubic_names: implemented only for l=0,1,2,3")

def U_J_to_radial_integrals(l, U_int, J_hund):
    """Compute the radial Slater integrals :math:`F_k` from ``U_int`` and ``J_hund``.

    Parameters
    ----------
    l : int
        Angular momentum of the shell being treated (``l=2`` for a ``d`` shell,
        ``l=3`` for an ``f`` shell).
    U_int : float
        Value of the screened Hubbard interaction.
    J_hund : float
        Value of the Hund's coupling.

    Returns
    -------
    list of float
        Slater integrals ``[F0, F2, F4, ...]``.
    """

    F = np.zeros((l+1),dtype=float)
    if l==1:
        F[0] = U_int
        F[1] = 5.0 * J_hund
    elif l == 2:
        F[0] = U_int
        F[1] = J_hund * 14.0 / (1.0 + 0.63)
        F[2] = 0.630 * F[1]
    elif l == 3:
        F[0] = U_int
        F[1] = 6435.0 * J_hund / (286.0 + 195.0 * 451.0 / 675.0 + 250.0 * 1001.0 / 2025.0)
        F[2] = 451.0 * F[1] / 675.0
        F[3] = 1001.0 * F[1] / 2025.0
    else: raise ValueError("U_J_to_radial_integrals: implemented only for l=1,2,3")

    return F

def radial_integrals_to_U_J(l, F):
    """Compute ``U_int`` and ``J_hund`` from the radial Slater integrals.

    Parameters
    ----------
    l : int
        Angular momentum of the shell being treated (``l=2`` for a ``d`` shell,
        ``l=3`` for an ``f`` shell).
    F : list of float
        Slater integrals ``[F0, F2, F4, ...]``.

    Returns
    -------
    U_int : float
        Value of the screened Hubbard interaction.
    J_hund : float
        Value of the Hund's coupling.
    """
    if l == 1:
        U_int = F[0]
        J_hund = F[1] / 5.0
    elif l == 2:
        U_int = F[0]
        J_hund = F[1] * (1.0 + 0.63) / 14.0
    elif l == 3:
        U_int = F[0]
        J_hund = F[1] * (286.0 + 195.0 * 451.0 / 675.0 + 250.0 * 1001.0 / 2025.0) / 6435.0
    else: raise ValueError("radial_integrals_to_U_J: implemented only for l=1,2,3")

    return U_int,J_hund

# (2l+1)^2 ((l 0) (k 0) (l 0))^2 \sum_{q=-k}^{k} (-1)^{m1+m2+q} ((l -m1) (k q) (l m3)) ((l -m2) (k -q) (l m4))
def angular_matrix_element(l, k, m1, m2, m3, m4):
    r"""Calculate the angular Racah-Wigner matrix element.

    .. math::
       (2l+1)^2
       \begin{pmatrix}
            l & k & l \\
            0 & 0 & 0
       \end{pmatrix}^2
       \sum_{q=-k}^k (-1)^{m_1+m_2+q}
       \begin{pmatrix}
            l & k & l \\
         -m_1 & q & m_3
       \end{pmatrix}
       \begin{pmatrix}
            l & k  & l \\
         -m_2 & -q & m_4
       \end{pmatrix}.

    Parameters
    ----------
    l : int
        Orbital angular momentum of the shell.
    k : int
        Order of the multipole expansion.
    m1, m2, m3, m4 : int
        Magnetic quantum numbers of the four orbitals.

    Returns
    -------
    float
        Value of the angular matrix element.
    """
    ang_mat_ele = 0
    for q in range(-k,k+1):
        ang_mat_ele += three_j_symbol((l,-m1),(k,q),(l,m3))*three_j_symbol((l,-m2),(k,-q),(l,m4))*(-1.0 if (m1+q+m2) % 2 else 1.0)
    ang_mat_ele *= (2*l+1)**2 * (three_j_symbol((l,0),(k,0),(l,0))**2)
    return ang_mat_ele

def three_j_symbol(jm1, jm2, jm3):
    r"""Calculate the Wigner 3-j symbol.

    .. math::
       \begin{pmatrix}
        l_1 & l_2 & l_3\\
        m_1 & m_2 & m_3
       \end{pmatrix}.

    Parameters
    ----------
    jm1 : tuple of int
        :math:`(j_1, m_1)`.
    jm2 : tuple of int
        :math:`(j_2, m_2)`.
    jm3 : tuple of int
        :math:`(j_3, m_3)`.

    Returns
    -------
    float
        Value of the 3-j symbol.
    """
    j1, m1 = jm1
    j2, m2 = jm2
    j3, m3 = jm3

    if (m1+m2+m3 != 0 or
        m1 < -j1 or m1 > j1 or
        m2 < -j2 or m2 > j2 or
        m3 < -j3 or m3 > j3 or
        j3 > j1 + j2 or
        j3 < abs(j1-j2)):
        return .0

    three_j_sym = -1.0 if (j1-j2-m3) % 2 else 1.0
    three_j_sym *= sqrt(fact(j1+j2-j3)*fact(j1-j2+j3)*fact(-j1+j2+j3)/fact(j1+j2+j3+1))
    three_j_sym *= sqrt(fact(j1-m1)*fact(j1+m1)*fact(j2-m2)*fact(j2+m2)*fact(j3-m3)*fact(j3+m3))

    t_min = max(j2-j3-m1,j1-j3+m2,0)
    t_max = min(j1-m1,j2+m2,j1+j2-j3)

    t_sum = 0
    for t in range(t_min,t_max+1):
        t_sum += (-1.0 if t % 2 else 1.0)/(fact(t)*fact(j3-j2+m1+t)*fact(j3-j1-m2+t)*fact(j1+j2-j3-t)*fact(j1-m1-t)*fact(j2+m2-t))

    three_j_sym *= t_sum
    return three_j_sym

def clebsch_gordan(jm1, jm2, jm3):
    r"""Calculate the Clebsch-Gordan coefficient.

    .. math::
       \langle j_1 m_1 j_2 m_2 | j_3 m_3 \rangle = (-1)^{j_1-j_2+m_3} \sqrt{2 j_3 + 1}
       \begin{pmatrix}
         j_1 & j_2 & j_3\\
         m_1 & m_2 & -m_3
       \end{pmatrix}.

    Parameters
    ----------
    jm1 : tuple of int
        :math:`(j_1, m_1)`.
    jm2 : tuple of int
        :math:`(j_2, m_2)`.
    jm3 : tuple of int
        :math:`(j_3, m_3)`.

    Returns
    -------
    float
        Value of the Clebsch-Gordan coefficient.
    """
    norm = sqrt(2*jm3[0]+1)*(-1 if jm1[0]-jm2[0]+jm3[1] % 2 else 1)
    return norm*three_j_symbol(jm1,jm2,(jm3[0],-jm3[1]))

def subarray(a, idxlist, n=None):
    """Extract a subarray from a matrix-like object.

    Parameters
    ----------
    a : numpy.ndarray
        Array to extract the subarray from.
    idxlist : list of tuples of int
        For each dimension, the indices to keep along that dimension.

    Returns
    -------
    numpy.ndarray
        The extracted subarray.

    Examples
    --------
    >>> import numpy as np
    >>> a = np.arange(2 * 4 * 4).reshape(2, 4, 4)
    >>> subarray(a, [(0,), (2, 3), (0, 1, 2, 3)])
    array([[[ 8,  9, 10, 11],
            [12, 13, 14, 15]]])

    This keeps

    - column 0 for the 1st dimension,
    - columns 2 and 3 for the 2nd dimension,
    - columns 0, 1, 2 and 3 for the 3rd dimension.
    """
    if n is None: n = len(a.shape)-1
    sa = a[tuple(slice(x) for x in a.shape[:n]) + (idxlist[n],)]
    return subarray(sa,idxlist, n-1) if n > 0 else sa
