# Copyright (c) 2021 Simons Foundation
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
# Authors: Nils Wentzell

from io import StringIO
import numpy as np

__all__ = ['k_space_path']


def k_space_path(paths, num=101, bz=None):
    """ Generate an array of k-vectors along a path defined by a list of pairs of k-vectors

    Parameters
    ----------
    paths : list of pairs of three-vectors of floats
       List of pairs of k-vectors (in reciprocal units) to create a path in-between.
    num : int, default=100
       Number of k-vectors along each segment the paths
    bz : brillouin_zone, optional
       When a Brillouin Zone is passed, calculate distance in absoulte units

    Returns
    -------
    kvecs: numpy.array [shape=(len(paths)*num,3)]
        Two-dimensional numpy array containing the path vectors as rows
    dist: numpy.array  [shape=(kvecs.shape[0])]
        One-dimensional numpy array containing, for each element in kvecs,
        the distance travelled along the path. Useful for plotting.
        The distances for the relevant k-points in paths can be obtained with dist[num-1::num].
    """

    if bz is None:
        cell = np.eye(3)
    else:
        cell = bz.units

    x = np.linspace(0., 1., num=num)
    kvecs = [ki[None, :] + x[:, None] * (kf - ki)[None, :] for ki, kf in paths]

    cur_dist = 0.0
    dist = np.array([], dtype=float)
    for kvec in kvecs:
        kvec_abs = np.dot(kvec, cell)
        dist_new = np.linalg.norm(kvec_abs - kvec_abs[0], axis=1) + cur_dist
        dist = np.concatenate((dist, dist_new))

        cur_dist = dist[-1]

    return np.vstack(kvecs), dist


# ----------------------------------------------------------------------
def parse_hopping_from_wannier90_hr_dat(filename):
    r""" Wannier90 real space hopping parser of ``*_hr.dat`` files.

    Returns a dictionary where the keys are the real-space hopping vectors,
    in terms of multiples of the lattice vectors, and the values are
    ``num_wann * num_wann`` numpy ndarrays with the hopping integrals.

    Parameters
    ----------
    filename : str
        Wannier90 ``*_hr.dat`` file to parse.

    Returns
    -------
    hopp_dict : dict
        Dictionary of real space hoppings.
    num_wann : int
        Total number of Wannier functions per unit-cell.

    """

    with open(filename, 'r') as fd:
        lines = fd.readlines()

    lines.pop(0)  # pop time header

    num_wann = int(lines.pop(0))
    nrpts = int(lines.pop(0))

    nlines = int(np.ceil(float(nrpts / 15.)))

    deg = np.array([])
    for line in lines[:nlines]:
        deg = np.concatenate((deg, np.loadtxt(StringIO(line), dtype=int, ndmin=1)))

    assert deg.shape == (nrpts,)

    hopp = "".join(lines[nlines:])
    hopp = np.loadtxt(StringIO(hopp))

    assert hopp.shape == (num_wann**2 * nrpts, 7)

    # Lattice coordinates in multiples of lattice vectors
    R = np.array(hopp[:, :3], dtype=int)
    # orbital index pairs, wannier90 counts from 1, fix by remove 1
    nm = np.array(hopp[:, 3:5], dtype=int) - 1

    t_re = hopp[:, 5]
    t_im = hopp[:, 6]

    # complex hopping amplitudes for each R, mn (H(R)_{mn})
    t = t_re + 1.j * t_im

    # -- Dict with hopping matrices
    r_dict = {}
    hopp_dict = {}
    for idx in range(R.shape[0]):
        r = tuple(R[idx])

        if r not in r_dict:
            r_dict[r] = 1
        else:
            r_dict[r] += 1

        if r not in hopp_dict:
            hopp_dict[r] = np.zeros((num_wann, num_wann), dtype=complex)

        n, m = nm[idx]
        hopp_dict[r][n, m] = t[idx]

    # -- Account for degeneracy of the Wigner-Seitz points

    for r, weight in zip(list(r_dict.keys()), deg):
        hopp_dict[r] /= weight

    return hopp_dict, num_wann


def parse_lattice_vectors_from_wannier90_wout(filename):
    r""" Wannier90 real space lattice vectors parser of ``*.wout`` files.

    Parameters
    ----------
    filename : str
        Wannier90 ``*.wout`` file to parse.

    Returns
    -------
    vectors : list of three three-tuples of floats
        Lattice vectors.

    """

    with open(filename, 'r') as fd:
        lines = fd.readlines()

    # -- Find start of data in text file
    for idx, line in enumerate(lines):
        if 'Lattice Vectors' in line:
            if '(Ang)' in line:
                unit = 1.0
            elif '(Bohr)' in line:
                unit = 0.5291772105638411
            else:
                raise NotImplementedError
            break

    if 'Lattice Vectors' not in line:
        raise IOError

    lines = "".join(lines[idx+1:idx+4])
    array = np.loadtxt(StringIO(lines), usecols=(1, 2, 3))

    array *= unit

    # -- convert 3x3 array to list of tuples
    vectors = []
    for idx in range(array.shape[0]):
        v = tuple(array[idx])
        vectors.append(v)

    return vectors


def extend_wannier90_to_spin(hopping, num_wann):
    hopping_spin = {}
    for key, value in hopping.items():
        hopping_spin[key] = np.kron(np.eye(2), value)
    return hopping_spin, 2 * num_wann


def TB_from_wannier90(seed, path='./',  extend_to_spin=False, add_local=None):
    r"""
    read wannier90 output and convert to TBLattice object

    reads wannier90 real space lattice vectors from seed.wout file.
    reads wannier90 hoppings from seed_hr.dat file

    Parameters
    ----------
    seed : str
        seedname of wannier90 run, name of *_hr.dat
    path : str, default = './'
        path to wannier90 output dir
    extend_to_spin: bool, default= False
        extend hopping Hamiltonian with spin indices
    add_local: numpy array , default = None
        add a local term to hopping[0,0,0] of shape Norb x Norb

    Returns
    -------
    TBL : triqs TBLattice object
        triqs tight binding object

    """

    from triqs.lattice.tight_binding import TBLattice

    hopping, num_wann = parse_hopping_from_wannier90_hr_dat(path + seed + '_hr.dat')
    units = parse_lattice_vectors_from_wannier90_wout(path + seed + '.wout')

    if extend_to_spin:
        hopping, num_wann = extend_wannier90_to_spin(hopping, num_wann)

    if add_local is not None:
        hopping[(0, 0, 0)] += add_local

    TBL = TBLattice(units=units, hopping=hopping, orbital_positions=[(0, 0, 0)]*num_wann,
                    orbital_names=[str(i) for i in range(num_wann)])
    return TBL
