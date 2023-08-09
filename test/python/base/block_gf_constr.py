from triqs.gf import *
G1 = GfImFreq(beta=40, target_shape=(1,1))

""" this test checks all constructors of BlockGf """

names = ('up','down')
blocks = (G1, G1)

def check_G_names(G):
    assert tuple(G.indices) == names

def check_G_shape(G):
    for key, block in G:
        assert block.data.shape == (2050, 1, 1)

def check_make_copies(G, make_copies=False):
    for key, block in G:
        if make_copies:
            assert block is not G1
        else:
            assert block is G1

G = BlockGf(name_list=names, block_list=blocks)
check_G_names(G)
check_G_shape(G)
check_make_copies(G)

G = BlockGf(name_list=names, block_list=blocks, make_copies=False)
check_G_names(G)
check_G_shape(G)
check_make_copies(G)

G = BlockGf(name_list=names, block_list=blocks, make_copies=True)
check_G_names(G)
check_G_shape(G)
check_make_copies(G, True)

G = BlockGf(name_list=names, block_list=blocks, make_copies=False, name='A')
check_G_names(G)
check_G_shape(G)
check_make_copies(G)
assert G.name == 'A'

G = BlockGf(name_list=names, block_list=blocks, make_copies=True, name='A')
check_G_names(G)
check_G_shape(G)
check_make_copies(G, True)
assert G.name == 'A'

G = BlockGf(mesh=G1.mesh, gf_struct=(('up',1),('down',1)))
check_G_names(G)
check_G_shape(G)

G = BlockGf(mesh=G1.mesh, gf_struct=(('up',1),('down',1)), target_rank=3)
check_G_names(G)
for key, block in G:
    assert block.data.shape == (2050, 1, 1, 1)

G = BlockGf(name_block_generator=[('up',G1),('down',G1)])
check_G_names(G)
check_G_shape(G)
check_make_copies(G)

G = BlockGf(name_block_generator=[('up',G1),('down',G1)], make_copies=False)
check_G_names(G)
check_G_shape(G)
check_make_copies(G)

G = BlockGf(name_block_generator=[('up',G1),('down',G1)], make_copies=True)
check_G_names(G)
check_G_shape(G)
check_make_copies(G, True)
