cdef class MeshImTime: 
    cdef mesh_imtime  _c

    def __init__(self, beta, stat, int n_max, kind):
        self._c =  make_mesh_imtime(beta, {'F' :Fermion, 'B' : Boson}[stat], n_max, {'H':half_bins, 'F':full_bins, 'W':without_last}[kind])
    
    def __len__ (self) : return self._c.size()
    
    property beta : 
        """Inverse temperature"""
        def __get__(self): return self._c.domain().beta

    property kind : 
        """Inverse temperature"""
        def __get__(self): return self._c.kind()
    
    property statistic : 
        def __get__(self): return 'F' if self._c.domain().statistic==Fermion else 'B'
    
    def __iter__(self) : # I use the C++ generator !
        cdef mesh_pt_generator[mesh_imtime ] g = mesh_pt_generator[mesh_imtime ](&self._c)
        while not g.at_end() : 
            yield g.to_point()
            g.increment()

    def __richcmp__(MeshImTime self, MeshImTime other,int op) : 
        if op ==2 : # ==
            return self._c == other._c

    def __reduce__(self):
        return self.__class__, (self.beta, self.statistic, len(self), {half_bins: 'H', full_bins: 'F', without_last: 'W'}[self.kind])

# C -> Python 
cdef inline make_MeshImTime ( mesh_imtime x) :
    return MeshImTime( x.domain().beta, 'F' if x.domain().statistic==Fermion else 'B',
                       x.size(), {half_bins: 'H', full_bins: 'F', without_last: 'W'}[x.kind()] )

