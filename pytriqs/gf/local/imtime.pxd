cdef extern from "triqs/gfs/imtime.hpp" namespace "triqs::gfs" : 
  
    cdef cppclass imtime_domain :
        double beta
        statistic_enum statistic
        imtime_domain ()

    cdef cppclass mesh_imtime "triqs::gfs::gf_mesh<triqs::gfs::imtime>"  :
        mesh_imtime ()
        mesh_imtime (mesh_imtime &)
        imtime_domain & domain()
        double x_min()
        long size()
        long kind()
        bint operator ==( mesh_imtime &)

    cdef mesh_imtime make_mesh_imtime "triqs::gfs::gf_mesh<triqs::gfs::imtime>" (double beta, statistic_enum S, size_t n_time_slices, mesh_enum mk)
    #cdef mesh_imtime make_mesh_imtime "triqs::gfs::gf_factories<triqs::gfs::imtime>::make_mesh" (double beta, statistic_enum S, size_t n_time_slices, mesh_enum mk)

    cdef cppclass gf_imtime "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::imtime>>" :
        gf_imtime()
        gf_imtime(gf_imtime &)
        # The constructor must be no_except, or the cython code won't be correct...
        gf_imtime(mesh_imtime, array_view[double, THREE], tail, nothing) #except +
        void operator << (gf_imtime &)
        mesh_imtime mesh() 
        array_view[double, THREE] data()
        tail singularity() 

cdef extern from "triqs/gfs/imtime.hpp"  :
    cdef void h5_write (h5_group, char *, gf_imtime &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_imtime &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_imtime &) 

# Python -> C
cdef gf_imtime as_gf_imtime (g) except +

# C -> Python 
cdef make_GfImTime (gf_imtime x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gfs/block.hpp" namespace "triqs::gfs" : 

    cdef cppclass gf_block_imtime "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::imtime>>>" :
        gf_block_imtime()
        gf_imtime & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_imtime  make_gf_block_imtime "triqs::gfs::make_block_gf_view_from_vector<triqs::gfs::gf<triqs::gfs::imtime>>" (vector[gf_imtime] &) 
    #cdef gf_block_imtime  make_gf_block_imtime "triqs::gfs::make_gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::imtime>>" (  vector[gf_imtime] &) 

cdef gf_block_imtime  as_gf_block_imtime (G) except +
cdef make_BlockGfImTime (gf_block_imtime G, block_indices_pack=*, name=*)

