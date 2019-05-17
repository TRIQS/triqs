# Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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
#
# Authors: Olivier Parcollet, Nils Wentzell


class Cpp2pyInfo:

    table_imports = {
        'triqs::gfs::' : 'triqs.gf',
        'triqs::operators::many_body_operator' : 'triqs.operators',
        'triqs::lattice' : 'triqs.lattice',
        'triqs::stat::histogram' : 'triqs.stat.histograms',
        'triqs::atom_diag::' : 'triqs.atom_diag',
        'mpi::' : 'triqs.utility.mpi',
        'h5::' : 'h5._h5py'
        }

    _table_converters = {
        'triqs::arrays::array' : 'arrays',
        'triqs::arrays::matrix' : 'arrays',
        'triqs::arrays::vector' : 'arrays',
        'triqs::gfs::gf*' : 'gf',
        'triqs::gfs::block_gf*' : 'gf',
        'triqs::gfs::block2_gf*' : 'gf',
        'triqs::operators::many_body_operator*' : 'operators_real_complex',
        'triqs::hilbert_space::fundamental_operator_set' : 'fundamental_operator_set',
        'triqs::utility::real_or_complex' : 'real_or_complex',
        }

    table_converters = dict ( (k, "triqs/cpp2py_converters/%s.hpp"%v) for (k,v) in list(_table_converters.items()))

def _get_cpp2py_wrapped_class_enums():
    return {'module_name' : 'UNUSED', 'includes' : "['<triqs/cpp2py_converters.hpp>']"}

__all__ = ['Cpp2pyInfo', 'archive', 'dos', 'fit', 'gf', 'lattice', 'operators', 'plot',
           'random_generator', 'sumk', 'utility']


