
// Bug # : Clef expression for the bubble diagram in imaginary time

#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;

int ntau    = 5;
double beta = 1.2345;

placeholder_prime<0> tau;

auto g = gf<imtime, scalar_valued>{{beta, Fermion, ntau}};
auto chi_ref = gf<imtime, scalar_valued>{{beta, Boson, ntau}};
auto chi = gf<imtime, scalar_valued>{{beta, Boson, ntau}};

// ----------------------------------------------------

TEST(Gf, bubble) {

  for (auto tau : g.mesh()) g[tau] = 1.;

  // this should give g(0<tau<beta) * g(-beta<tau<0) = (+1) * (-1) = -1
  
  chi(tau) << g(tau) * g(-tau); // This is not working correctly on the boundaries!

  for (auto tau : chi_ref.mesh()) chi_ref[tau] = -1.;

  EXPECT_ARRAY_NEAR(chi.data(), chi_ref.data());
}

// ----------------------------------------------------

TEST(Gf, bubble_boundary_hack) {

  for (auto tau : g.mesh()) g[tau] = 1.;

  // this should give g(0<tau<beta) * g(-beta<tau<0) = (+1) * (-1) = -1

  double eps = 1e-9;
  
  for( auto tau : g.mesh() ) {
    double tau_num = tau;

    // -- Hack shifting points on the boundary inside [0, beta]
    if( abs(tau_num - beta) < eps ) tau_num -= eps;
    if( abs(tau_num) < eps ) tau_num += eps;
    
    chi[tau] = g(tau_num) * g(-tau_num); 
  }
  
  for (auto tau : chi_ref.mesh()) chi_ref[tau] = -1.;

  EXPECT_ARRAY_NEAR(chi.data(), chi_ref.data());
}

MAKE_MAIN;
