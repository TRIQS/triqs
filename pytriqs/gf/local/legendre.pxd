cdef extern from "triqs/gfs/legendre.hpp" namespace "triqs::gfs" : 
  
    cdef cppclass legendre_domain :
        size_t Nmax
        double beta
        statistic_enum statistic
        legendre_domain ()

    cdef cppclass mesh_legendre "triqs::gfs::gf_mesh<triqs::gfs::legendre>"  :
        mesh_legendre ()
        mesh_legendre (mesh_legendre &)
        legendre_domain & domain()
        long size()
        bint operator == (mesh_legendre &)

    cdef mesh_legendre make_mesh_legendre "triqs::gfs::gf_mesh<triqs::gfs::legendre>" (double beta, statistic_enum S, size_t n_leg)
    #cdef mesh_legendre make_mesh_legendre "triqs::gfs::gf_factories<triqs::gfs::legendre>::make_mesh" (double beta, statistic_enum S, size_t n_leg)

    cdef cppclass gf_legendre "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::legendre>>" :
        gf_legendre()
        gf_legendre(gf_legendre &)
        gf_legendre(mesh_legendre, array_view[double, THREE], nothing, nothing) #except +
        void operator << (gf_legendre &)
        mesh_legendre mesh() 
        array_view[double, THREE] data()

cdef extern from "triqs/gfs/legendre.hpp"  :
    cdef void h5_write (h5_group, char *, gf_legendre &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_legendre &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_legendre &) 

# Python -> C
cdef gf_legendre as_gf_legendre(g) except +

# C -> Python 
cdef make_GfLegendre(gf_legendre x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gfs/block.hpp" namespace "triqs::gfs" : 

    cdef cppclass gf_block_legendre "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::legendre>>>" :
        gf_block_legendre()
        gf_legendre & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_legendre  make_gf_block_legendre "triqs::gfs::make_block_gf_view_from_vector<triqs::gfs::gf<triqs::gfs::legendre>>" (vector[gf_legendre] &) 
    #cdef gf_block_legendre  make_gf_block_legendre "triqs::gfs::make_gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::legendre>>" (  vector[gf_legendre] &) 

cdef gf_block_legendre  as_gf_block_legendre (G) except +
cdef make_BlockGfLegendre (gf_block_legendre G, block_indices_pack=*, name=*)

