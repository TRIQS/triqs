#pragma once
#include <triqs/mc_tools/random_generator.hpp>
#include <vector>
#include "configuration.hpp"

// A move flipping a random spin
struct flip {

  configuration *config;
  triqs::mc_tools::random_generator &RNG;

  int site;
  double delta_energy;

  // constructor
  flip(configuration *config_, triqs::mc_tools::random_generator &RNG_) : config(config_), RNG(RNG_) {}

  double attempt() {
    // pick a random site
    site = RNG(config->N);

    // find the neighbours with periodicity
    int left  = (site == 0 ? config->N - 1 : site - 1);
    int right = (site == config->N - 1 ? 0 : site + 1);

    // compute energy difference from field
    delta_energy = (config->chain[site] ? 2 : -2) * config->field;

    // compute energy difference from J
    if (config->chain[left] == config->chain[right]) { delta_energy += (config->chain[left] == config->chain[site] ? 4 : -4) * config->J; }

    // return Metroplis ratio
    return std::exp(-config->beta * delta_energy);
  }

  // if move accepted just flip site and update energy and magnetization
  double accept() {
    config->M += (config->chain[site] ? -2 : 2);
    config->chain[site] = !config->chain[site];
    config->energy += delta_energy;

    return 1.0;
  }

  // nothing to do if the move is rejected
  void reject() {}
};
