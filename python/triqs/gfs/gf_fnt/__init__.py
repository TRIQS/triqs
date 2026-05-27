"""Free functions operating on a :class:`~triqs.gfs.gf.Gf` or :class:`~triqs.gfs.block_gf.BlockGf`.

The functions in this package are generated from the C++ library and
injected as methods into :class:`~triqs.gfs.gf.Gf` via the
:class:`~triqs.gfs.gf.AddMethod` metaclass; the standalone bindings are 
also re-exported here.
"""

from .gf_fnt_fit_and_density import *
from .gf_fnt_misc import *
from .gf_fnt_fourier import *
