# Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
# Copyright (c) 2015 Igor Krivenko
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
# Authors: Michel Ferrero, Alexander Hampel, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

r"""Pure-Python utilities operating on :class:`~triqs.gfs.gf.Gf` and 
:class:`~triqs.gfs.block_gf.BlockGf`."""

from . import lazy_expressions, descriptors, gf_fnt
from triqs.mesh import MeshImFreq, MeshLegendre, MeshImTime, MeshReFreq, MeshReTime
from .block_gf import BlockGf
from .block2_gf import Block2Gf
from .gf import Gf
from .gf_factories import make_hermitian
import numpy as np
from itertools import product
from .backwd_compat.gf_refreq import GfReFreq
from timeit import default_timer as timer


def map_block(fun, G):
    r"""Apply ``fun`` to every block of ``G`` and return the result.

    Parameters
    ----------
    fun : callable
        Function with signature ``fun(g) -> Gf`` (or returning any
        object — see "Returns" below).
    G : BlockGf or Block2Gf
        Container whose blocks ``fun`` is applied to.

    Returns
    -------
    BlockGf or Block2Gf or list
        - If ``fun`` returns :class:`~triqs.gfs.gf.Gf` instances, the 
          result is a new :class:`~triqs.gfs.block_gf.BlockGf` / 
          :class:`~triqs.gfs.block2_gf.Block2Gf` with the same block
          structure as ``G``.
        - Otherwise the per-block results are returned as a plain list
          (or list of lists for :class:`~triqs.gfs.block2_gf.Block2Gf`).

    Raises
    ------
    Exception
        If ``G`` is neither a :class:`~triqs.gfs.block_gf.BlockGf` nor a 
        :class:`~triqs.gfs.block2_gf.Block2Gf`.

    Examples
    --------
    >>> from triqs.gfs import map_block, inverse
    >>> G_inv = map_block(inverse, G_block)
    """
    if isinstance(G, BlockGf):
        block_list = [fun(bl) for name, bl in G]
        if isinstance(block_list[0], Gf):
            return BlockGf(name_list=list(G.indices), block_list=block_list)
        else:
            return block_list

    elif isinstance(G, Block2Gf):
        block_list = []
        for bn1 in G.indices1:
            block_list.append([fun(G[bn1, bn2]) for bn2 in G.indices2])
        if isinstance(block_list[0][0], Gf):
            return Block2Gf(name_list1=list(G.indices1), name_list2=list(G.indices2), block_list=block_list)
        else:
            return block_list

    else:
        raise Exception('map_block only applicable for BlockGf and Block2Gf')


def inverse(x):
    r"""Element-wise inverse with lazy-expression support.

    Parameters
    ----------
    x : Gf, BlockGf, LazyExpr or numeric
        Object to invert. For a :class:`~triqs.gfs.gf.Gf` / 
        :class:`~triqs.gfs.block_gf.BlockGf` this calls ``x.inverse()`` 
        (target-space matrix inverse at every mesh point); for a lazy 
        expression a new lazy node is returned; for a plain scalar 
        ``1.0 / x``.

    Returns
    -------
    Same type as ``x``
        The inverse.

    Examples
    --------
    >>> from triqs.gfs import inverse, iOmega_n
    >>> Sigma << iOmega_n - inverse(G_iw)
    """
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("inverse", inverse) (x)
    if hasattr(x,'inverse'):
        return x.inverse()
    try:
        return 1.0 / x
    except TypeError as e:
        raise TypeError(
            f"inverse(): cannot invert object of type {type(x).__name__}: {x!r}"
        ) from e

def conjugate(x):
    r"""Complex conjugate with lazy-expression support.

    Parameters
    ----------
    x : Gf, BlockGf or LazyExpr
        Object to conjugate. Must expose a ``conjugate()`` method
        unless it is a lazy expression.

    Returns
    -------
    Same type as ``x``
        :math:`G^*`.
    """
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("conjugate", conjugate) (x)
    assert hasattr(x,'conjugate')
    return x.conjugate()

