cdef class MeshLegendre: 
    cdef mesh_legendre  _c

    def __init__(self, beta, stat, int n_leg):
        self._c = make_mesh_legendre(beta, {'F' :Fermion, 'B' : Boson}[stat], n_leg)
    
    def __len__ (self) : return self._c.size()
    
    property beta : 
        """Inverse temperature"""
        def __get__(self): return self._c.domain().beta
    
    property statistic : 
        def __get__(self): return 'F' if self._c.domain().statistic==Fermion else 'B'
    
    def __iter__(self) : # I use the C++ generator !
        cdef mesh_pt_generator[mesh_legendre ] g = mesh_pt_generator[mesh_legendre ](&self._c)
        while not g.at_end() : 
            yield g.to_point()
            g.increment()

    def __richcmp__(MeshLegendre self, MeshLegendre other,int op) : 
        if op ==2 : # ==
            return self._c == other._c

    def __reduce__(self):
        return self.__class__, (self.beta, self.statistic, len(self))

# C -> Python 
cdef inline make_MeshLegendre ( mesh_legendre x) :
    return MeshLegendre( x.domain().beta, 'F' if x.domain().statistic==Fermion else 'B', x.size() )

