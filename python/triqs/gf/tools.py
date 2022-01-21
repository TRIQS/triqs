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


def make_delta(V, eps, mesh, block_names=None):
    """
    Create a hybridization function from given hoppings and bath energies
    as
    .. math:: \Delta_{kl}^{disc} (i \omega_n) = \sum_{j=1}^{Nb} V_{kj} S V_{jl}^* .
    where S is either
    .. math:: [i \omega_n - eps_j]^{-1}
    for MeshImFreq or
    .. math:: - exp(-tau * eps_j) / (1 + exp(-\beta * eps_j) )
    for MeshImTime

    Parameters
    -----------
    V : np.array (shape Norb x NB) or list thereof
            Bath hopping matrix or matrix for each Gf block
    eps : list(float) or list(list(float))
            Bath energies or energies for each Gf block
    mesh : MeshImFreq or MeshImTime
            Mesh of the hybridization function
    block_names : list(str)
            List of block names, used if V, eps are lists

    Returns
    -------
    delta : Gf or BlockGf
            Hybridization function on given mesh

    """

    if isinstance(V, list):

        if block_names is None:
            block_names = [str(i) for i in range(len(V))]

        assert isinstance(block_names, list), 'block_names should be a list(str)'
        assert len(V) == len(eps) and len(V) == len(block_names), \
            'mismatch between list size of V, eps and block_names'

        delta_list = [make_delta(v, e, mesh) for v, e in zip(V, eps)]
        return BlockGf(name_list=block_names, block_list=delta_list)

    assert V.shape[1] == len(eps), 'number of bath sides in V and eps does not match'

    delta_res = Gf(mesh=mesh, target_shape=[V.shape[0], V.shape[0]])

    if isinstance(mesh, MeshImFreq):
        # only optimize pos frequencies
        opt_idx = len(mesh)//2 if mesh.positive_only() else 0
        mesh_values = np.linspace(mesh(mesh.first_index()), mesh(mesh.last_index()), len(mesh))[opt_idx:]
        one_fermion = 1/(mesh_values[:, None] - eps[None, :])
    elif isinstance(mesh, MeshImTime):
        mesh_values = np.linspace(0, mesh.beta, len(mesh))
        opt_idx = 0
        one_fermion = -np.exp(-mesh_values[:, None] * eps[None, :] + mesh.beta * ((eps < 0.0) * eps)
                              [None, :]) / (1. + np.exp(-mesh.beta * np.abs(eps[None, :])))

    delta_res.data[opt_idx:] = np.einsum('wkj, jl -> wkl', V[None, :, :] * one_fermion[:, None, :], V.conj().T)

    if opt_idx > 0:
        delta_res.data[:opt_idx] = delta_res.data[opt_idx:].conj()[::-1]

    return delta_res


