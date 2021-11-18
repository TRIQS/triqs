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
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::mesh;

TEST(MeshConcept, LinearMeshConceptTest) {
  std::cout << "Check Domain Concept: " << type_name_domain<R_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<matsubara_freq_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<matsubara_time_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<legendre_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<discrete_domain>() << std::endl;
  std::cout << "Check Domain Concept: " << type_name_domain<domain_product<R_domain, R_domain>>() << std::endl;
}

MAKE_MAIN;
