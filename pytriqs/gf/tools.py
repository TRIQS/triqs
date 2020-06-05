################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
# Copyright (C) 2019 The Simons Foundation
# Authors: H. U.R. Strand
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
from . import lazy_expressions, descriptors, gf_fnt
from .meshes import MeshImFreq, MeshReFreq, MeshImTime, MeshReTime, MeshLegendre
from .block_gf import BlockGf
from .gf import Gf
import numpy as np
from itertools import product
from .backwd_compat.gf_refreq import GfReFreq 
from .map_block import map_block

def inverse(x):
    """
    Return the inverse of a Green's function
    """
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("inverse", inverse) (x)
    assert hasattr(x,'inverse')
    return x.inverse()

def conjugate(x):
    """
    Return the conjugate of a Green's function
    """
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("conjugate", conjugate) (x)
    assert hasattr(x,'conjugate')
    return x.conjugate()

def transpose(x):
    """
    Return the transpose of a Green's function
    """
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("transpose", transpose) (x)
    assert hasattr(x,'transpose')
    return x.transpose()


def delta(g):
    """
    Compute Delta_iw from G0_iw.
    CAUTION: This function assumes the following properties of g
      * The diagonal components of g should decay as 1/iOmega
      * g should fullfill the property g[iw][i,j] = conj(g[-iw][j,i])

    Parameters
    ----------
    g : BlockGf (of GfImFreq) or GfImFreq
        Non-interacting Green's function.

    Returns
    -------
    delta_iw : BlockGf (of GfImFreq) or GfImFreq
               Hybridization function.
    """

    if isinstance(g, BlockGf):
        return BlockGf(name_block_generator = [ (n, delta(g0)) for n,g0 in g], make_copies=False)
    elif isinstance(g.mesh, MeshImFreq):
        assert len(g.target_shape) in [0,2], "delta(g) requires a matrix or scalar_valued Green function"
        assert gf_fnt.is_gf_hermitian(g), "delta(g) requires a Green function with the property g[iw][i,j] = conj(g[-iw][j,i])"
        delta_iw = g.copy()
        delta_iw << descriptors.iOmega_n - inverse(g)
        tail, err = gf_fnt.fit_hermitian_tail(delta_iw)
        delta_iw << delta_iw - tail[0]
        if err > 1e-5: print("WARNING: delta extraction encountered a sizeable tail-fit error: ", err)
        return delta_iw
    else:
        raise TypeError("No function delta for g0 object of type %s"%type(g)) 


# Determine one of G0_iw, G_iw and Sigma_iw from other two using Dyson's equation
def dyson(**kwargs):
    """
    Solve Dyson's equation for given two of G0_iw, G_iw and Sigma_iw to yield the third.

    Parameters
    ----------
    G0_iw : Gf, optional
            Non-interacting Green's function.
    G_iw : Gf, optional
           Interacting Green's function.
    Sigma_iw : Gf, optional
               Self-energy.
    """
    if not (len(kwargs)==2 and set(kwargs.keys())<set(['G0_iw','G_iw', 'Sigma_iw'])):
        raise ValueError('dyson: Two (and only two) of G0_iw, G_iw and Sigma_iw must be provided to determine the third.')
    if 'G0_iw' not in kwargs:
        G0_iw = inverse(kwargs['Sigma_iw'] + inverse(kwargs['G_iw']))
        return G0_iw
    elif 'G_iw' not in kwargs:
        G_iw = inverse(inverse(kwargs['G0_iw']) - kwargs['Sigma_iw'])
        return G_iw
    elif 'Sigma_iw' not in kwargs:
        Sigma_iw = inverse(kwargs['G0_iw']) - inverse(kwargs['G_iw'])
        return Sigma_iw

def read_gf_from_txt(block_txtfiles, block_name):
    """
    Read a GfReFreq from text files with the format (w, Re(G), Im(G)) for a single block.
    
    Notes
    -----
    A BlockGf must be constructed from multiple GfReFreq objects if desired.
    The mesh must be the same for all files read in.
    Non-uniform meshes are not supported.

    Parameters
    ----------
    block_txtfiles: Rank 2 square np.array(str) or list[list[str]]
        The text files containing the GF data that need to read for the block.
        e.g. [['up_eg1.dat']] for a one-dimensional block and
             [['up_eg1_1.dat','up_eg2_1.dat'],
              ['up_eg1_2.dat','up_eg2_2.dat']] for a 2x2 block.
    block_name: str
        Name of the block.

    Returns
    -------
    g: GfReFreq
        The real frequency Green's function read in.
    """
    block_txtfiles = np.array(block_txtfiles) # Must be an array to use certain functions
    N1,N2 = block_txtfiles.shape
    mesh = np.genfromtxt(block_txtfiles[0,0],usecols=[0]) # Mesh needs to be the same for all blocks
    g = GfReFreq(indices=list(range(N1)),window=(np.min(mesh),np.max(mesh)),n_points=len(mesh),name=block_name)
    for i,j in product(list(range(N1)),list(range(N2))):
        data = np.genfromtxt(block_txtfiles[i,j],usecols=[1,2])
        g.data[:,i,j] = data[:,0] + 1j*data[:,1]
    return g


