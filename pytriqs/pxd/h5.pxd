from libcpp.string cimport string as std_string
cdef extern from "<triqs/h5.hpp>" namespace "triqs::h5" :
#cdef extern from "<triqs/arrays/h5/group_or_file.hpp>" : #namespace "triqs::arrays::h5" :

    cdef cppclass h5_group "triqs::h5::group" :
        h5_group (int, bint)
        h5_group ()

cdef extern from "<triqs/h5/h5_extractor.hpp>" namespace "triqs::h5" :

    cdef cppclass h5_extractor "triqs::h5::h5_extractor" [T] :
        h5_extractor()
        T & operator()( h5_group &, std_string)

cdef inline h5_group make_h5_group (gr) :
        import h5py
        return h5_group (gr.id.id, type(gr) == h5py.highlevel.Group)
