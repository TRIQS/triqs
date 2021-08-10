# Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2015 Igor Krivenko
# Copyright (c) 2018-2020 Simons Foundation
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
# Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

from . import lazy_expressions, descriptors, gf_fnt
from .meshes import MeshImFreq, MeshReFreq, MeshImTime, MeshReTime, MeshLegendre
from .block_gf import BlockGf
from .gf import Gf
from .gf_factories import make_hermitian
import numpy as np
from itertools import product
from .backwd_compat.gf_refreq import GfReFreq
from .map_block import map_block
from timeit import default_timer as timer

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

def make_delta(V, eps, mesh, block_names = None):
    """
    creates a hybridization function from given hoppings and bath energies
    as
    .. math:: \Delta_{kl}^{disc} (i \omega_n) = \sum_{i=1}^{Nb} V_{ki} S V_{il}* .
    where S is either:
    .. math:: [i \omega_n - \epsilon_i]^{-1}
    for MeshImFreq or
    .. math:: - exp(-1* tau * e_i) / (1+ exp(-\beta * e_i) )
    for MeshImTime

    Parameters
    -----------
    mesh : triqs Gf MeshImFreq or MeshImTime object
    V_opt : bath hoppings V (shape Norb x nB) as numpy array per Gf block
    e_opt : list of optimized bath energies per Gf block
    block_names : list of block names, used if V, eps are lists

    Returns
    -------
    delta : Gf or BlockGf
            Discretized hybridization function on given mesh

    """

    if isinstance(V, list):
        delta_list = []
        for V_bloc, e_bloc in zip(V, eps):
            delta_list.append(make_delta(V_bloc, e_bloc, mesh))

        if isinstance(block_names, list) and len(block_names) == len(delta_list):
            b_names = block_names
        else:
            b_names = [str(i) for i in range(len(delta_list))]

        return BlockGf(name_list = b_names, block_list = delta_list)


    assert V.shape[1] == len(eps), 'number of bath sides in V and eps does not match'

    delta_res = Gf(mesh=mesh, target_shape=[V.shape[0],V.shape[0]])

    mesh_values = np.array([w.value for w in mesh])

    if isinstance(mesh, MeshImFreq):
        one_fermion = 1/(mesh_values[:,None,None] - eps[None,None,:])
    elif isinstance(mesh, MeshImTime):
        one_fermion = -np.exp(-mesh_values[:,None,None] * eps[None,None,:]) / (1. + np.exp(-mesh.beta * eps[None,None,:]))

    delta_res.data[:] = np.einsum('wij, wjk -> wik', V[None,:,:] * one_fermion, V.conj().T[None,:,:])

    return delta_res

