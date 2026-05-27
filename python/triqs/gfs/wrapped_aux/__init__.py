"""Internal C++ dispatchers backing :class:`~triqs.gfs.gf.Gf`.

This package is an implementation detail of the Green's function
Python bindings — not part of the public API. It groups the
``CallProxy`` classes that implement ``Gf.__call__`` (sharded by
target rank to keep template-instantiation cost manageable) together
with a few low-level helpers used internally by the pure-Python
:class:`~triqs.gfs.gf.Gf` class.

User code should access these features through :class:`triqs.gfs.gf.Gf`
and the functions in :mod:`triqs.gfs.gf_fnt` / :mod:`triqs.gfs.gf_factories`.
"""

from .wrapped_aux_target_rank_0 import *
from .wrapped_aux_target_rank_1 import *
from .wrapped_aux_target_rank_2 import *
from .wrapped_aux_target_rank_3 import *
from .wrapped_aux_target_rank_4 import *
from .wrapped_aux_other import _set_from_gf_data_mul_LR, _make_gf_from_real_gf
