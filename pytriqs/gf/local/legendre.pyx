from gf_legendre import GfLegendre

cdef class GfLegendre_cython:
    cdef gf_legendre _c
    def __init__(self, MeshLegendre mesh, data):
        self._c = gf_legendre(mesh._c, array_view[double,THREE](data), nothing(), nothing())
    
    def __write_hdf5_cython__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

    def set_from_imtime(self, GfImTime_cython gt) :
        """Fills self with the Legendre transform of gt"""
        self._c << lazy_imtime_legendre(gt._c)

    def set_from_imfreq(self, GfImFreq_cython gw) :
        """Fills self with the Legendre transform of gw"""
        self._c << lazy_imfreq_legendre(gw._c)

    def density(self):
        return density(self._c).to_python()

    def enforce_discontinuity(self, disc):
        enforce_discontinuity(self._c, array_view[double,TWO](disc))

    def __dealloc__ (self):
        pass

#----------------  Reading from h5 ---------------------------------------

def h5_read_GfLegendre(gr, key):
    try:
      indicesL = gr[key]['indices']['left']
      indicesR = gr[key]['indices']['right']
      pack = [indicesL, indicesR]
    except:
      pack = []
    try:
      name = gr[key]['name']
    except:
      name = key
    return make_GfLegendre(h5_extractor[gf_legendre]()(make_h5_group(gr),key), pack, name)

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (GfLegendre, read_fun = h5_read_GfLegendre)

#----------------  Convertions functions ---------------------------------------

# Python -> C
cdef gf_legendre as_gf_legendre (g) except +:
    return (<GfLegendre_cython?>g)._c

# C -> Python. Do NOT add except +
cdef make_GfLegendre (gf_legendre x, indices_pack = [], name = "g"):
    data = x.data().to_python()
    if indices_pack == []:
      indices_pack = [range(data.shape[1]), range(data.shape[2])]
    return GfLegendre(
            mesh = make_MeshLegendre (x.mesh()),
            data = data,
            indices_pack = indices_pack,
            name = name)

# Python -> C for blocks
cdef gf_block_legendre  as_gf_block_legendre (G) except +:
        cdef vector[gf_legendre] v_c
        for n,g in G:
            v_c.push_back(as_gf_legendre(g))
        return make_gf_block_legendre (v_c)

# C -> Python for block
cdef make_BlockGfLegendre (gf_block_legendre G, block_indices_pack = [], name = "G"):
    gl = []
    name_list = G.mesh().domain().names()
    if block_indices_pack == []:
      for i,n in enumerate(name_list):
        sha = G[i].data().to_python().shape[1:3]
        block_indices_pack.append( [range(sha[0]), range(sha[1])] )
    for i,n in enumerate(name_list):
      gl.append( make_GfLegendre(G[i], block_indices_pack[i]) )
    return BlockGf( name_list = name_list, block_list = gl, name = name )

