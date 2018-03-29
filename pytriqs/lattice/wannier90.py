################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2017 by The Simons Foundation
# Author: H. U.R. Strand
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

import numpy as np
from StringIO import StringIO
from collections import OrderedDict

# ----------------------------------------------------------------------
def parse_hopping_from_wannier90_hr_dat(filename):

    """ Parse real space hopping matrix from Wannier90 "seedname_hr.dat". 
    Returns a dictionary where the keys are the real-space vectors, in 
    terms of multiples of the lattice vectors, and the values are
    num_wannier x num_wannier numpy arrays with the hopping integrals. """
    
    with open(filename, 'r') as fd:
        lines = fd.readlines()

    lines.pop(0) # pop time header

    num_wann = int(lines.pop(0))
    nrpts = int(lines.pop(0))

    #print 'num_wann =', num_wann
    #print 'nrpts =', nrpts

    nlines = int(nrpts / 15)

    deg = "".join(lines[:nlines])
    deg = np.loadtxt(StringIO(deg), dtype=np.int)
    deg = deg.flatten()

    assert( deg.shape == (nrpts,) )

    hopp = "".join(lines[nlines:])
    hopp = np.loadtxt(StringIO(hopp))

    assert( hopp.shape == (num_wann**2 * nrpts, 7) )
    
    R = np.array(hopp[:, :3], dtype=np.int) # Lattice coordinates in multiples of lattice vectors
    nm = np.array(hopp[:, 3:5], dtype=np.int) - 1 # orbital index pairs, wannier90 counts from 1, fix by remove 1

    t_re = hopp[:, 5]
    t_im = hopp[:, 6]
    t = t_re + 1.j * t_im # complex hopping amplitudes for each R, mn (H(R)_{mn})

    # -- Dict with hopping matrices

    r_dict = OrderedDict()
    hopp_dict = {}
    for idx in xrange(R.shape[0]):
        r = tuple(R[idx])

        if r not in r_dict:
            r_dict[r] = 1
        else:
            r_dict[r] += 1

        if r not in hopp_dict:
            hopp_dict[r] = np.zeros((num_wann, num_wann), dtype=np.complex)

        n, m = nm[idx]
        hopp_dict[r][n, m] = t[idx]

    # -- Account for degeneracy of the Wigner-Seitz points
    
    for r, weight in zip(r_dict.keys(), deg):
        hopp_dict[r] /= weight

    return hopp_dict, num_wann
        
# ----------------------------------------------------------------------
def parse_lattice_vectors_from_wannier90_wout(filename):

    """ Extract the lattice vectors from the Wannier90
    "seedname.wout" file. Returns a list of three tuples. """

    with open(filename, 'r') as fd:
        lines = fd.readlines()

    # -- Find start of data in text file
    
    for idx, line in enumerate(lines):
        if 'Lattice Vectors (Ang)' in line:
            break

    lines = "".join(lines[idx+1:idx+4])
    array = np.loadtxt(StringIO(lines), usecols=(1, 2, 3))

    # -- convert 3x3 array to list of tuples
    
    vectors = []
    for idx in xrange(array.shape[0]):
        v = tuple(array[idx])
        vectors.append(v)

    return vectors

# ----------------------------------------------------------------------
def parse_reciprocal_lattice_vectors_from_wannier90_wout(filename):

    """ Extract the lattice vectors from the Wannier90
    "seedname.wout" file. Returns a list of three tuples. """

    with open(filename, 'r') as fd:
        lines = fd.readlines()

    # -- Find start of data in text file
    
    for idx, line in enumerate(lines):
        if 'Reciprocal-Space Vectors (Ang^-1)' in line:
            break

    lines = "".join(lines[idx+1:idx+4])
    array = np.loadtxt(StringIO(lines), usecols=(1, 2, 3))

    # -- convert 3x3 array to list of tuples
    
    vectors = []
    for idx in xrange(array.shape[0]):
        v = tuple(array[idx])
        vectors.append(v)

    return vectors

# ----------------------------------------------------------------------
def parse_band_structure_from_wannier90_band_dat(filename):

    """ Extract the reciprocal lattice vectors from the Wannier90
    "seedname.wout" file. Returns a list of three tuples. """

    with open(filename, 'r') as fd:
        lines = fd.readlines()

    # -- Count the number of empty lines

    num_wann = 0
    for line in lines:
        if line.strip(' ') == '\n':
            num_wann += 1

    lines = "".join(lines)

    data = np.loadtxt(StringIO(lines)).T
    n_pts = data.shape[-1]

    data = data.reshape(2, num_wann, n_pts/num_wann)    
    w, E = data[0], data[1]

    return E, w

