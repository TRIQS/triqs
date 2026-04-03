r"""Green's function for a semi-circular density of states.

.. math::

    \rho(\omega) = \frac{2}{\pi D^2}\,\sqrt{D^2 - \omega^2},
    \qquad |\omega| \le D

Closed-form expressions exist on the Matsubara and real-frequency axes.
On the imaginary-time axis a fast panel quadrature is used.

Functions
---------
g_semicirc_iw            Matsubara frequency  G(iω_n)
g_semicirc_w             Real frequency       G(ω + i0⁺)
g_semicirc_tau           Imaginary time       G(τ)   via panel quadrature
g_semicirc_tau_adapquad  Imaginary time       G(τ)   via adaptive quadrature
"""

import numpy as np
from scipy.special import roots_jacobi, roots_legendre
from scipy.integrate import quad


# ── Matsubara frequency ─────────────────────────────────────────────────────

def g_semicirc_iw(iw, D):
    r"""Semi-circular Green's function on the Matsubara axis.

    .. math::
        G(i\omega) = \frac{2}{D^2}\bigl(i\omega
            - i\,\mathrm{sign}(\omega)\,\sqrt{D^2 + \omega^2}\bigr)

    Parameters
    ----------
    iw : complex or array_like
        Matsubara frequencies (purely imaginary, e.g. ``1j * wn``).
    D  : float
        Half-bandwidth.

    Returns
    -------
    G : complex or ndarray
    """
    iw = np.asarray(iw, dtype=complex)
    return (2.0 / D**2) * (iw - 1j * np.sign(iw.imag) * np.sqrt(D**2 + (iw / 1j)**2))


# ── Real frequency ──────────────────────────────────────────────────────────

def g_semicirc_w(w, D):
    r"""Retarded semi-circular Green's function on the real-frequency axis.

    .. math::
        G^R(\omega) = \frac{2}{D^2}
        \begin{cases}
            \omega - i\sqrt{D^2 - \omega^2}, & |\omega| < D \\
            \omega - \mathrm{sign}(\omega)\sqrt{\omega^2 - D^2}, & |\omega| \ge D
        \end{cases}

    Parameters
    ----------
    w : float or array_like
        Real frequencies.
    D : float
        Half-bandwidth.

    Returns
    -------
    G : complex or ndarray
    """
    w = np.atleast_1d(np.asarray(w, dtype=float))
    G = np.empty(w.shape, dtype=complex)
    inside = np.abs(w) < D
    G[inside] = (2.0 / D**2) * (w[inside] - 1j * np.sqrt(D**2 - w[inside]**2))
    G[~inside] = (2.0 / D**2) * (w[~inside] - np.sign(w[~inside]) * np.sqrt(w[~inside]**2 - D**2))
    return G.squeeze()


# ── Imaginary time ──────────────────────────────────────────────────────────

