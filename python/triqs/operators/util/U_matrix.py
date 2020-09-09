# Copyright (c) 2015-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2015-2017 Igor Krivenko
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
# Authors: Michel Ferrero, Igor Krivenko, Michael, Priyanka Seth, Nils Wentzell

from math import sqrt
from scipy.special import factorial as fact
from itertools import product
import numpy as np

# The interaction matrix in desired basis
# U^{spherical}_{m1 m2 m3 m4} = \sum_{k=0}^{2l} F_k angular_matrix_element(l, k, m1, m2, m3, m4)
# H = \frac{1}{2} \sum_{ijkl,\sigma \sigma'} U_{ijkl} a_{i \sigma}^\dagger a_{j \sigma'}^\dagger a_{l \sigma'} a_{k \sigma}.
def U_matrix(l, radial_integrals=None, U_int=None, J_hund=None, basis='spherical', T=None):
    r"""
    Calculate the full four-index U matrix being given either radial_integrals or U_int and J_hund.
    The convetion for the U matrix is that used to construct the Hamiltonians, namely:

    .. math:: H = \frac{1}{2} \sum_{ijkl,\sigma \sigma'} U_{ijkl} a_{i \sigma}^\dagger a_{j \sigma'}^\dagger a_{l \sigma'} a_{k \sigma}.

    Parameters
    ----------
    l : integer
        Angular momentum of shell being treated (l=2 for d shell, l=3 for f shell).
    radial_integrals : list, optional
                       Slater integrals [F0,F2,F4,..].
                       Must be provided if U_int and J_hund are not given.
                       Preferentially used to compute the U_matrix if provided alongside U_int and J_hund.
    U_int : scalar, optional
            Value of the screened Hubbard interaction.
            Must be provided if radial_integrals are not given.
    J_hund : scalar, optional
             Value of the Hund's coupling.
             Must be provided if radial_integrals are not given.
    basis : string, optional
            The basis in which the interaction matrix should be computed.
            Takes the values

            - 'spherical': spherical harmonics,
            - 'cubic': cubic harmonics,
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.
        The transformation matrix is defined such that new creation operators :math:`b^\dagger` are related to
        the old ones :math:`a^\dagger` as

        .. math:: b_{i \sigma}^\dagger = \sum_j T_{ij} a^\dagger_{j \sigma}.


    Returns
    -------
    U_matrix : float numpy array
               The four-index interaction matrix in the chosen basis.

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
    # U^{spherical}_{m1 m2 m3 m4} = \sum_{k=0}^{2l} F_k angular_matrix_element(l, k, m1, m2, m3, m4)
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

# Convert full 4-index U matrix to 2-index density-density form
def reduce_4index_to_2index(U_4index):
    r"""
    Reduces the four-index matrix to two-index matrices for parallel and anti-parallel spins.

    Parameters
    ----------
    U_4index : float numpy array
               The four-index interaction matrix.

    Returns
    -------
    U : float numpy array
        The two-index interaction matrix for parallel spins.
    Uprime : float numpy array
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

# Construct the 2-index matrices for the density-density form
def U_matrix_kanamori(n_orb, U_int, J_hund):
    r"""
    Calculate the Kanamori U and Uprime matrices.

    Parameters
    ----------
    n_orb : integer
            Number of orbitals in basis.
    U_int : scalar
            Value of the screened Hubbard interaction.
    J_hund : scalar
             Value of the Hund's coupling.

    Returns
    -------
    U : float numpy array
        The two-index interaction matrix for parallel spins.
    Uprime : float numpy array
             The two-index interaction matrix for anti-parallel spins.

    """

    U  = np.zeros((n_orb,n_orb),dtype=float)      # matrix for same spin
    Uprime = np.zeros((n_orb,n_orb),dtype=float)  # matrix for opposite spin

    m_range = list(range(n_orb))
    for m,mp in product(m_range,m_range):
        if m == mp:
            Uprime[m,mp] = U_int
        else:
            U[m,mp]  = U_int - 3.0*J_hund
            Uprime[m,mp] = U_int - 2.0*J_hund

    return U, Uprime

# Get t2g or eg components
def t2g_submatrix(U, convention=''):
    r"""
    Extract the t2g submatrix of the full d-manifold two- or four-index U matrix.

    Parameters
    ----------
    U : float numpy array
        Two- or four-index interaction matrix.
    convention : string, optional
                 The basis convention.
                 Takes the values

                 - '': basis ordered as ("xy","yz","z^2","xz","x^2-y^2"),
                 - 'wien2k': basis ordered as ("z^2","x^2-y^2","xy","yz","xz").

    Returns
    -------
    U_t2g : float numpy array
            The t2g component of the interaction matrix.

    """
    if convention == 'wien2k':
        return subarray(U, len(U.shape)*[(2,3,4)])
    elif convention== '':
        return subarray(U, len(U.shape)*[(0,1,3)])
    else:
        raise ValueError("Unknown convention: "+str(convention))

