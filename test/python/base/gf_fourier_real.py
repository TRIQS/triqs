# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Nils Wentzell

from triqs.gf import *

# ==== Matrix-valued Green functions

gw = GfReFreq(indices = [1], window = (-5, 5), n_points = 1001, name = "egBlock")
gw << SemiCircular(2.0)

gt = make_gf_from_inverse_fourier(gw)
