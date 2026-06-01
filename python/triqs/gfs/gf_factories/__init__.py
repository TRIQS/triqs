"""Factory functions that build a new :class:`~triqs.gfs.gf.Gf` or :class:`~triqs.gfs.block_gf.BlockGf`.

Each factory returns a freshly allocated Green's function; the input
is left unchanged. For in-place transforms see :mod:`triqs.gfs.gf_fnt`.
"""

from .gf_factories_hermitian import *
from .gf_factories_fourier import *
from .gf_factories_dlr_imtime import *
from .gf_factories_dlr_imfreq import *
from .gf_factories_dlr2d import *
