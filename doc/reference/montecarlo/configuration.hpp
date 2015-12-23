#pragma once
// The configuration of the system
struct configuration {

 // N is the length of the chain, M the total magnetization,
 // beta the inverse temperature, J the coupling,
 // field the magnetic field and energy the energy of the configuration
 int N, M;
 double beta, J, field, energy;

 // the chain of spins: true means "up", false means "down"
 std::vector<bool> chain;

 // constructor
 configuration(int N_, double beta_, double J_, double field_)
    : N(N_), M(-N), beta(beta_), J(J_), field(field_), energy(-N * (J - field)), chain(N, false) {}
};

