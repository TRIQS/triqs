from gf_imfreq import GfImFreq

cdef class GfImFreq_cython:

    cdef gf_imfreq _c

    def __init__(self, MeshImFreq mesh, data, TailGf tail):
        self._c = gf_imfreq (mesh._c, array_view[dcomplex,THREE](data), tail._c , nothing())
    
    def __write_hdf5_cython__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

    def set_from_fourier(self,GfImTime_cython gt) :
        """Fills self with the Fourier transform of gt"""
        self._c << lazy_fourier( gt._c )

    def set_from_legendre(self, GfLegendre_cython gl) :
        """Fills self with the Legendre transform of gl"""
        self._c << lazy_legendre_imfreq(gl._c)

    def density(self):
        return density(self._c).to_python()

    def __dealloc__ (self):
        pass

#----------------  Reading from h5 ---------------------------------------

def h5_read_GfImFreq(gr, key):
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
    return make_GfImFreq(h5_extractor[gf_imfreq]()(make_h5_group(gr),key), pack, name)

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (GfImFreq, read_fun = h5_read_GfImFreq)

#----------------  Convertions functions ---------------------------------------
  
# Python -> C
cdef gf_imfreq as_gf_imfreq (g) except +:
    return (<GfImFreq_cython?>g)._c

# C -> Python. Do NOT add except +
cdef make_GfImFreq (gf_imfreq x, indices_pack = [], name = "g"):
    data = x.data().to_python()
    if indices_pack == []:
      indices_pack = [range(data.shape[1]), range(data.shape[2])]
    return GfImFreq( 
            mesh = make_MeshImFreq (x.mesh()), 
            data = data,
            tail = make_TailGf (x.singularity()),
            indices_pack = indices_pack,
            name = name)

# Python -> C for blocks
cdef gf_block_imfreq  as_gf_block_imfreq (G) except +:
        cdef vector[gf_imfreq] v_c
        for n,g in G:
            v_c.push_back(as_gf_imfreq(g))
        return make_gf_block_imfreq (v_c)

# C -> Python for block
cdef make_BlockGfImFreq (gf_block_imfreq G, block_indices_pack = [], name = "G"):
    gl = []
    name_list = G.mesh().domain().names()
    if block_indices_pack == []:
      for i,n in enumerate(name_list):
        sha = G[i].data().to_python().shape[1:3]
        block_indices_pack.append( [range(sha[0]), range(sha[1])] )
    for i,n in enumerate(name_list):
      gl.append( make_GfImFreq(G[i], block_indices_pack[i]) )
    return BlockGf( name_list = name_list, block_list = gl, name = name )

