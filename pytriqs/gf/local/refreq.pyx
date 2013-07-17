from gf_refreq import GfReFreq

cdef class GfReFreq_cython:
    cdef gf_refreq _c
    def __init__(self, MeshReFreq mesh, data, TailGf tail):
        self._c = gf_refreq(mesh._c, array_view[dcomplex,THREE](data), tail._c, nothing())
    
    def __write_hdf5_cython__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

    def set_from_fourier(self, GfReTime_cython gt) :
        """Fills self with the Fourier transform of gt"""
        self._c << lazy_fourier( gt._c )

    def inverse_fourier(self):
        return make_GfReTime(inverse_fourier(self._c))

    def set_from_pade(self, GfImFreq_cython gw, n_points = 100, freq_offset = 0.0) :
        pade(self._c, gw._c, n_points, freq_offset)

    def __dealloc__ (self):
        pass

#----------------  Reading from h5 ---------------------------------------

def h5_read_GfReFreq(gr, key):
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
    return make_GfReFreq(h5_extractor[gf_refreq]()(make_h5_group(gr),key), pack, name)

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (GfReFreq, read_fun = h5_read_GfReFreq)

#----------------  Convertions functions ---------------------------------------

# Python -> C
cdef gf_refreq as_gf_refreq (g) except +:
    return (<GfReFreq_cython?>g)._c

# C -> Python. Do NOT add except +
cdef make_GfReFreq (gf_refreq x, indices_pack = [], name = "g"):
    data = x.data().to_python()
    if indices_pack == []:
      indices_pack = [range(data.shape[1]), range(data.shape[2])]
    return GfReFreq(
            mesh = make_MeshReFreq (x.mesh()),
            data = data,
            tail = make_TailGf (x.singularity()),
            indices_pack = indices_pack,
            name = name)

# Python -> C for blocks
cdef gf_block_refreq  as_gf_block_refreq (G) except +:
        cdef vector[gf_refreq] v_c
        for n,g in G:
            v_c.push_back(as_gf_refreq(g))
        return make_gf_block_refreq (v_c)

# C -> Python for block
cdef make_BlockGfReFreq (gf_block_refreq G, block_indices_pack = [], name = "G"):
    gl = []
    name_list = G.mesh().domain().names()
    if block_indices_pack == []:
      for i,n in enumerate(name_list):
        sha = G[i].data().to_python().shape[1:3]
        block_indices_pack.append( [range(sha[0]), range(sha[1])] )
    for i,n in enumerate(name_list):
      gl.append( make_GfReFreq(G[i], block_indices_pack[i]) )
    return BlockGf( name_list = name_list, block_list = gl, name = name )

