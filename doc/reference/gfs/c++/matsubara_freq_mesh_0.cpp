#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
 double beta = 1;
 int n_pts = 4;
 gf_mesh<imfreq> m(beta, Fermion, n_pts);
 std::cout << "Mesh m with only positive Matsubara frequencies : " << std::endl;
 for (auto& w : m) std::cout << w << std::endl;
 std::cout << "m.first_index() = " << m.first_index() << std::endl;
 std::cout << "m.last_index() = " << m.last_index() << std::endl;
 std::cout << "m.size() = " << m.size() << std::endl;
 gf_mesh<imfreq> m2(beta, Fermion, n_pts, false);
 std::cout << "Mesh m2 with positive and negative Matsubara frequencies : " << std::endl;
 for (auto& w : m2) std::cout << w << std::endl;
 std::cout << "m2.first_index() = " << m2.first_index() << std::endl;
 std::cout << "m2.last_index() = " << m2.last_index() << std::endl;
 std::cout << "m2.size() = " << m2.size() << std::endl;
 return 0;
}

