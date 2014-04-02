from gf_imtime import GfImTime

cdef class GfImTime_cython:

    cdef gf_imtime _c

    def __init__(self, MeshImTime mesh, data, TailGf tail):
        self._c = gf_imtime (mesh._c, array_view[double,THREE](data), tail._c, nothing())
    
    def __write_hdf5_cython__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

    def set_from_inverse_fourier(self,GfImFreq_cython gw) :
        """Fills self with the Inverse Fourier transform of gw"""        
        self._c << inverse_fourier( gw._c)

    def set_from_legendre(self, GfLegendre_cython gl) :
        """Fills self with the Legendre transform of gl"""
        self._c << legendre_to_imtime(gl._c)

    def __dealloc__ (self):
        pass

    def __call__ (self, float tau) : 
        return matrix[double](self._c (tau)).to_python()

#----------------  Reading from h5 ---------------------------------------

def h5_read_GfImTime(gr, key):
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
    return make_GfImTime(h5_extractor[gf_imtime]()(make_h5_group(gr),key), pack, name)

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (GfImTime, read_fun = h5_read_GfImTime)

#----------------  Convertions functions ---------------------------------------

# Python -> C
cdef gf_imtime as_gf_imtime (g) except +:
    return (<GfImTime_cython?>g)._c

# C -> Python. Do NOT add except +
cdef make_GfImTime (gf_imtime x, indices_pack = [], name = "g"):
    data = x.data().to_python()
    if indices_pack == []:
      indices_pack = [range(data.shape[1]), range(data.shape[2])]
    else :
        # check that the dimensions are ok
        assert len(indices_pack)==2
        assert len(indices_pack[0]) == data.shape[1]
        assert len(indices_pack[1]) == data.shape[2]
    return GfImTime( 
            mesh = make_MeshImTime (x.mesh()), 
            data = data,
            tail = make_TailGf (x.singularity()),
            indices_pack = indices_pack,
            name = name)

# Python -> C for blocks
cdef gf_block_imtime  as_gf_block_imtime (G) except +:
        cdef vector[gf_imtime] v_c
        for n,g in G:
            v_c.push_back(as_gf_imtime(g))
        return make_gf_block_imtime (v_c)

# C -> Python for block
cdef make_BlockGfImTime (gf_block_imtime G, block_indices_pack = [], name = "G"):
    gl = []
    name_list = G.mesh().domain().names()
    if block_indices_pack == []:
      for i,n in enumerate(name_list):
        sha = G[i].data().to_python().shape[1:3]
        block_indices_pack.append( [range(sha[0]), range(sha[1])] )
    for i,n in enumerate(name_list):
      gl.append( make_GfImTime(G[i], block_indices_pack[i]) )
    return BlockGf( name_list = name_list, block_list = gl, name = name )

