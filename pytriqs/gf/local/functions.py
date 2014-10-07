from inverse import inverse
from block_gf import BlockGf
from gf import GfImFreq
from descriptor_base import A_Omega_Plus_B

def delta(g) :
    """Compute delta from G0"""
    if type(g) == BlockGf:
    	return BlockGf(name_block_generator = [ (n, delta(g0)) for n,g0 in g], make_copies=False)
    elif type(g) == GfImFreq:
    	g0_iw_inv = inverse(g)
    	delta_iw = g0_iw_inv.copy()
    	delta_iw <<= A_Omega_Plus_B(g0_iw_inv.tail[-1], g0_iw_inv.tail[0])
    	delta_iw -= g0_iw_inv
        return delta_iw
    else:
        raise TypeError, "No function delta for g0 object of type %s"%type(g) 
