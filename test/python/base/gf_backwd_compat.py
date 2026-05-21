# Copyright (c) 2026 Simons Foundation
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

import sys
import unittest
import warnings


class test_triqs_gf_backwd_compat(unittest.TestCase):

    def test_warning_emitted(self):
        # Force a re-import so the FutureWarning fires even if triqs.gf was
        # already imported elsewhere in the process.
        sys.modules.pop("triqs.gf", None)
        with warnings.catch_warnings(record=True) as caught:
            warnings.simplefilter("always")
            import triqs.gf  # noqa: F401
        self.assertTrue(
            any(issubclass(w.category, FutureWarning) and "triqs.gf" in str(w.message) for w in caught),
            "Expected a FutureWarning mentioning triqs.gf on import",
        )

    def test_module_identity(self):
        import triqs.gf
        import triqs.gfs
        import triqs.mesh
        self.assertIs(triqs.gf, triqs.gfs)
        self.assertIs(sys.modules["triqs.gf.meshes"], triqs.mesh)

    def test_submodule_paths(self):
        # Every previously hand-maintained alias should still resolve.
        for path in [
            "triqs.gf.gf", "triqs.gf.block_gf", "triqs.gf.block2_gf", "triqs.gf.map_block",
            "triqs.gf.descriptor_base", "triqs.gf.descriptors", "triqs.gf.lazy_expressions",
            "triqs.gf.tools", "triqs.gf.dlr_crm_dyson_solver",
            "triqs.gf.plot", "triqs.gf.backwd_compat", "triqs.gf.gf_fnt",
            "triqs.gf.gf_factories", "triqs.gf.wrapped_aux",
            "triqs.gf.backwd_compat.gf_imfreq", "triqs.gf.backwd_compat.gf_imtime",
            "triqs.gf.backwd_compat.gf_refreq", "triqs.gf.backwd_compat.gf_retime",
            "triqs.gf.backwd_compat.gf_legendre",
            "triqs.gf.meshes", "triqs.gf.mesh_product", "triqs.gf.mesh_point",
            "triqs.gf.matsubara_freq", "triqs.gf.semicirc",
            "triqs.gf.meshes.mesh_product",
        ]:
            with self.subTest(path=path):
                __import__(path)
                self.assertIn(path, sys.modules)

    def test_legacy_imports(self):
        # Names that should resolve through both layouts.
        from triqs.gf import GfImFreq, BlockGf, MeshImFreq, MatsubaraFreq, MeshProduct  # noqa: F401
        from triqs.gf.backwd_compat.gf_imfreq import GfImFreq as GfImFreq2  # noqa: F401
        from triqs.gf.semicirc import g_semicirc_iw  # noqa: F401
        # triqs.gf.meshes must be the Python package (exposes Python-only names like
        # MeshProduct and MatsubaraFreq), not the compiled .so submodule.
        from triqs.gf.meshes import MeshImFreq as MIF2, MeshProduct as MP2, MatsubaraFreq as MF2  # noqa: F401
        from triqs.gf.meshes.mesh_product import MeshProduct as MP3  # noqa: F401
        from triqs.gf.mesh_product import MeshProduct as MP4  # noqa: F401
        from triqs.gf.mesh_point import MeshPoint  # noqa: F401
        from triqs.gf.matsubara_freq import MatsubaraFreq as MF3  # noqa: F401


if __name__ == "__main__":
    unittest.main()
