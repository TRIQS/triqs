from pytriqs.gf.local import *
from test_bl import *

gt = BlockGf(name_list = ['a',], block_list = [GfImTime(indices = [0,1], beta = 50, n_points = 200),], make_copies=False)
fct_2_blocks(gt,gt)