def eg_submatrix(U, convention=''):
    r"""
    Extract the eg submatrix of the full d-manifold two- or four-index U matrix.

    Parameters
    ----------
    U : float numpy array
        Two- or four-index interaction matrix.
    convention : string, optional
                 The basis convention.
                 Takes the values

                 - '': basis ordered as ("xy","yz","z^2","xz","x^2-y^2"),
                 - 'wien2k': basis ordered as ("z^2","x^2-y^2","xy","yz","xz").


    Returns
    -------
    U_eg : float numpy array
           The eg component of the interaction matrix.

    """
    if convention == 'wien2k':
        return subarray(U, len(U.shape)*[(0,1)])
    elif convention == '':
        return subarray(U, len(U.shape)*[(2,4)])
    else:
        raise ValueError("Unknown convention: "+str(convention))


# Transform the interaction matrix into another basis
def transform_U_matrix(U_matrix, T):
    r"""
    Transform a four-index interaction matrix into another basis.
    The transformation matrix is defined such that new creation operators :math:`b^\dagger` are related to
    the old ones :math:`a^\dagger` as

    .. math:: b_{i \sigma}^\dagger = \sum_j T_{ij} a^\dagger_{j \sigma}.

    Parameters
    ----------
    U_matrix : float numpy array
               The four-index interaction matrix in the original basis.
    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    U_matrix : float numpy array
               The four-index interaction matrix in the new basis.

    """
    return np.einsum("ij,kl,jlmo,mn,op",np.conj(T),np.conj(T),U_matrix,np.transpose(T),np.transpose(T))

# Rotation matrices: complex harmonics to cubic harmonics
# Complex harmonics basis: ..., Y_{-2}, Y_{-1}, Y_{0}, Y_{1}, Y_{2}, ...
def spherical_to_cubic(l, convention=''):
    r"""
    Get the spherical harmonics to cubic harmonics transformation matrix.

    Parameters
    ----------
    l : integer
        Angular momentum of shell being treated (l=2 for d shell, l=3 for f shell).
    convention : string, optional
                 The basis convention.
                 Takes the values

                 - '': basis ordered as ("xy","yz","z^2","xz","x^2-y^2"),
                 - 'wien2k': basis ordered as ("z^2","x^2-y^2","xy","yz","xz").

    Returns
    -------
    T : real/complex numpy array
        Transformation matrix for basis change.

    """

    if not convention in ('wien2k',''):
        raise ValueError("Unknown convention: "+str(convention))

    size = 2*l+1
    T = np.zeros((size,size),dtype=complex)
    if convention == 'wien2k' and l != 2:
        raise ValueError("spherical_to_cubic: wien2k convention implemented only for l=2")
    if l == 0:
        cubic_names = ("s")
    elif l == 1:
        cubic_names = ("x","y","z")
        T[0,0] = 1.0/sqrt(2);   T[0,2] = -1.0/sqrt(2)
        T[1,0] = 1j/sqrt(2);    T[1,2] = 1j/sqrt(2)
        T[2,1] = 1.0
    elif l == 2:
        if convention == 'wien2k':
            cubic_names = ("z^2","x^2-y^2","xy","yz","xz")
            T[0,2] = 1.0
            T[1,0] = 1.0/sqrt(2);   T[1,4] = 1.0/sqrt(2)
            T[2,0] =-1.0/sqrt(2);   T[2,4] = 1.0/sqrt(2)
            T[3,1] = 1.0/sqrt(2);   T[3,3] =-1.0/sqrt(2)
            T[4,1] = 1.0/sqrt(2);   T[4,3] = 1.0/sqrt(2)
        else:
            cubic_names = ("xy","yz","z^2","xz","x^2-y^2")
            T[0,0] = 1j/sqrt(2);    T[0,4] = -1j/sqrt(2)
            T[1,1] = 1j/sqrt(2);    T[1,3] = 1j/sqrt(2)
            T[2,2] = 1.0
            T[3,1] = 1.0/sqrt(2);   T[3,3] = -1.0/sqrt(2)
            T[4,0] = 1.0/sqrt(2);   T[4,4] = 1.0/sqrt(2)
    elif l == 3:
        cubic_names = ("x(x^2-3y^2)","z(x^2-y^2)","xz^2","z^3","yz^2","xyz","y(3x^2-y^2)")
        T[0,0] = 1.0/sqrt(2);    T[0,6] = -1.0/sqrt(2)
        T[1,1] = 1.0/sqrt(2);    T[1,5] = 1.0/sqrt(2)
        T[2,2] = 1.0/sqrt(2);    T[2,4] = -1.0/sqrt(2)
        T[3,3] = 1.0
        T[4,2] = 1j/sqrt(2);   T[4,4] = 1j/sqrt(2)
        T[5,1] = 1j/sqrt(2);   T[5,5] = -1j/sqrt(2)
        T[6,0] = 1j/sqrt(2);   T[6,6] = 1j/sqrt(2)
    else: raise ValueError("spherical_to_cubic: implemented only for l=0,1,2,3")

    return np.matrix(T)

