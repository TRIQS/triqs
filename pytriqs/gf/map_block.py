################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2018 N. Wentzell
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
from block_gf import BlockGf
from block2_gf import Block2Gf

def map_block(fun, G):
    """
    Map function f(Gf)->Gf to every element of a BlockGf or Block2Gf
    """

    if isinstance(G, BlockGf):
        block_list = [fun(bl) for name, bl in G]
        return BlockGf(name_list = list(G.indices), block_list = block_list)

    elif isinstance(G, Block2Gf):
        block_list = []
        for bn1 in G.indices1:
            block_list.append([fun(G[bn1,bn2]) for bn2 in G.indices2])
        return Block2Gf(name_list1 = list(G.indices1), name_list2 = list(G.indices2), block_list = block_list)

    else:
        raise Exception('map_block only applicable for BlockGf and Block2Gf') 