def transpose(x):
    r"""Transpose in target space with lazy-expression support.

    Parameters
    ----------
    x : Gf, BlockGf or LazyExpr
        Object to transpose. Must expose a ``transpose()`` method
        unless it is a lazy expression.

    Returns
    -------
    Same type as ``x``
        :math:`G^T` (mesh axes untouched).
    """
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("transpose", transpose) (x)
    assert hasattr(x,'transpose')
    return x.transpose()


def delta(g):
    r"""Hybridization function :math:`\Delta(i\omega_n)` extracted from 
    a non-interacting Green's function.

    Computes :math:`\Delta = i\omega_n - G_0^{-1}` and removes the
    fitted constant tail so that :math:`\Delta(i\omega_n) \to 0` at
    large frequencies.

    Parameters
    ----------
    g : Gf or BlockGf
        Non-interacting Green's function :math:`G_0` on a Matsubara
        mesh, either matrix-valued or scalar-valued.

    Returns
    -------
    Gf or BlockGf
        Hybridization function on the same mesh as :math:`G_0`.

    Notes
    -----
    Assumes:

    * the diagonal components of :math:`G_0` decay as :math:`1/i\omega_n`;
    * :math:`G_0` is Hermitian, i.e.
      :math:`G_0(i\omega_n)_{ij} = \overline{G_0(-i\omega_n)_{ji}}`.

    Prints a warning if the tail fit residual exceeds ``1e-5``.

    Examples
    --------
    >>> from triqs.gfs import delta
    >>> Delta_iw = delta(G0_iw)
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
    r"""Solve Dyson's equation for the missing quantity.

    Given exactly two of :math:`G_0`, :math:`G` and :math:`\Sigma`,
    return the third using

    .. math::

        G^{-1} = G_0^{-1} - \Sigma.

    Parameters
    ----------
    G0_iw : Gf or BlockGf, optional
        Non-interacting Green's function.
    G_iw : Gf or BlockGf, optional
        Interacting Green's function.
    Sigma_iw : Gf or BlockGf, optional
        Self-energy.

    Returns
    -------
    Gf or BlockGf
        The remaining quantity, of the same type as the inputs.

    Raises
    ------
    ValueError
        If fewer or more than two of the three keyword arguments are
        provided.

    Examples
    --------
    >>> from triqs.gfs import dyson
    >>> Sigma_iw = dyson(G0_iw=G0_iw, G_iw=G_iw)
    >>> G_iw    = dyson(G0_iw=G0_iw, Sigma_iw=Sigma_iw)
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
    r"""Read a real-frequency Green's function block from text files.

    Each text file must have three columns ``(w, Re(G), Im(G))``;
    multiple files are combined into a matrix-valued
    :class:`~triqs.gfs.backwd_compat.gf_refreq.GfReFreq`.

    Parameters
    ----------
    block_txtfiles : 2D array-like of str
        Rank-2 array of file names, one per matrix element of the
        target space, e.g. ``[['up_eg1.dat']]`` for a 1x1 block or
        ``[['11.dat', '12.dat'], ['21.dat', '22.dat']]`` for a 2x2
        block.
    block_name : str
        Name attached to the returned Green's function.

    Returns
    -------
    GfReFreq
        Matrix-valued real-frequency Green's function read from disk.

    Notes
    -----
    * The mesh must be identical across files.
    * Non-uniform meshes are not supported.
    * A :class:`~triqs.gfs.block_gf.BlockGf` must be assembled manually 
      from multiple :class:`~triqs.gfs.backwd_compat.gf_refreq.GfReFreq` 
      objects if desired.

    Examples
    --------
    >>> g_up = read_gf_from_txt([['up_11.dat', 'up_12.dat'],
    ...                          ['up_21.dat', 'up_22.dat']], 'up')
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
    r"""Write a frequency Green's function to text files.

    Writes one file per matrix element of the target space with three
    columns ``(w, Re(G), Im(G))`` (real frequency) or
    ``(omega_n, Re(G), Im(G))`` (Matsubara). File names follow the
    pattern ``"{g.name}_{i}_{j}.dat"``.

    Parameters
    ----------
    g : Gf
        Real- or Matsubara-frequency Green's function with a
        matrix-valued target space.

    Raises
    ------
    ValueError
        If ``g.mesh`` is neither :class:`~triqs.mesh.meshes.MeshReFreq` 
        nor :class:`~triqs.mesh.meshes.MeshImFreq`.
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
    r"""Container for high-frequency tail coefficients, initialized to zero.

    Parameters
    ----------
    g : Gf or BlockGf
        Green's function on a frequency or time mesh for which the tail
        is constructed.
    n_moments : int, optional
        Number of tail moments :math:`G_n` (orders ``0 ... n_moments-1``).
        Default ``10``.

    Returns
    -------
    numpy.ndarray or BlockGf-like
        Zero array of shape ``(n_moments, *g.target_shape)`` for a
        single :class:`~triqs.gfs.gf.Gf`; a block-wise container for a
        :class:`~triqs.gfs.block_gf.BlockGf`.

    Raises
    ------
    RuntimeError
        If ``g`` is not a frequency or time Green's function.
    """
    if isinstance(g, Gf) and isinstance(g.mesh, (MeshImFreq, MeshReFreq, MeshImTime, MeshReTime)):
        n_moments = max(1, n_moments)
        return np.zeros((n_moments,) + g.target_shape, dtype = np.complex128)
    elif isinstance(g, BlockGf):
        return map_block(lambda g_bl: make_zero_tail(g_bl, n_moments), g)
    else:
        raise RuntimeError("Error: make_zero_tail has to be called on a frequency or time Green function object")