# Names of cubic harmonics
def cubic_names(l):
    r"""
    Get the names of the cubic harmonics.

    Parameters
    ----------
    l : integer or string
        Angular momentum of shell being treated.
        Also takes 't2g' and 'eg' as arguments.

    Returns
    -------
    cubic_names : tuple of strings
                  Names of the orbitals.

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

# Convert U,J -> radial integrals F_k
def U_J_to_radial_integrals(l, U_int, J_hund):
    r"""
    Determine the radial integrals F_k from U_int and J_hund.

    Parameters
    ----------
    l : integer
        Angular momentum of shell being treated (l=2 for d shell, l=3 for f shell).
    U_int : scalar
            Value of the screened Hubbard interaction.
    J_hund : scalar
             Value of the Hund's coupling.

    Returns
    -------
    radial_integrals : list
                       Slater integrals [F0,F2,F4,..].

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

# Convert radial integrals F_k -> U,J
def radial_integrals_to_U_J(l, F):
    r"""
    Determine U_int and J_hund from the radial integrals.

    Parameters
    ----------
    l : integer
        Angular momentum of shell being treated (l=2 for d shell, l=3 for f shell).
    F : list
        Slater integrals [F0,F2,F4,..].

    Returns
    -------
    U_int : scalar
            Value of the screened Hubbard interaction.
    J_hund : scalar
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

# Angular matrix elements of particle-particle interaction
# (2l+1)^2 ((l 0) (k 0) (l 0))^2 \sum_{q=-k}^{k} (-1)^{m1+m2+q} ((l -m1) (k q) (l m3)) ((l -m2) (k -q) (l m4))
def angular_matrix_element(l, k, m1, m2, m3, m4):
    r"""
    Calculate the angular matrix element

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
    l : integer
    k : integer
    m1 : integer
    m2 : integer
    m3 : integer
    m4 : integer

    Returns
    -------
    ang_mat_ele : scalar
                  Angular matrix element.

    """
    ang_mat_ele = 0
    for q in range(-k,k+1):
        ang_mat_ele += three_j_symbol((l,-m1),(k,q),(l,m3))*three_j_symbol((l,-m2),(k,-q),(l,m4))*(-1.0 if (m1+q+m2) % 2 else 1.0)
    ang_mat_ele *= (2*l+1)**2 * (three_j_symbol((l,0),(k,0),(l,0))**2)
    return ang_mat_ele

# Wigner 3-j symbols
# ((j1 m1) (j2 m2) (j3 m3))
def three_j_symbol(jm1, jm2, jm3):
    r"""
    Calculate the three-j symbol

    .. math::
       \begin{pmatrix}
        l_1 & l_2 & l_3\\
        m_1 & m_2 & m_3
       \end{pmatrix}.

    Parameters
    ----------
    jm1 : tuple of integers
          (j_1 m_1)
    jm2 : tuple of integers
          (j_2 m_2)
    jm3 : tuple of integers
          (j_3 m_3)

    Returns
    -------
    three_j_sym : scalar
                  Three-j symbol.

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

# Clebsch-Gordan coefficients
# < j1 m1 j2 m2 | j3 m3 > = (-1)^{j1-j2+m3} \sqrt{2j3+1} ((j1 m1) (j2 m2) (j3 -m3))
def clebsch_gordan(jm1, jm2, jm3):
    r"""
    Calculate the Clebsh-Gordan coefficient

    .. math::
       \langle j_1 m_1 j_2 m_2 | j_3 m_3 \rangle = (-1)^{j_1-j_2+m_3} \sqrt{2 j_3 + 1}
       \begin{pmatrix}
         j_1 & j_2 & j_3\\
         m_1 & m_2 & -m_3
       \end{pmatrix}.

    Parameters
    ----------
    jm1 : tuple of integers
          (j_1 m_1)
    jm2 : tuple of integers
          (j_2 m_2)
    jm3 : tuple of integers
          (j_3 m_3)

    Returns
    -------
    cgcoeff : scalar
              Clebsh-Gordan coefficient.

    """
    norm = sqrt(2*jm3[0]+1)*(-1 if jm1[0]-jm2[0]+jm3[1] % 2 else 1)
    return norm*three_j_symbol(jm1,jm2,(jm3[0],-jm3[1]))

# Create subarray containing columns in idxlist
# e.g. idxlist = [(0),(2,3),(0,1,2,3)] gives
#  column 0 for 1st dim,
#  columns 2 and 3 for 2nd dim,
#  columns 0,1,2 and 3 for 3rd dim.
def subarray(a,idxlist,n=None) :
    r"""
    Extract a subarray from a matrix-like object.

    Parameters
    ----------
    a : matrix or array
    idxlist : list of tuples
              Columns that need to be extracted for each dimension.

    Returns
    -------
    subarray : matrix or array

    Examples
    --------
    idxlist = [(0),(2,3),(0,1,2,3)] gives

    - column 0 for 1st dim,
    - columns 2 and 3 for 2nd dim,
    - columns 0, 1, 2 and 3 for 3rd dim.

    """
    if n is None: n = len(a.shape)-1
    sa = a[tuple(slice(x) for x in a.shape[:n]) + (idxlist[n],)]
    return subarray(sa,idxlist, n-1) if n > 0 else sa
