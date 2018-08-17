#pragma once
#include "configuration.hpp"

// The measure of the magnetization
struct compute_m {

  configuration *config;
  double Z, M;

  compute_m(configuration *config_) : config(config_), Z(0), M(0) {}

  // accumulate Z and magnetization
  void accumulate(int sign) {
    Z += sign;
    M += config->M;
  }

  // get final answer M / (Z*N)
  void collect_results(triqs::mpi::communicator c) {
    double sum_Z = triqs::mpi::mpi_reduce(Z, c);
    double sum_M = triqs::mpi::mpi_reduce(M, c);
    if (c.rank() == 0) std::cout << "Magnetization: " << sum_M / sum_Z << std::endl;
  }
};
