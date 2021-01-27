import numpy
from math import pi
from cmath import sqrt, log
from triqs.gf import *
from triqs.gf.descriptors import Function

beta = 100  # Inverse temperature
L = 101     # Number of Matsubara frequencies used in the Pade approximation
eta = 0.01  # Imaginary frequency shift

## Test Green's functions ##

# Two Lorentzians
def GLorentz(z):
    return 0.7/(z-2.6+0.3*1j) + 0.3/(z+3.4+0.1*1j)

# Semicircle
def GSC(z):
    return 2.0*(z + sqrt(1-z**2)*(log(1-z) - log(-1+z))/pi)

# A superposition of GLorentz(z) and GSC(z) with equal weights
def G(z):
    return 0.5*GLorentz(z) + 0.5*GSC(z)

# Matsubara GF
gm = GfImFreq(indices = [0], beta = beta, name = "gm", n_points=2000)
gm << Function(G)

# Real frequency BlockGf(documentation/manual/triqs)
gr = GfReFreq(indices = [0], window = (-5.995, 5.995), n_points = 1200, name = "gr")
gr << Function(G)

# Analytic continuation of gm
g_pade = GfReFreq(indices = [0], window = (-5.995, 5.995), n_points = 1200, name = "g_pade")
g_pade.set_from_pade(gm, n_points = L, freq_offset = eta)

# Comparison plot
from triqs.plot.mpl_interface import oplot
oplot(gr[0,0], '-o', mode = 'S', name = "Original DOS")
oplot(g_pade[0,0], '-x', mode = 'S', name = "Pade-reconstructed DOS")