def write_gf_to_txt(g):
    """
    Write a GfReFreq or GfImFreq to in the format (w/iw, Re(G), Im(G)) for a single block.
    
    Parameters
    ----------
    g: GfReFreq or GfImFreq
        The real/imaginary frequency Green's function to be written out.
    """
    if isinstance(g.mesh, MeshReFreq):
        mesh = np.array([w.real for w in g.mesh]).reshape(-1,1)
    elif isinstance(g.mesh, MeshImFreq):
        mesh = np.array([w.imag for w in g.mesh]).reshape(-1,1)
    else:
        raise ValueError('write_gf_to_txt: Only GfReFreq and GfImFreq quantities are supported.')
    for i,j in product(list(range(g.target_shape[0])),list(range(g.target_shape[1]))):
        txtfile = '%s_%s_%s.dat'%(g.name,i,j)
        redata = g.data[:,i,j].real.reshape((g.data.shape[0],-1))
        imdata = g.data[:,i,j].imag.reshape((g.data.shape[0],-1))
        mesh_and_data = np.hstack((mesh,redata,imdata))
        np.savetxt(txtfile,mesh_and_data)


def make_zero_tail(g, n_moments=10):
    """
    Return a container for the high-frequency coefficients of a given Green function initialized to zero.
    
    Parameters
    ----------
    g: GfImFreq or GfReFreq or GfImTime or GfReTime
        The real/imaginary frequency/time Green's function that we create the tail-array for.

    n_moments [default=10]: The number of high-frequency moments in the tail (starting from order 0).
    """
    if isinstance(g, Gf) and isinstance(g.mesh, (MeshImFreq, MeshReFreq, MeshImTime, MeshReTime)):
        n_moments = max(1, n_moments)
        return np.zeros((n_moments,) + g.target_shape, dtype = np.complex128)
    elif isinstance(g, BlockGf):
        return map_block(lambda g_bl: make_zero_tail(g_bl, n_moments), g)
    else:
        raise RuntimeError("Error: make_zero_tail has to be called on a frequency or time Green function object")


def fit_legendre(g_t, order=10):
    """ General fit of a noisy imaginary time Green's function
    to a low order Legendre expansion in imaginary time.

    Only Hermiticity is imposed on the fit, so discontinuities has
    to be fixed separately (see the method enforce_discontinuity)

    Author: Hugo U.R. Strand 

    Parameters
    ----------

    g_t : TRIQS imaginary time Green's function (matrix valued)
          Imaginary time Green's function to fit (possibly noisy binned data)

    order : int
            Maximal order of the fitted Legendre expansion

    Returns
    -------

    g_l : TRIQS Legendre polynomial Green's function (matrix valued)
          Fitted Legendre Green's function with order `order`
    """

    import numpy.polynomial.legendre as leg

    if isinstance(g_t, BlockGf):
        return map_block(lambda g_bl: fit_legendre(g_bl, order), g_t)

    assert isinstance(g_t, Gf) and isinstance(g_t.mesh, MeshImTime), "fit_legendre expects imaginary-time Green function objects"
    assert len(g_t.target_shape) == 2, "fit_legendre currently only implemented for matrix_valued Green functions"

    # -- flatten the data to 2D N_tau x (N_orb * N_orb)

    shape = g_t.data.shape
    fshape = [shape[0], np.prod(shape[1:])]

    # -- extend data accounting for hermiticity

    mesh = g_t.mesh
    tau = np.array([ t.value for t in mesh ])
    # Rescale to the interval (-1,1)
    x = 2. * tau / mesh.beta - 1.

    data = g_t.data.reshape(fshape)
    data_herm = np.transpose(g_t.data, axes=(0, 2, 1)).conjugate().reshape(fshape)

    # -- Separated real valued linear system, with twice the number of RHS terms

    data_re = 0.5 * (data + data_herm).real
    data_im = 0.5 * (data + data_herm).imag
    data_ext = np.hstack((data_re, data_im))

    c_l_ext = leg.legfit(x, data_ext, order - 1)
    c_l_re, c_l_im = np.split(c_l_ext, 2, axis=-1)
    c_l = c_l_re + 1.j * c_l_im

    # -- make Legendre Green's function of the fitted coeffs

    lmesh = MeshLegendre(mesh.beta, mesh.statistic, order)

    # Nb! We have to scale the actual Legendre coeffs to the Triqs "scaled" Legendre coeffs
    # see Boehnke et al. PRB (2011)
    l = np.arange(len(lmesh))
    scale = np.sqrt(2.*l + 1) / mesh.beta
    scale = scale.reshape([len(lmesh)] + [1]*len(g_t.target_shape))
    
    g_l = Gf(mesh=lmesh, target_shape=g_t.target_shape)
    g_l.data[:] = c_l.reshape(g_l.data.shape) / scale

    return g_l
