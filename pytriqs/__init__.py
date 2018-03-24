
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

from cpp2py import Cpp2pyInfoBase

class Cpp2pyInfo(Cpp2pyInfoBase):

    table_imports = {
        'triqs::gfs::' : 'pytriqs.gf',
        'triqs::operators::many_body_operator' : 'pytriqs.operators',
        'triqs::lattice' : 'pytriqs.lattice',
        'triqs::statistics::histogram' : 'pytriqs.statistics.histograms',
        'triqs::atom_diag::' : 'pytriqs.atom_diag'
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
        'triqs::h5::group' : 'h5'
        }

    table_converters = dict ( (k, "triqs/cpp2py_converters/%s.hpp"%v) for (k,v) in _table_converters.items())

def _get_cpp2py_wrapped_class_enums():
    return {'module_name' : 'UNUSED', 'includes' : "['<triqs/cpp2py_converters.hpp>']"}

__all__ = ['Cpp2pyInfo', 'archive', 'dos', 'fit', 'gf', 'lattice', 'operators', 'plot',
           'random_generator', 'sumk', 'utility']


