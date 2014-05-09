cdef class MeshReTime: 
    cdef mesh_retime  _c

    def __init__(self, double t_min, double t_max, int n_max, kind):
        self._c = make_mesh_retime(t_min, t_max, n_max, {'H':half_bins, 'F':full_bins, 'W':without_last}[kind])
    
    property t_min:
        def __get__(self): return self._c.x_min()

    property t_max:
        def __get__(self): return self._c.x_max()

    property kind:
        def __get__(self): return {half_bins: 'H', full_bins: 'F', without_last: 'W'}[self._c.kind()]

    def __len__ (self) : return self._c.size()
    
    def __iter__(self) : # I use the C++ generator !
        cdef mesh_pt_generator[mesh_retime] g = mesh_pt_generator[mesh_retime](&self._c)
        while not g.at_end() : 
            yield g.to_point()
            g.increment()

    def __richcmp__(MeshReTime self, MeshReTime other, int op) : 
        if op ==2 : # ==
            return self._c == other._c

    def __reduce__(self):
        return self.__class__, (self.t_min, self.t_max, len(self), self.kind)

# C -> Python 
cdef inline make_MeshReTime ( mesh_retime x) :
    return MeshReTime( x.x_min(), x.x_max(), x.size(), {half_bins: 'H', full_bins: 'F', without_last: 'W'}[x.kind()] )

