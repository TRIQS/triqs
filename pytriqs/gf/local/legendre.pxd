cdef extern from "triqs/gf/legendre.hpp" namespace "triqs::gf" : 
  
    cdef cppclass legendre_domain :
        size_t Nmax
        double beta
        statistic_enum statistic
        legendre_domain ()

    #cdef cppclass mesh_legendre "triqs::gf::discrete_mesh<triqs::gf::legendre::domain_t>":
    cdef cppclass mesh_legendre "triqs::gf::discrete_mesh<triqs::gf::legendre_domain>":
        mesh_legendre ()
        mesh_legendre (mesh_legendre &)
        legendre_domain & domain()
        long size()
        bint operator == (mesh_legendre &)

    cdef mesh_legendre make_mesh_legendre "triqs::gf::make_gf_mesh<triqs::gf::legendre>" (double beta, statistic_enum S, size_t n_leg)
    #cdef mesh_legendre make_mesh_legendre "triqs::gf::gf_factories<triqs::gf::legendre>::make_mesh" (double beta, statistic_enum S, size_t n_leg)

    cdef cppclass gf_legendre "triqs::python_tools::cython_proxy<triqs::gf::gf_view<triqs::gf::legendre>>" :
        gf_legendre()
        gf_legendre(gf_legendre &)
        gf_legendre(mesh_legendre, array_view[double, THREE], nothing, nothing) #except +
        void operator << (gf_legendre &)
        mesh_legendre mesh() 
        array_view[double, THREE] data()

cdef extern from "triqs/gf/legendre.hpp"  :
    cdef void h5_write (h5_group, char *, gf_legendre &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_legendre &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_legendre &) 

# Python -> C
cdef gf_legendre as_gf_legendre(g) except +

# C -> Python 
cdef make_GfLegendre(gf_legendre x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gf/block.hpp" namespace "triqs::gf" : 

    cdef cppclass gf_block_legendre "triqs::python_tools::cython_proxy<triqs::gf::gf_view<triqs::gf::block_index,triqs::gf::gf<triqs::gf::legendre>>>" :
        gf_block_legendre()
        gf_legendre & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_legendre  make_gf_block_legendre "triqs::gf::make_gf_view<triqs::gf::block_index,triqs::gf::gf<triqs::gf::legendre>>" (  vector[gf_legendre] &) 

cdef gf_block_legendre  as_gf_block_legendre (G) except +
cdef make_BlockGfLegendre (gf_block_legendre G, block_indices_pack=*, name=*)

