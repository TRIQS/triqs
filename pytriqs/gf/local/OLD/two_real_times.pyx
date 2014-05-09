from gf_two_real_times import GfTwoRealTime

cdef class GfTwoRealTime_cython:
    cdef gf_two_real_times _c
    def __init__(self, MeshTwoRealTime mesh, data):
        self._c = gf_two_real_times(mesh._c, array_view[dcomplex,THREE](data), nothing(), nothing())
    
    def __write_hdf5_cython__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

    def __dealloc__ (self):
        pass

    def slice1d(self, double t):
        return make_GfReTime(slice1d(self._c,t))


#----------------  Reading from h5 ---------------------------------------

def h5_read_GfTwoRealTime(gr, key):
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
    return make_GfTwoRealTime(h5_extractor[gf_two_real_times]()(make_h5_group(gr),key), pack, name)

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (GfTwoRealTime, read_fun = h5_read_GfTwoRealTime)

#----------------  Convertions functions ---------------------------------------

# Python -> C
cdef gf_two_real_times as_gf_two_real_times (g) except +:
    return (<GfTwoRealTime_cython?>g)._c

# C -> Python. Do NOT add except +
cdef make_GfTwoRealTime (gf_two_real_times x, indices_pack = [], name = "g"):
    data = x.data().to_python()
    if indices_pack == []:
      indices_pack = [range(data.shape[1]), range(data.shape[2])]
    return GfTwoRealTime(
            mesh = make_MeshTwoRealTime (x.mesh()),
            data = data,
            indices_pack = indices_pack,
            name = name)

# Python -> C for blocks
cdef gf_block_two_real_times  as_gf_block_two_real_times (G) except +:
        cdef vector[gf_two_real_times] v_c
        for n,g in G:
            v_c.push_back(as_gf_two_real_times(g))
        return make_gf_block_two_real_times (v_c)

# C -> Python for block
cdef make_BlockGfTwoRealTime (gf_block_two_real_times G, block_indices_pack = [], name = "G"):
    gl = []
    name_list = G.mesh().domain().names()
    if block_indices_pack == []:
      for i,n in enumerate(name_list):
        sha = G[i].data().to_python().shape[1:3]
        block_indices_pack.append( [range(sha[0]), range(sha[1])] )
    for i,n in enumerate(name_list):
      gl.append( make_GfTwoRealTime(G[i], block_indices_pack[i]) )
    return BlockGf( name_list = name_list, block_list = gl, name = name )

