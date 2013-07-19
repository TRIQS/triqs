.. highlight:: c


The four reference Green functions
##################################

Real time
----------

``make_gf(double tmin, double tmax, size_t n_points, tqa::mini_vector<size_t,2> shape)``

``make_gf(double tmin, double tmax, size_t n_points, tqa::mini_vector<size_t,2> shape, mesh_kind mk)``


Real frequency
---------------

``make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t)``

``make_gf(double wmin, double wmax, size_t n_freq, tqa::mini_vector<size_t,2> shape)``

``make_gf(double wmin, double wmax, size_t n_freq, tqa::mini_vector<size_t,2> shape, mesh_kind mk)``


Matsubara time
---------------

``make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t)``

``make_gf(double beta, statistic_enum S,  tqa::mini_vector<size_t,2> shape, size_t Nmax=1025, mesh_kind mk= half_bins)``

``make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape, size_t Nmax, mesh_kind mk, local::tail_view const & t)``


Matsubara frequency
--------------------

``make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t)``

``make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape)``

``make_gf(double beta, statistic_enum S,  tqa::mini_vector<size_t,2> shape, size_t Nmax)``

``make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape, size_t Nmax, local::tail_view const & t)``