def fit_legendre(g_t, order=10):
    r"""Fit a (possibly noisy) imaginary-time Green's function to a 
    Legendre expansion.

    Parameters
    ----------
    g_t : Gf or BlockGf
        Matrix-valued imaginary-time Green's function (typically the
        binned output of a QMC solver).
    order : int, optional
        Maximal order of the Legendre expansion (number of coefficients
        used). Default ``10``.

    Returns
    -------
    Gf or BlockGf
        Legendre Green's function with ``order`` coefficients.

    Notes
    -----
    Only Hermiticity is imposed during the fit; discontinuities at
    :math:`\tau = 0^+` and :math:`\tau = \beta^-` must be enforced
    separately via :func:`triqs.gfs.enforce_discontinuity`.

    Original author: Hugo U.R. Strand.
    """

    import numpy.polynomial.legendre as leg

    if isinstance(g_t, BlockGf):
        return map_block(lambda g_bl: fit_legendre(g_bl, order), g_t)

    assert isinstance(g_t, Gf) and isinstance(g_t.mesh, MeshImTime), "fit_legendre expects imaginary-time Green function objects"

    # -- flatten the data to 2D N_tau x (N_orb * N_orb)

    shape = g_t.data.shape
    fshape = [shape[0], int(np.prod(shape[1:]))]

    # -- extend data accounting for hermiticity

    mesh = g_t.mesh
    tau = np.array([ t.value for t in mesh ])
    # Rescale to the interval (-1,1)
    x = 2. * tau / mesh.beta - 1.

    # -- Separated real valued linear system, with twice the number of RHS terms

    g_t_herm = make_hermitian(g_t)
    data = g_t_herm.data.reshape(fshape)
    data_ext = np.hstack((data.real, data.imag))

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
    r"""Hybridization function built from bath hoppings and energies.

    .. math::

        \Delta_{kl}(i\omega_n) = \sum_{j=1}^{N_b} V_{kj}\, S_j\, V_{jl}^*,

    where the bath propagator is

    .. math::

        S_j = \frac{1}{i\omega_n - \varepsilon_j}
        \qquad \text{(Matsubara)}
        \qquad\text{or}\qquad
        S_j = -\frac{e^{-\tau\varepsilon_j}}{1 + e^{-\beta\varepsilon_j}}
        \qquad \text{(imaginary time).}

    Parameters
    ----------
    V : numpy.ndarray, or list thereof
        Bath hopping matrix. A list of arrays produces a
        :class:`~triqs.gfs.block_gf.BlockGf`, one block per array.
    eps : list of float, or list of list of float
        Bath energies. Must match ``V`` block-by-block.
    mesh : MeshImFreq or MeshImTime
        Mesh of the resulting hybridization function.
    block_names : list of str, optional
        Names used to label the blocks when ``V`` / ``eps`` are lists.
        Defaults to ``['0', '1', ...]``.

    Returns
    -------
    Gf or BlockGf
        Hybridization function on ``mesh``.
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
        mesh_values = np.linspace(mesh(mesh.first_index), mesh(mesh.last_index), len(mesh))
        one_fermion = 1/(mesh_values[:, None] - eps[None, :])
    elif isinstance(mesh, MeshImTime):
        mesh_values = np.linspace(0, mesh.beta, len(mesh))
        one_fermion = -np.exp(-mesh_values[:, None] * eps[None, :] + mesh.beta * ((eps < 0.0) * eps)
                              [None, :]) / (1. + np.exp(-mesh.beta * np.abs(eps[None, :])))

    delta_res.data[:] = np.einsum('wkj, jl -> wkl', V[None, :, :] * one_fermion[:, None, :], V.conj().T)

    return delta_res


def discretize_bath(delta_in, Nb, eps0=3, V0=None, tol=1e-15, maxiter=10000,
                    cmplx=False, method='BFGS'):
    r"""Fit a hybridization function with ``Nb`` discrete bath sites.

    The discretized hybridization

    .. math::

        \Delta_{kl}^{\mathrm{disc}}(i\omega_n)
        = \sum_{j=1}^{N_b} V_{kj}\, S_j\, V_{jl}^*,
        \qquad
        S_j = \begin{cases}
            (i\omega_n - \varepsilon_j)^{-1}, & \text{Matsubara} \\
            -e^{-\tau\varepsilon_j}/(1+e^{-\beta\varepsilon_j}), &
                \text{imaginary time}
        \end{cases}

    is fitted to ``delta_in`` by minimizing the norm

    .. math::

        \biggl[\frac{1}{N}
            \sum_{n=1}^{N}
            \bigl|\Delta^{\mathrm{disc}}(i\omega_n) - \Delta(i\omega_n)\bigr|^2
        \biggr]^{1/2}

    (or the analogous imaginary-time expression) using
    :func:`scipy.optimize.minimize` /
    :func:`scipy.optimize.basinhopping`.

    Parameters
    ----------
    delta_in : Gf or BlockGf
        Matsubara or imaginary-time hybridization function to discretize.
    Nb : int
        Number of bath sites per Gf block.
    eps0 : float or list of float, optional
        Approximate bandwidth (used to seed bath energies on a linspace)
        or explicit initial guess for the bath energies. Default ``3.0``.
    V0 : float, complex or numpy.ndarray, optional
        Initial guess for ``V``:

        * scalar — broadcast to every element of ``V``.
        * 2D array — used as is.
        * ``None`` (default) — derive from the Cholesky factor of the
          leading moment of ``delta_in``
          (:math:`\lim_{\omega\to\infty} i\omega\,\Delta(i\omega)`, or
          :math:`-\Delta(0^+) - \Delta(\beta^-)` for imaginary time).
    tol : float, optional
        Optimizer tolerance (``ftol`` / ``xatol``). Default ``1e-15``.
    maxiter : int, optional
        Maximum number of optimization steps. Default ``10000``.
    cmplx : bool, optional
        If ``True``, optimize complex hoppings. Default ``False``.
    method : {'BFGS', 'Nelder-Mead', 'basinhopping'}, optional
        Optimization method. Default ``'BFGS'`` (uses ``L-BFGS-B``
        internally).

    Returns
    -------
    V_opt : numpy.ndarray or list thereof
        Optimized bath hoppings.
    eps_opt : list of float or list of list of float
        Optimized bath energies, sorted in ascending order.
    delta_disc : Gf or BlockGf
        Discretized hybridization function on the mesh of ``delta_in``.
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
            V   = x[0:2*Nb*n_orb].view(complex).reshape(n_orb, Nb)
            eps = x[2*Nb*n_orb:]
        else:
            V   = x[0:Nb*n_orb].reshape(n_orb, Nb)
            eps = x[Nb*n_orb:]
        return V, eps

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
        # col by the sqrt(#occurrences)
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

    delta_disc = make_delta(V_opt, eps_opt, delta_in.mesh)

    # if Gf is scalar-valued we have to squeeze the trivial axes
    if len(delta_in.target_shape) == 0:
        delta_disc = delta_disc[0, 0]

    return V_opt, eps_opt, delta_disc
