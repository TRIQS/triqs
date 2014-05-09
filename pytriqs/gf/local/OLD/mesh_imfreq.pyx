cdef class MeshImFreq: 
    cdef mesh_imfreq  _c

    def __init__(self, beta, stat, int n_pts, bint positive_only=True): 
        self._c =  make_mesh_imfreq(beta,{'F' :Fermion, 'B' : Boson}[stat], n_pts, positive_only)
    
    def __len__ (self) : return self._c.size()
    
    property beta : 
        """Inverse temperature"""
        def __get__(self): return self._c.domain().beta
    
    property statistic : 
        def __get__(self): return 'F' if self._c.domain().statistic==Fermion else 'B'
    
    property positive_only :
        def __get__(self): return  self._c.positive_only()

    def __iter__(self) : # I use the C++ generator !
        cdef mesh_pt_generator[mesh_imfreq ] g = mesh_pt_generator[mesh_imfreq ](&self._c)
        while not g.at_end() : 
            yield g.to_point()
            g.increment()

    def __richcmp__(MeshImFreq self, MeshImFreq other,int op) : 
        if op ==2 : # ==
            return self._c == other._c

    def __reduce__(self):
        return self.__class__, (self.beta, self.statistic, len(self))

# C -> Python 
cdef inline make_MeshImFreq (mesh_imfreq x) :
    return MeshImFreq( x.domain().beta, 'F' if x.domain().statistic==Fermion else 'B', x.size(), x.positive_only() )


