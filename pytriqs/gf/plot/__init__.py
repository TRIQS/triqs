from . import one_var
from pytriqs.gf.meshes import *
from pytriqs.gf.mesh_product import MeshProduct

def dispatcher(g) :
    """ given g, pick up the right plotting function"""

    if not isinstance(g.mesh, MeshProduct):
        m= g.mesh

        if isinstance(m, MeshImFreq) : return one_var.imfreq
        if isinstance(m, MeshImTime) : return one_var.imtime
        if isinstance(m, MeshReFreq) : return one_var.refreq
        if isinstance(m, MeshReTime) : return one_var.retime
        if isinstance(m, MeshLegendre) : return one_var.legendre

    else:
        raise NotImplemented 

__init__ = [dispatcher]
