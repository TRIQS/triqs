"""
Generate wrapped_aux_target_rank_{0..4}.{hpp, cpp, toml} files for explicit template instantiations
of gf_proxy::operator() for each (mesh, target, argument-type) combination.

One .hpp file is generated per target rank (0=scalar, 1=vector, 2=matrix, 3/4=tensor).
Each file contains:
  - c2py_module namespace: using aliases so c2py can discover the CallProxy types
  - triqs::gfs namespace: explicit instantiations of gf_proxy::operator()<ReturnType>(args...)
"""

from collections import defaultdict
from itertools import product

# ---------------------------------------------------------------------------
# Configuration tables
# ---------------------------------------------------------------------------

# Valid call argument types for each mesh
VALID_ARGS = {
    'imfreq':       ['long', 'matsubara_freq'],
    'imtime':       ['long', 'double'],
    'refreq':       ['long', 'double'],
    'retime':       ['long', 'double'],
    'legendre':     ['long', 'double'],
    'dlr':          ['long', 'double', 'matsubara_freq'],
    'chebyshev':    ['long', 'double'],
    'brzone':       ['std::array<long,3>', 'std::array<double, 3>'],
    'cyclat':       ['std::array<long,3>', 'triqs::lattice::bravais_lattice::point_t'],
    'fourier_poly': ['long', 'std::array<double, 3>'],
}

# Optional per-rank overrides of VALID_ARGS. If a mesh/rank isn't listed here,
# VALID_ARGS[mesh] is used.
VALID_ARGS_PER_RANK = {}

# (rank, target C++ type, return C++ type) for each target rank
TARGETS = [
    (0, 'scalar_valued',    'dcomplex'),
    (1, 'tensor_valued<1>', 'array<dcomplex, 1>'),
    (2, 'matrix_valued',    'matrix<dcomplex>'),
    (3, 'tensor_valued<3>', 'array<dcomplex, 3>'),
    (4, 'tensor_valued<4>', 'array<dcomplex, 4>'),
]

# Lattice meshes form product meshes only with non-lattice meshes (and vice versa)
LATTICE_MESHES = {'brzone', 'cyclat'}

# Meshes that only live as single-mesh GFs (no product with another mesh).
# Kept as its own set so future additions don't overload LATTICE_MESHES.
SINGLE_ONLY_MESHES = {'fourier_poly'}

# C++ mesh name -> Python class name component (e.g. 'imfreq' -> 'ImFreq')
MESH_TO_PYNAME = {
    'imfreq': 'ImFreq', 'imtime': 'ImTime', 'refreq': 'ReFreq', 'retime': 'ReTime',
    'legendre': 'Legendre', 'dlr': 'DLR', 'chebyshev': 'Chebyshev',
    'brzone': 'BrZone', 'cyclat': 'CycLat',
    'fourier_poly': 'FourierPoly',
}


def real_valued(target):
    """Convert e.g. 'scalar_valued' -> 'scalar_real_valued'."""
    return target.replace("_", "_real_", 1)


def mesh_pyname(mesh):
    """Map a C++ mesh (possibly product) to its Python name for CallProxy classes."""
    if mesh.startswith('prod<'):
        # 'prod<brzone,imfreq>' -> 'BrZone_x_ImFreq'
        inner = mesh[len('prod<'):-1]
        m1, m2 = (s.strip() for s in inner.split(','))
        return f"{MESH_TO_PYNAME[m1]}_x_{MESH_TO_PYNAME[m2]}"
    return MESH_TO_PYNAME[mesh]


# ---------------------------------------------------------------------------
# Build the list of (mesh, return_types, rank, target, arg_types) per rank.
#
# For single meshes: one entry per (rank, target, mesh) with the mesh's valid args.
#   - imtime additionally gets a real_valued variant.
#
# For product meshes prod<M1,M2>: only when exactly one of M1,M2 is a lattice mesh.
#   The argument list covers all (x,y) pairs plus partial-evaluation with 'all_t'.
#   - If either component is imtime, a real_valued variant is also generated.
# ---------------------------------------------------------------------------

calls_by_rank = defaultdict(list)

meshes = list(VALID_ARGS)

def args_for(mesh, rank):
    return VALID_ARGS_PER_RANK.get(mesh, {}).get(rank, VALID_ARGS[mesh])


