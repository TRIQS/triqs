from gf_retime import GfReTime

cdef class GfReTime_cython:
    cdef gf_retime _c
    def __init__(self, MeshReTime mesh, data, TailGf tail):
        self._c = gf_retime(mesh._c, array_view[dcomplex,THREE](data), tail._c, nothing())
    
    def __write_hdf5_cython__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

    # Put it back ?
    #def fourier(self):
    #    return make_GfReFreq(fourier(self._c))

    def set_from_inverse_fourier(self, GfReFreq_cython gw) :
        """Fills self with the Inverse Fourier transform of gw"""
        self._c << inverse_fourier( gw._c)

    def __dealloc__ (self):
        pass

#----------------  Reading from h5 ---------------------------------------

def h5_read_GfReTime(gr, key):
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
    return make_GfReTime(h5_extractor[gf_retime]()(make_h5_group(gr),key), pack, name)

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (GfReTime, read_fun = h5_read_GfReTime)

#----------------  Convertions functions ---------------------------------------

# Python -> C
cdef gf_retime as_gf_retime (g) except +:
    return (<GfReTime_cython?>g)._c

# C -> Python. Do NOT add except +
cdef make_GfReTime (gf_retime x, indices_pack = [], name = "g"):
    data = x.data().to_python()
    if indices_pack == []:
      indices_pack = [range(data.shape[1]), range(data.shape[2])]
    return GfReTime(
            mesh = make_MeshReTime (x.mesh()),
            data = data,
            tail = make_TailGf (x.singularity()),
            indices_pack = indices_pack,
            name = name)

# Python -> C for blocks
cdef gf_block_retime  as_gf_block_retime (G) except +:
        cdef vector[gf_retime] v_c
        for n,g in G:
            v_c.push_back(as_gf_retime(g))
        return make_gf_block_retime (v_c)

# C -> Python for block
cdef make_BlockGfReTime (gf_block_retime G, block_indices_pack = [], name = "G"):
    gl = []
    name_list = G.mesh().domain().names()
    if block_indices_pack == []:
      for i,n in enumerate(name_list):
        sha = G[i].data().to_python().shape[1:3]
        block_indices_pack.append( [range(sha[0]), range(sha[1])] )
    for i,n in enumerate(name_list):
      gl.append( make_GfReTime(G[i], block_indices_pack[i]) )
    return BlockGf( name_list = name_list, block_list = gl, name = name )

