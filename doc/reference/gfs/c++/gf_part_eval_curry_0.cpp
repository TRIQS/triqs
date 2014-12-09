#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using triqs::clef::placeholder;
int main() {
 double beta = 1, tmin = 0, tmax = 1.0;
 int n_re_time = 100, n_im_time = 100;

 using g_t_tau_s = gf<cartesian_product<retime, imtime>, scalar_valued>;

 // a scalar valued function
 auto g = g_t_tau_s{{{tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}}};

 // evaluation of the second variable to 3 : 3 is the **index** of the point !
 auto g_sliced = partial_eval<1>(g(), 3);
 // g_sliced is now a gf_view/gf_const_view<retime> seeing the value at the index 3

 std::cout << g_sliced << std::endl;
}

