cdef class MeshTwoRealTime: 
    cdef mesh_two_real_times  _c

    def __init__(self, double tmax, double n_time_slices) : 
        self._c = make_mesh_two_real_times(tmax,n_time_slices)
    
    property t_min:
        def __get__(self): return get_1d_mesh_from_2times_mesh(self._c).x_min()

    property t_max:
        def __get__(self): return get_1d_mesh_from_2times_mesh(self._c).x_max()

    #property kind:
    #    def __get__(self): return self._c.kind()

    def __len__ (self) : return self._c.size()
    
    #def __iter__(self) : # I use the C++ generator !
    #    cdef mesh_pt_generator[mesh_two_real_times] g = mesh_pt_generator[mesh_two_real_times](&self._c)
    #    while not g.at_end() : 
    #        yield g.to_point()
    #        g.increment()

    def __richcmp__(MeshTwoRealTime self, MeshTwoRealTime other, int op) : 
        if op ==2 : # ==
            return self._c == other._c

    def __reduce__(self):
        return self.__class__, (self.t_min, self.t_max, len(self)) #, self.kind)

# C -> Python 
cdef inline make_MeshTwoRealTime (mesh_two_real_times x) :
    return MeshTwoRealTime(get_1d_mesh_from_2times_mesh(x).x_max(), get_1d_mesh_from_2times_mesh(x).size() )