for rank, target, return_t in TARGETS:
    for m1 in meshes:
        xs = args_for(m1, rank)

        # Single-mesh entries
        calls_by_rank[rank].append((m1, [return_t] * len(xs), rank, target, xs))
        if m1 == 'imtime':
            calls_by_rank[rank].append((m1, [return_t] * len(xs), rank, real_valued(target), xs))

        # Product-mesh entries: exactly one of (m1, m2) must be a lattice mesh.
        # Single-only meshes don't participate in products.
        if m1 in SINGLE_ONLY_MESHES:
            continue
        for m2 in meshes:
            if m2 in SINGLE_ONLY_MESHES:
                continue
            m1_is_lat = m1 in LATTICE_MESHES
            m2_is_lat = m2 in LATTICE_MESHES
            if not (m1_is_lat ^ m2_is_lat):
                continue

            ys = args_for(m2, rank)
            prod_mesh = f"prod<{m1},{m2}>"

            # Full evaluation: all (x, y) combinations
            # Partial evaluation: fix one index to 'all_t', vary the other
            arg_list = (
                [(x, y) for x, y in product(xs, ys)]
                + [('all_t', y) for y in ys]
                + [(x, 'all_t') for x in xs]
            )
            ret_list = (
                [return_t] * (len(xs) * len(ys))
                + [f"gf<{m1}, {target}>"] * len(ys)
                + [f"gf<{m2}, {target}>"] * len(xs)
            )
            calls_by_rank[rank].append((prod_mesh, ret_list, rank, target, arg_list))

            if 'imtime' in (m1, m2):
                real_target = real_valued(target)
                ret_list_real = (
                    [return_t] * (len(xs) * len(ys))
                    + [f"gf<{m1}, {real_target}>"] * len(ys)
                    + [f"gf<{m2}, {real_target}>"] * len(xs)
                )
                calls_by_rank[rank].append((prod_mesh, ret_list_real, rank, real_target, arg_list))

# ---------------------------------------------------------------------------
# Generate one .hpp, .cpp, and .toml per rank
# ---------------------------------------------------------------------------

for rank in range(5):
    # Generate .toml
    with open(f"wrapped_aux_target_rank_{rank}.toml", "w") as f:
        f.write('package_name = "triqs.gfs"\n')
        f.write('documentation = ""\n')
        f.write('namespaces = "triqs::gfs"\n')
        f.write('match_names = "gf_proxy"\n')

    # Generate .cpp
    with open(f"wrapped_aux_target_rank_{rank}.cpp", "w") as f:
        f.write("// This file is automatically generated by wrapped_aux.inst.py. Do not edit!\n")
        f.write("#include <c2py/c2py.hpp>\n")
        f.write("#include <nda/c2py/converters.hpp>\n")
        f.write("#include <triqs/c2py_converters/gf.hpp>\n")
        f.write("#include <triqs/c2py_converters/lattice.hpp>\n")
        f.write("#include <triqs/c2py_converters/mesh.hpp>\n")
        f.write(f'#include "./wrapped_aux_target_rank_{rank}.hpp"\n')
        f.write("\n")
        # Pull the clair-c2py-generated Python bindings into this TU so they
        # compile alongside the explicit gf_proxy::operator() instantiations.
        f.write(f'#include "wrapped_aux_target_rank_{rank}.wrap.cxx"\n')

    # Generate .hpp
    with open(f"wrapped_aux_target_rank_{rank}.hpp", "w") as f:
        f.write("// This file is automatically generated by wrapped_aux.inst.py. Do not edit!\n")
        f.write("#pragma once\n\n")
        f.write('#include "./gf_proxy.hpp"\n')
        f.write('#include <triqs/lattice.hpp>\n')
        f.write('#include <triqs/mesh.hpp>\n\n')
        f.write('using namespace triqs::arrays;\n')
        f.write('using namespace triqs::gfs;\n\n')

        # Type aliases for c2py type discovery
        f.write("namespace c2py_module {\n\n")
        for mesh, _, _, target, _ in calls_by_rank[rank]:
            c_type = f"gf_proxy<gf_view<{mesh},{target}>>"
            py_name = mesh_pyname(mesh)
            real_suffix = "_R" if "_real_" in target else ""
            f.write(f"using CallProxy{py_name}_{rank}{real_suffix} = triqs::gfs::{c_type};\n")
        f.write("\n} // namespace c2py_module\n\n")

        # Explicit template instantiations
        f.write("namespace triqs::gfs {\n\n")
        for mesh, ret_types, _, target, arg_types in calls_by_rank[rank]:
            c_type = f"gf_proxy<gf_view<{mesh},{target}>>"
            for arg, ret in zip(arg_types, ret_types):
                if isinstance(arg, tuple):
                    args = ', '.join(f'{t} const&' for t in arg)
                else:
                    args = f"{arg} const&"
                f.write(f"template {ret} {c_type}::operator()<{ret}>({args});\n")
        f.write("\n} // namespace triqs::gfs\n")
