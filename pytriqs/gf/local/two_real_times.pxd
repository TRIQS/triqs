cdef extern from "triqs/gf/two_real_times.hpp" namespace "triqs::gf" : 
  
    cdef cppclass two_real_times_domain :
        two_real_times_domain()

    #cdef cppclass mesh_retime "triqs::gf::linear_mesh<triqs::gf::retime::domain_t>"  :
    cdef cppclass mesh_two_real_times "triqs::gf::mesh_product<triqs::gf::linear_mesh<triqs::gf::R_domain>,triqs::gf::linear_mesh<triqs::gf::R_domain>>"  :
        mesh_two_real_times ()
        mesh_two_real_times (mesh_two_real_times &)
        two_real_times_domain & domain()
        double x_min()
        double x_max()
        long size()
        double kind()
        bint operator ==( mesh_two_real_times &)

    cdef mesh_two_real_times make_mesh_two_real_times  "triqs::gf::make_gf_mesh<triqs::gf::two_real_times>" (double t_max, double n_time_slices)

    cdef mesh_retime & get_1d_mesh_from_2times_mesh "triqs::gf::get_1d_mesh_from_2times_mesh" (mesh_two_real_times &)

    cdef cppclass gf_two_real_times "triqs::python_tools::cython_proxy<triqs::gf::gf_view<triqs::gf::two_real_times>>" :
        gf_two_real_times()
        gf_two_real_times(gf_two_real_times &)
        # The constructor must be no_except, or the cython code won't be correct...
        gf_two_real_times(mesh_two_real_times, array_view[dcomplex, THREE], nothing, nothing) #except +
        void operator << (gf_two_real_times &)
        mesh_two_real_times mesh() 
        array_view[dcomplex, THREE] data()
        tail singularity() 

    cdef gf_retime slice1d "triqs::gf::slice" (gf_two_real_times &, double t) except +

cdef extern from "triqs/gf/two_real_times.hpp"  :
    cdef void h5_write (h5_group, char *, gf_two_real_times &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_two_real_times &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_two_real_times &) 

# Python -> C
cdef gf_two_real_times as_gf_two_real_times (g) except +

# C -> Python 
cdef make_GfTwoRealTime (gf_two_real_times x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gf/block.hpp" namespace "triqs::gf" : 

    cdef cppclass gf_block_two_real_times "triqs::python_tools::cython_proxy<triqs::gf::gf_view<triqs::gf::block_index,triqs::gf::gf<triqs::gf::two_real_times>>>" :
        gf_block_two_real_times()
        gf_two_real_times & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_two_real_times  make_gf_block_two_real_times "triqs::gf::make_gf_view<triqs::gf::block_index,triqs::gf::gf<triqs::gf::two_real_times>>" (  vector[gf_two_real_times] &) 

cdef gf_block_two_real_times  as_gf_block_two_real_times (G) except +
cdef make_BlockGfTwoRealTime (gf_block_two_real_times G, block_indices_pack=*, name=*)

