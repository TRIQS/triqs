cdef extern from "triqs/gfs/two_real_times.hpp" namespace "triqs::gfs" : 
  
    cdef cppclass two_real_times_domain :
        two_real_times_domain()

    #cdef cppclass mesh_retime "triqs::gfs::linear_mesh<triqs::gfs::retime::domain_t>"  :
    cdef cppclass mesh_two_real_times "triqs::gfs::mesh_product<triqs::gfs::linear_mesh<triqs::gfs::R_domain>,triqs::gfs::linear_mesh<triqs::gfs::R_domain>>"  :
        mesh_two_real_times ()
        mesh_two_real_times (mesh_two_real_times &)
        two_real_times_domain & domain()
        double x_min()
        double x_max()
        long size()
        double kind()
        bint operator ==( mesh_two_real_times &)

    cdef mesh_two_real_times make_mesh_two_real_times  "triqs::gfs::make_gf_mesh<triqs::gfs::two_real_times>" (double t_max, double n_time_slices)

    cdef mesh_retime & get_1d_mesh_from_2times_mesh "triqs::gfs::get_1d_mesh_from_2times_mesh" (mesh_two_real_times &)

    cdef cppclass gf_two_real_times "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::two_real_times>>" :
        gf_two_real_times()
        gf_two_real_times(gf_two_real_times &)
        # The constructor must be no_except, or the cython code won't be correct...
        gf_two_real_times(mesh_two_real_times, array_view[dcomplex, THREE], nothing, nothing) #except +
        void operator << (gf_two_real_times &)
        mesh_two_real_times mesh() 
        array_view[dcomplex, THREE] data()
        tail singularity() 

    cdef gf_retime slice1d "triqs::gfs::slice" (gf_two_real_times &, double t) except +

cdef extern from "triqs/gfs/two_real_times.hpp"  :
    cdef void h5_write (h5_group, char *, gf_two_real_times &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_two_real_times &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_two_real_times &) 

# Python -> C
cdef gf_two_real_times as_gf_two_real_times (g) except +

# C -> Python 
cdef make_GfTwoRealTime (gf_two_real_times x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gfs/block.hpp" namespace "triqs::gfs" : 

    cdef cppclass gf_block_two_real_times "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::two_real_times>>>" :
        gf_block_two_real_times()
        gf_two_real_times & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_two_real_times  make_gf_block_two_real_times "triqs::gfs::make_gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::two_real_times>>" (  vector[gf_two_real_times] &) 

cdef gf_block_two_real_times  as_gf_block_two_real_times (G) except +
cdef make_BlockGfTwoRealTime (gf_block_two_real_times G, block_indices_pack=*, name=*)

