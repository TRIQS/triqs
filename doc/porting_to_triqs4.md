# Porting your application to TRIQS 4.0

The porting script handles the mechanical renamings; the rest is a short checklist of
changes it cannot make. Work on a fresh branch off your TRIQS-3.3.x-compatible version and
commit after each step.

## 1. Run the porting script

```bash
wget https://raw.githubusercontent.com/TRIQS/triqs/unstable/porting_tools/port_to_triqs4
chmod u+x port_to_triqs4 && ./port_to_triqs4
```

It applies the regex-safe renamings: `triqs.gf` → `triqs.gfs` (Python module) and the
`triqs.gf.{meshes,mesh_product,mesh_point}` → `triqs.mesh` relocations, the removed
`triqs/h5.hpp` / `triqs/mpi/{base,vector}.hpp` / Matsubara-domain headers, the nda 2.0 linalg
renames (`eigenelements`→`eigh`, `eigenvalues`→`eigvalsh`, `det_and_inverse.hpp`→`det.hpp`+`inv.hpp`),
`function_arg_ret_type`→`callable_traits`, and the deprecated numpy aliases. Review the diff and commit.

## 2. Manual changes

The actual porting work — most apps only hit the first few:

- **Python bindings: cpp2py → clair + c2py** *(the big one, if your app builds its own C++
  Python modules).* Replace each `*_desc.py` with a `python/<pkg>/*.toml` (`package_name`,
  `namespaces`) plus a `*.cpp` (`namespace c2py_module` instantiation aliases, `extern template`
  for free functions); annotate headers with `C2PY_IGNORE` / `C2PY_RENAME(...)` /
  `C2PY_PROPERTY_GET(...)` as needed; build via `c2py_add_module` in CMake. Use a ported
  TRIQS-core module (e.g. `python/triqs/gfs`) as a template; the generated `*.wrap.cxx/.hxx`
  are not hand-edited. (The `triqs++` wrapper is gone — build through cmake.)
- **`gf_struct` block names must be `str`** — integer labels no longer work: `[[0, 2]]` → `[["0", 2]]`.
- **DLR meshes now symmetrize by default** (and `fit_gf_dlr`'s `symmetrize` default is `True`).
  If you use DLR, review the results — a numerical change, not a build error.
- **Removed API, if used:** `triqs.gf.map_block` (gone); `triqs::AnyOf`/`any_of`/`is_any_of`
  → `nda::AnyOf`; `mesh::index_to_freq` and `make_mesh_range_prod`.
- **CMake** — bump the project and require TRIQS 4.0 (which needs **clang ≥ 19 or gcc ≥ 14**):
  ```cmake
  project(APPNAME VERSION 4.0.0 LANGUAGES C CXX)
  find_package(TRIQS 4.0 REQUIRED)
  ```
- **app4triqs skeleton** — if your app is based on it, [merge the skeleton update](https://github.com/triqs/app4triqs#merging-app4triqs-skeleton-updates).

## 3. Build and test

Rebuild and run your test suite — it is the only reliable check against the behavioral
changes above (especially DLR). Use `-DPYTHON_EXECUTABLE=path_to_python3` to pick a Python.