def discretize_bath(delta_in, Nb, eps0= 3, V0 = 0.0, tol=1e-8, maxiter = 10000, cmplx = False):
    """
    discretizes a given Delta_iw with Nb bath sites using
    scipy.optimize.minimize using the Nelder-Mead algorithm.
    The tolerance is ensuring the converging of the input
    hopping and energy values. The discretized delta_disc_iw
    is constructed as:
    .. math:: \Delta_{kl}^{disc} (i \omega_n) = \sum_{i=1}^{Nb} V_{ki} S  V_{il}^* .
    where S is either:
    .. math:: [i \omega_n - \epsilon_i]^{-1}
    for MeshImFreq or
    .. math:: - exp(-1* tau * eps_i) / (1+ exp(-\beta * eps_i) )
    for MeshImTime.

    and the norm to minimize the difference between delta_disc_iw and delta_in for scipy is:
    .. math:: \left[ \frac{1}{\sqrt(N)} \sum_{i \omega_n}^{N} | \Delta^{disc} (i \omega_n) - \Delta (i \omega_n) |^2 \right]^{\frac{1}{2}}
    and for MeshImTime
    .. math:: \left[ \frac{1}{\sqrt(N)} \sum_{\tau}^{N} | \Delta^{disc} (\tau) - \Delta (\tau) |^2 \right]^{\frac{1}{2}}

    Parameters
    ----------
    delta_in : Gf or BlockGf
        Matsubara hybridization function to discretize
    Nb : int
        number of bath sites per Gf block
    eps0: float or list of floats, optional
        initial guesses for bath energies or if a single value is given approximate bandwidth for equidistant bath energies [default=3.0]
    V0 : float or np.ndarray, optional
        initial guess used for all hopping values [default=0.0] or if a np.ndarray of correct shape (norbxNb) is given initial guess for hoppings
    tol : float, optional
        tolerance for scipy minimize on data to optimize (xatol Nelder-Mead) [default=1e-8]
    maxiter : float, optional
        maximal number of optimization steps [default=10000]
    complx : allow the hoppings V to be complex
    Returns
    -------
    V_opt : sorted bath hoppings V as numpy array (list if Gf block is given)
    eps_opt : sorted list of optimized bath energies (list if Gf block is given)
    delta_disc : Gf or BlockGf
               Discretized hybridization function
    """
    from scipy.optimize import minimize

    if isinstance(delta_in, BlockGf):
        V_opt, eps_opt, delta_list = [], [], []
        for i, (block, delta) in zip(range(len(list(delta_in.indices))),delta_in):
            if isinstance(eps0, list) and isinstance(V0, list):
                res = discretize_bath(delta, Nb, eps0[i], V0[i], tol, maxiter, cmplx)
            else:
                res = discretize_bath(delta, Nb, eps0, V0, tol, maxiter, cmplx)
            V_opt.append(res[0])
            eps_opt.append(res[1])
            delta_list.append(res[2])

        return V_opt, eps_opt, BlockGf(name_list = list(delta_in.indices), block_list = delta_list)

    # some tests if input is okay
    assert isinstance(delta_in.mesh, MeshImFreq) or isinstance(delta_in.mesh, MeshImTime), 'input delta_in should have a mesh MeshImFreq or MeshImTime'

    # enforce hermiticity
    if isinstance(delta_in.mesh, MeshImFreq):
        delta_in << make_hermitian(delta_in)
    # prepare discretized delta with same mesh
    delta_disc = delta_in.copy()
    delta_disc.zero()

    def unflatten(x):
        # first half of parameters are hoppings, second half are bath energies
        if cmplx:
            V = x[0:2*Nb*n_orb].view(complex).reshape(n_orb, Nb)
            e = x[2*Nb*n_orb:]
        else:
            V = x[0:Nb*n_orb].reshape(n_orb, Nb)
            e = x[Nb*n_orb:]
        return V, e

    ####
    # define minimizer for scipy
    def minimizer(parameters):
        V, e = unflatten(parameters)

        # build discretized bath function as
        # delta = sum_i V_ji^* f(eps_i) * V_ik  with
        #     f = [(w - eps_i)]^-1
        # for Matsubara and
        #     f = -exp(-tau * eps_i) / [1 + exp(-beta * eps_i)]
        # for imaginary time.
        # delta has shape (Nmesh, Norb, Nb)
        if isinstance(delta_in.mesh, MeshImFreq):
            one_fermion = 1/(mesh_values[:,None] - e[None,:])
        elif isinstance(delta_in.mesh, MeshImTime):
            one_fermion = -np.exp(-mesh_values[:,None] * e[None,:]) / (1. + np.exp(-delta_in.mesh.beta * e[None,:]))
        delta = np.dot(V[None,:,:] * one_fermion[:,None,:], V.conj().T)

        # if Gf is scalar values we have to squeeze the add axis
        if len(delta_in.target_shape)==0:
            delta_disc.data[opt_idx:] = delta.squeeze()
        else:
            delta_disc.data[opt_idx:] = delta

        # calculate norm
        norm = np.linalg.norm(delta_disc.data[opt_idx:] - delta_in.data[opt_idx:])/np.sqrt(len(mesh_values))

        return norm
    ####

    if len(delta_in.target_shape)==0:
        n_orb = 1
    else:
        n_orb = delta_in.target_shape[0]

    # only optimize pos frequencies
    if isinstance(delta_in.mesh, MeshImFreq) and not delta_disc.mesh.positive_only():
        opt_idx = len(delta_disc.mesh)//2
    else:
        opt_idx = 0

    mesh_values = np.array([w.value for w in delta_disc.mesh][opt_idx:])

    # initialize bath_hoppings
    # create bath hoppings V with dim (Nb)
    if isinstance(V0, np.ndarray):
        assert V0.shape == (n_orb, Nb), 'V0 shape is incorrect. Must be ({},{}), but is {}'.format(n_orb, Nb, V0.shape)
    else:
        V0 = V0*np.ones((n_orb, Nb), dtype=complex if cmplx else float)

    # bath energies are initialized as linspace over the approximate bandwidth or given as list
    if (isinstance(eps0, list) or isinstance(eps0, np.ndarray)):
        assert len(eps0) == Nb, 'len(eps) does not match number of bath sides'
    else:
        eps0 = np.linspace(-eps0, eps0, Nb)

    # parameters for scipy must be a 1D array
    parameters = np.concatenate([V0.view(float).flatten(), eps0])

    # run the minimizer with method Nelder-Mead and optimize the hoppings and energies to given
    # tolerance
    start_time = timer()
    result = minimize(minimizer, parameters, method='Nelder-Mead', options = {'xatol' : tol, 'maxiter' : maxiter, 'adaptive': True})

    if result.success:
         print('optimization finished in {:.2f} s after {} iterations with norm {:.3e}'.format(timer()-start_time, result.nit, result.fun))
    else:
        raise RuntimeError('optimization has failed, scipy minimize signaled no success: {}'.format(result.message))
    # results
    V_opt, eps_opt = unflatten(result.x)

    if opt_idx > 0:
        delta_disc.data[:opt_idx] = delta_disc.data[opt_idx:].conj()[::-1]

    # sort by energy
    order = np.argsort(eps_opt)
    eps_opt = eps_opt[order]
    V_opt = V_opt[:,order]

    return V_opt, eps_opt, delta_disc
