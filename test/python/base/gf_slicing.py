# Copyright (c) 2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Nils Wentzell

from triqs.gf import *
from triqs.utility import capture_stdout

ref_output="""['0', '1', '2']
['0', '1'] ['0']
['a', 'b', 'c']
['a', 'b'] ['a']"""

with capture_stdout() as output:
    g = GfImFreq(beta=10, indices=[0,1,2])
    print(g.indices[0])
    print(g[0:2,0:1].indices[0], g[0:2,0:1].indices[1])
    
    g = GfImFreq(beta=10, indices=['a','b','c'])
    print(g.indices[0])
    print(g[0:2,0:1].indices[0], g[0:2,0:1].indices[1])

assert(output == ref_output.splitlines())
