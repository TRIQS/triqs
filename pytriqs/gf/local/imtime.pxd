cdef extern from "triqs/gf/imtime.hpp" namespace "triqs::gf" : 
  
    cdef cppclass imtime_domain :
        double beta
        statistic_enum statistic
        imtime_domain ()

    #cdef cppclass mesh_imtime "triqs::gf::linear_mesh<triqs::gf::imtime::domain_t>"  :
    cdef cppclass mesh_imtime "triqs::gf::linear_mesh<triqs::gf::matsubara_domain<false>>" :
        mesh_imtime ()
        mesh_imtime (mesh_imtime &)
        imtime_domain & domain()
        double x_min()
        long size()
        long kind()
        bint operator ==( mesh_imtime &)

    cdef mesh_imtime make_mesh_imtime "triqs::gf::make_gf_mesh<triqs::gf::imtime>" (double beta, statistic_enum S, size_t n_time_slices, mesh_enum mk)
    #cdef mesh_imtime make_mesh_imtime "triqs::gf::gf_factories<triqs::gf::imtime>::make_mesh" (double beta, statistic_enum S, size_t n_time_slices, mesh_enum mk)

    cdef cppclass gf_imtime "triqs::python_tools::cython_proxy<triqs::gf::gf_view<triqs::gf::imtime>>" :
        gf_imtime()
        gf_imtime(gf_imtime &)
        # The constructor must be no_except, or the cython code won't be correct...
        gf_imtime(mesh_imtime, array_view[double, THREE], tail, nothing) #except +
        void operator << (gf_imtime &)
        mesh_imtime mesh() 
        array_view[double, THREE] data()
        tail singularity() 

cdef extern from "triqs/gf/imtime.hpp"  :
    cdef void h5_write (h5_group, char *, gf_imtime &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_imtime &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_imtime &) 

# Python -> C
cdef gf_imtime as_gf_imtime (g) except +

# C -> Python 
cdef make_GfImTime (gf_imtime x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gf/block.hpp" namespace "triqs::gf" : 

    cdef cppclass gf_block_imtime "triqs::python_tools::cython_proxy<triqs::gf::gf_view<triqs::gf::block_index,triqs::gf::gf<triqs::gf::imtime>>>" :
        gf_block_imtime()
        gf_imtime & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_imtime  make_gf_block_imtime "triqs::gf::make_gf_view<triqs::gf::block_index,triqs::gf::gf<triqs::gf::imtime>>" (  vector[gf_imtime] &) 

cdef gf_block_imtime  as_gf_block_imtime (G) except +
cdef make_BlockGfImTime (gf_block_imtime G, block_indices_pack=*, name=*)

