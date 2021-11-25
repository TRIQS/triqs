// Copyright (c) 2021 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Philipp Dumitrescu

#include "mesh_concept_test.hpp"
// Domains
#include <triqs/mesh/domains/real_complex.hpp>
#include <triqs/mesh/domains/legendre.hpp>
#include <triqs/mesh/domains/matsubara.hpp>
#include <triqs/mesh/domains/interval.hpp>

using namespace triqs::mesh;

TEST(MeshConcepts, DomainConceptTest) {
  std::cout << "Check Domain Concept: " << type_name_domain<real_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<complex_domain>() << std::endl;
  //
  std::cout << "Check Domain Concept: " << type_name_domain<real_N_domain<2>>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<real_N_domain<7>>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<complex_N_domain<2>>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<complex_N_domain<7>>() << std::endl;
  //
  std::cout << "Check Domain Concept: " << type_name_domain<legendre_domain>() << std::endl;
  //
  std::cout << "Check Domain Concept: " << type_name_domain<matsubara_freq_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<matsubara_time_domain>() << std::endl;
  //
  std::cout << "Check Domain Concept: " << type_name_domain<real_interval_domain>() << std::endl;
}

TEST(MeshConcepts, IntervalDomainConceptTest) {
  //
  std::cout << "Check Domain Concept: " << type_name_interval_domain<legendre_domain>() << std::endl;
  //
  std::cout << "Check Domain Concept: " << type_name_interval_domain<matsubara_time_domain>() << std::endl;
  //
  std::cout << "Check Domain Concept: " << type_name_interval_domain<real_interval_domain>() << std::endl;
}

MAKE_MAIN;