def g_semicirc_tau(tau, beta, D, p=12, n_levels=None):
    r"""Semi-circular Green's function on the imaginary-time axis.

    Evaluates

    .. math::
        G(\tau) = -\frac{2}{\pi}\int_0^1
            \frac{e^{-\tau D\omega} + e^{(\tau-\beta)D\omega}}
                 {1 + e^{-\beta D\omega}}\,
            \sqrt{1-\omega^2}\,d\omega

    using dyadic panel quadrature on :math:`[0,1]`:

    * Gauss-Jacobi (:math:`\alpha=1/2,\,\beta_J=0`) on the panel
      :math:`[1/2,\,1]` to absorb the :math:`\sqrt{1-\omega}` singularity.
    * Gauss-Legendre on all other panels, which are dyadically refined
      toward :math:`\omega = 0`.

    Parameters
    ----------
    tau      : float or array_like
        Imaginary time(s), :math:`0 \le \tau \le \beta`.
    beta     : float
        Inverse temperature.
    D        : float
        Half-bandwidth.
    p        : int
        Quadrature order per panel (default 12).
    n_levels : int or None
        Number of dyadic refinement levels.  Default
        :math:`\lceil\log_2(\beta D)\rceil` so the smallest panel width
        :math:`2^{-n} < 1/(\beta D)`.

    Returns
    -------
    G : float or ndarray
    """
    tau = np.atleast_1d(np.asarray(tau, dtype=float))

    if n_levels is None:
        n_levels = max(1, int(np.ceil(np.log2(beta * D))))

    # Dyadic panel endpoints: [0, 2^{-n}, 2^{-(n-1)}, ..., 1/2]
    eps = np.array([0.0] + [2.0**(-k) for k in range(n_levels, 0, -1)])

    gl_x, gl_w = roots_legendre(p)
    gj_x, gj_w = roots_jacobi(p, 0.5, 0.0)

    G = np.zeros(tau.size)

    # Rightmost panel [1/2, 1]: Gauss-Jacobi absorbs sqrt(1-w).
    # Map [-1,1] -> [1/2,1]:  w = 3/4 + t/4,  1-w = (1-t)/4
    w = 0.75 + 0.25 * gj_x
    bDw = beta * D * w
    tDw = tau[:, None] * (D * w[None, :])
    K = (np.exp(-tDw) + np.exp(tDw - bDw)) / (1.0 + np.exp(-bDw))
    G -= (2.0 / np.pi) * 0.25**1.5 * ((K * np.sqrt(1.0 + w)) @ gj_w)

    # Remaining panels: Gauss-Legendre
    for i in range(n_levels):
        a, b = eps[i], eps[i + 1]
        h = (b - a) / 2.0
        m = (a + b) / 2.0
        w = m + h * gl_x
        bDw = beta * D * w
        tDw = tau[:, None] * (D * w[None, :])
        K = (np.exp(-tDw) + np.exp(tDw - bDw)) / (1.0 + np.exp(-bDw))
        G -= (2.0 / np.pi) * h * ((K * np.sqrt(1.0 - w**2)) @ gl_w)

    return G.squeeze()


# ── Imaginary time (adaptive quadrature reference) ──────────────────────────

def g_semicirc_tau_adapquad(tau, beta, D, epsabs=0.0, epsrel=1e-13):
    r"""Semi-circular Green's function on the imaginary-time axis via
    adaptive quadrature (scipy.integrate.quad).

    Evaluates the original integral

    .. math::
        G(\tau) = -\int_{-D}^{D}
            \frac{e^{-\tau\omega}}{1 + e^{-\beta\omega}}\,\rho(\omega)\,d\omega

    by splitting into :math:`[-D,0]` and :math:`[0,D]` to avoid overflow.

    Parameters
    ----------
    tau    : float or array_like
        Imaginary time(s), :math:`0 \le \tau \le \beta`.
    beta   : float
        Inverse temperature.
    D      : float
        Half-bandwidth.
    epsabs : float
        Absolute error tolerance (default 0).
    epsrel : float
        Relative error tolerance (default 1e-13).

    Returns
    -------
    G : float or ndarray
    """
    tau = np.atleast_1d(np.asarray(tau, dtype=float))
    G = np.zeros_like(tau)
    rho = lambda w: (2.0 / (np.pi * D**2)) * np.sqrt(np.clip(D**2 - w**2, 0.0, None))

    for j, t in enumerate(tau):
        v1, _ = quad(lambda w: np.exp(-t*w) / (1 + np.exp(-beta*w)) * rho(w),
                     0, D, limit=500, epsabs=epsabs, epsrel=epsrel)
        v2, _ = quad(lambda w: np.exp((beta-t)*w) / (np.exp(beta*w) + 1) * rho(w),
                     -D, 0, limit=500, epsabs=epsabs, epsrel=epsrel)
        G[j] = -(v1 + v2)

    return G.squeeze()
