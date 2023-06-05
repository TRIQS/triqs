# Copyright (c) 2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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
# Authors: Michel Ferrero, Nils Wentzell

from triqs.gf import *
from test_bl import *

gt = BlockGf(name_list = ['a',], block_list = [GfImTime(indices = [0,1], beta = 50, n_points = 200),], make_copies=False)
fct_2_blocks(gt,gt)
