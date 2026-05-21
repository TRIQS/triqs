import warnings
import sys
import importlib
import pkgutil

warnings.warn(
    "The triqs.gf module has been renamed to triqs.gfs. "
    "Please update your imports. triqs.gf will be removed in a future version.",
    FutureWarning, stacklevel=2
)

import triqs.gfs
import triqs.mesh


def _alias_tree(src, dst):
    """Alias src as dst, and recursively alias every submodule of src under dst."""
    sys.modules[dst] = src
    if hasattr(src, "__path__"):
        for info in pkgutil.walk_packages(src.__path__, prefix=f"{src.__name__}."):
            sys.modules[info.name.replace(src.__name__, dst, 1)] = importlib.import_module(info.name)


# Order matters — later writes to sys.modules override earlier ones.
_alias_tree(triqs.mesh, "triqs.gf")           # legacy flat aliases (triqs.gf.mesh_product, ...)
_alias_tree(triqs.gfs, "triqs.gf")            # triqs.gf is triqs.gfs
_alias_tree(triqs.mesh, "triqs.gf.meshes")    # triqs.gf.meshes is triqs.mesh (Python package, not the .so)