def discretize_bath(delta_in, Nb, eps0=3, V0=None, tol=1e-15, maxiter=10000,
                    cmplx=False, method='BFGS'):
    """
    Discretize a given hybridization function using Nb bath sites.

    The discretized hybridization is constructed as
    .. math:: \Delta_{kl}^{disc} (i \omega_n) = \sum_{j=1}^{Nb} V_{kj} S  V_{jl}^* .
    where S is either:
    .. math:: [i \omega_n - eps_j]^{-1}
    for MeshImFreq or
    .. math:: - exp(-tau * eps_j) / (1 + exp(-\beta * eps_j) )
    for MeshImTime.

    The hoppings V and energies eps are chosen to minimize the norm
    .. math:: \left[ \frac{1}{\sqrt(N)} \sum_{i \omega_n}^{N} | \Delta^{disc} (i \omega_n) - \Delta (i \omega_n) |^2 \right]^{\frac{1}{2}}
    and for MeshImTime
    .. math:: \left[ \frac{1}{\sqrt(N)} \sum_{\tau}^{N} | \Delta^{disc} (\tau) - \Delta (\tau) |^2 \right]^{\frac{1}{2}}
    This minimization is performed using scipy.optimize.minimize with the
    Nelder-Mead algorithm with the given tolerance.

    Parameters
    ----------
    delta_in : Gf or BlockGf
        Matsubara or imaginary-time hybridization function to discretize
    Nb : int
        Number of bath sites per Gf block
    eps0: float or list(float), default=3.0
        Approximate bandwith or initial guesses for bath energies.
    V0 : float or np.ndarray (shape norb X Nb), optional
        If float: initial guess used for all hopping values.
        If np.ndarray: initial guess for V.
        Otherwise use the cholesky decomposition of
        .. math:: \lim_{\omega->\infty} i\omega*\Delta(i\omega)
        or
        .. math:: -\Delta(\tau=0^+) - \Delta(\tau=\beta^-)
        to obtain an initial guess for V.
    tol : float, default=1e-15
        Tolerance for scipy minimize on data to optimize (xatol / ftol)
    maxiter : int, default=10000
        Maximum number of optimization steps
    complx : bool, default=False
        Allow the hoppings V to be complex
    method : string, default=BFGS
        Method for minimizing the function in scipy minimize

    Returns
    -------
    V_opt : np.array (shape norb x Nb) or list thereof
        Optimized bath hoppings
    eps_opt : list(float) or list(list(float)
        Optimized bath energies (sorted)
    delta_disc : Gf or BlockGf
        Discretized hybridization function
    """
    from scipy.optimize import minimize, basinhopping
    if isinstance(delta_in, BlockGf):
        V_opt, eps_opt, delta_list = [], [], []
        for j, (block, delta) in enumerate(delta_in):
            res = discretize_bath(delta, Nb,
                                  eps0[j] if isinstance(eps0, list) else eps0,
                                  V0[j] if isinstance(V0, list) else V0,
                                  tol, maxiter, cmplx, method)
            V_opt.append(res[0])
            eps_opt.append(res[1])
            delta_list.append(res[2])

        return V_opt, eps_opt, BlockGf(name_list=list(delta_in.indices), block_list=delta_list)

    # some tests if input is okay
    assert isinstance(delta_in.mesh, MeshImFreq) or isinstance(delta_in.mesh, MeshImTime), 'input delta_in should have a mesh MeshImFreq or MeshImTime'

    if isinstance(delta_in.mesh, MeshImFreq):
        assert delta_in.is_gf_real_in_tau()

    # enforce hermiticity
    delta_in << make_hermitian(delta_in)

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
        V, eps = unflatten(parameters)

        # Build discretized bath function
        delta_disc = make_delta(V, eps, delta_in.mesh)

        # if Gf is scalar-valued we have to squeeze the trivial axes
        if len(delta_in.target_shape) == 0:
            delta_disc = delta_disc[0, 0]

        # calculate norm
        norm = np.linalg.norm(delta_disc.data - delta_in.data)/np.sqrt(len(delta_disc.mesh))
        return norm
    ####

    if len(delta_in.target_shape) == 0:
        n_orb = 1
    else:
        n_orb = delta_in.target_shape[0]

    # initialize bath_hoppings
    # create bath hoppings V with dim (Nb)
    if isinstance(V0, np.ndarray):
        assert V0.shape == (n_orb, Nb), 'V0 shape is incorrect. Must be ({},{}), but is {}'.format(n_orb, Nb, V0.shape)
    elif isinstance(V0, (float, complex)):
        if isinstance(V0, complex) and not cmplx:
            raise ValueError('V0 initialized with a complex value, but cmplx=False')
        V0 = V0*np.ones((n_orb, Nb))
    elif V0 is None:
        print('initial guess of V from cholesky decomposition of leading order moment of delta_in')
        # get 1st moment of delta_in
        if isinstance(delta_in.mesh, MeshImFreq):
            known_moments = make_zero_tail(delta_in, n_moments=1)
            delta_in.mesh.set_tail_fit_parameters(tail_fraction=0.3)
            tail, err = delta_in.fit_hermitian_tail(known_moments=known_moments)
            leading_moment = tail[1]
        else:
            leading_moment = -delta_in.data[0, ...]-delta_in.data[-1, ...]
        # obtain guess from cholesky decomposition of 1st moment (tail[1])
        chol = np.linalg.cholesky(leading_moment)
        # chol always returns complex arrays
        if not cmplx:
            chol = chol.real
        # chol has shape n_orb x n_orb. We repeat columns
        # of chol until V matrix is filled and normalize each
        # col by the sqrt(#occurances)
        col_idxs = [i % n_orb for i in range(Nb)]
        V0 = np.block([chol[:, i:i+1] / np.sqrt(col_idxs.count(i)) for i in col_idxs])
    else:
        raise ValueError('V0 has invalid type {}, should be one of: None, float, complex, or np.ndarray'.format(type(V0)))

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
    if method == 'BFGS':
        result = minimize(minimizer, parameters, method='L-BFGS-B',
                          options={'ftol': tol, 'gtol': 1e-15, 'maxiter': maxiter, "disp": False, "maxfun": maxiter})
    elif method == 'basinhopping':
        result = basinhopping(minimizer, parameters, niter_success=30, niter=maxiter, disp=False, stepsize=0.8,
                              minimizer_kwargs={'method': 'L-BFGS-B',
                                                'options': {'ftol': tol, 'gtol': 1e-15, "disp": False, "maxfun": 10000000}})\
                              .lowest_optimization_result
    elif method == 'Nelder-Mead':
        result = minimize(minimizer, parameters, method='Nelder-Mead', options={'xatol': tol, 'maxiter': maxiter, 'adaptive': True})
    else:
        raise ValueError('method for minimizer not recognized')

    print('optimization finished in {:.2f} s after {} iterations with norm {:.3e}'.format(timer()-start_time, result.nit, result.fun))

    if not result.success:
        print('optimization finished, but scipy minimize signaled no success, check result: {}'.format(result.message))
    # results
    V_opt, eps_opt = unflatten(result.x)

    # sort by energy
    order = np.argsort(eps_opt)
    eps_opt = eps_opt[order]
    V_opt = V_opt[:, order]

    return V_opt, eps_opt, make_delta(V_opt, eps_opt, delta_in.mesh)
