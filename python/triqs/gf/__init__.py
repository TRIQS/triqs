import warnings
import sys

warnings.warn(
    "The triqs.gf module has been renamed to triqs.gfs. "
    "Please update your imports. triqs.gf will be removed in a future version.",
    FutureWarning, stacklevel=2
)

import triqs.gfs
from triqs.gfs import *
from triqs.gfs import __all__

# Alias all submodules so triqs.gf.<submodule> imports still work
import importlib
for _name in [
    'gf', 'block_gf', 'block2_gf', 'map_block',
    'descriptor_base', 'descriptors', 'lazy_expressions',
    'tools', 'dlr_crm_dyson_solver',
    'plot', 'backwd_compat', 'gf_fnt', 'gf_factories', 'wrapped_aux',
]:
    importlib.import_module(f'triqs.gfs.{_name}')
    sys.modules[f'triqs.gf.{_name}'] = getattr(triqs.gfs, _name)

# Also alias nested backwd_compat submodules
for _name in ['gf_imfreq', 'gf_imtime', 'gf_refreq', 'gf_retime', 'gf_legendre']:
    importlib.import_module(f'triqs.gfs.backwd_compat.{_name}')
    sys.modules[f'triqs.gf.backwd_compat.{_name}'] = getattr(triqs.gfs.backwd_compat, _name)

# Alias triqs.gf.meshes -> triqs.mesh and its submodules
import triqs.mesh
sys.modules['triqs.gf.meshes'] = triqs.mesh
for _name in ['mesh_product', 'mesh_point', 'matsubara_freq']:
    importlib.import_module(f'triqs.mesh.{_name}')
    sys.modules[f'triqs.gf.{_name}'] = getattr(triqs.mesh, _name)

del _name
