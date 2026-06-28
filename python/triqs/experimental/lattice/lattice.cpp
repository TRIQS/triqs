#include <c2py/c2py.hpp>
#include <nda/c2py/converters.hpp>
#include <triqs/experimental/lattice/bz_integrators.hpp>
#include <triqs/experimental/lattice/tb_hk.hpp>
#include <triqs/experimental/lattice/gloc.hpp>
#include <triqs/experimental/lattice/superlattice.hpp>
#include <triqs/experimental/lattice/wannier_loader.hpp>
#include <triqs/experimental/utility/root_finder.hpp>

namespace triqs::experimental::lattice {

  // explicit mesh type instantiations of find_chemical_potential
  template double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::gf<mesh::dlr_imfreq, gfs::matrix_valued> const &Sigma,
                                          bz_int_options const &opt, std::string method, double precision, bool verbosity);
  template double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::gf<mesh::imfreq, gfs::matrix_valued> const &Sigma,
                                          bz_int_options const &opt, std::string method, double precision, bool verbosity);
  template double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::gf<mesh::refreq, gfs::matrix_valued> const &Sigma,
                                          bz_int_options const &opt, std::string method, double precision, bool verbosity);
  template double find_chemical_potential(double const target_density, tb_hk const &H_k,
                                          gfs::block_gf<mesh::dlr_imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt,
                                          std::string method, double precision, bool verbosity);
  template double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::block_gf<mesh::imfreq, gfs::matrix_valued> const &Sigma,
                                          bz_int_options const &opt, std::string method, double precision, bool verbosity);
  template double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::block_gf<mesh::refreq, gfs::matrix_valued> const &Sigma,
                                          bz_int_options const &opt, std::string method, double precision, bool verbosity);

  // explicit mesh type instantiations of gloc

  // with self energy
  template gfs::gf<mesh::dlr_imfreq, gfs::matrix_valued> gloc(tb_hk const &H_k, double mu, gfs::gf<mesh::dlr_imfreq, gfs::matrix_valued> const &Sigma,
                                                              bz_int_options const &opt);
  template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(tb_hk const &H_k, double mu, gfs::gf<mesh::imfreq, gfs::matrix_valued> const &Sigma,
                                                          bz_int_options const &opt);
  template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(tb_hk const &H_k, double mu, gfs::gf<mesh::refreq, gfs::matrix_valued> const &Sigma,
                                                          bz_int_options const &opt);
  // with self energy, block gf
  template gfs::block_gf<mesh::dlr_imfreq, gfs::matrix_valued>
  gloc(tb_hk const &H_k, double mu, gfs::block_gf<mesh::dlr_imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);
  template gfs::block_gf<mesh::imfreq, gfs::matrix_valued>
  gloc(tb_hk const &H_k, double mu, gfs::block_gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);
  template gfs::block_gf<mesh::refreq, gfs::matrix_valued>
  gloc(tb_hk const &H_k, double mu, gfs::block_gf<mesh::refreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);
  // zero self energy, standard gf
  template gfs::gf<mesh::dlr_imfreq, gfs::matrix_valued> gloc(mesh::dlr_imfreq const &w_mesh, tb_hk const &H_k, double mu, bz_int_options const &opt);
  template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(mesh::imfreq const &w_mesh, tb_hk const &H_k, double mu, bz_int_options const &opt);
  template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(mesh::refreq const &w_mesh, tb_hk const &H_k, double mu, bz_int_options const &opt);

} // namespace triqs::experimental::lattice

#include "lattice.wrap.cxx"
