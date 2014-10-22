/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <iostream>
#include <type_traits>
#include <triqs/gfs.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::gfs;
using namespace triqs::clef;

int main(int argc, char* argv[]) {

 mpi::environment env(argc, argv);
 mpi::communicator world;

 std::ofstream out("node" + std::to_string(world.rank()));

 double beta = 10;
 int Nfreq = 8;
 placeholder<0> w_;

 auto g1 = gf<imfreq>{{beta, Fermion, Nfreq}, {1, 1}}; // using ARR = array<double,2>;
 g1(w_) << 1 / (w_ + 1);

 out << "g1.data" << g1.data() << std::endl;

 { 
  out<< "reduction "<< std::endl;
  gf<imfreq> g2 = mpi::reduce(g1, world);
  out << g2.data()<<std::endl;
  out << g2.singularity() << std::endl;
 }
 
 { 
  out<< "all reduction "<< std::endl;
  gf<imfreq> g2 = mpi::all_reduce(g1, world);
  out << g2.data()<<std::endl;
  out << g2.singularity() << std::endl;
 }

 {
  out << "scatter-gather test with =" << std::endl;
  auto g2 = g1;
  auto g2b = g1;

  g2 = mpi::scatter(g1);
  g2(w_) << g2(w_) * (1 + world.rank());
  g2b = mpi::gather(g2);

  out << g2b.data() << std::endl;
 }

 {
  out << "scatter-allgather test with construction" << std::endl;

  gf<imfreq> g2 = mpi::scatter(g1);
  //std::cout << " bo 1 "<< g2.mesh().size() << g2.data() << g2.singularity().data() << " rank = " <<  world.rank() << std::endl;
  //std::cout << g2(g2.singularity()) *2 << std::endl;
  // Fix this issue...
  g2.singularity() = g1.singularity();
  g2(w_) << g2(w_)  * (1 + world.rank());
  //std::cout << " bo 2 "<< world.rank() << std::endl;

  g1 = mpi::allgather(g2);

  out << g1.data() << std::endl;
 }

 {
  out << "reduce with block Green's function" << std::endl;
  block_gf<imfreq> bgf = make_block_gf({g1, g1, g1});

  block_gf<imfreq> bgf2;
  //auto bgf2=bgf;
  
  // TODO : Fix the mesh
  //bgf2 = mpi::scatter(bgf);
  //bgf = mpi::allgather(bgf2);

  bgf2 = mpi::reduce(bgf);

  out << bgf[0].data() << std::endl;
  out << bgf2[0].data() << std::endl;
 }

 {
  out << "Building directly scattered, and gather" << std::endl;
  auto m = mpi_scatter(gf_mesh<imfreq>{beta, Fermion, Nfreq}, world, 0);
  auto g3 = gf<imfreq>{m, {1, 1}}; 
  g3(w_) << 1 / (w_ + 1);
  auto g4 = g3;
  out<< "chunk ..."<<std::endl;
  out << g3.data() << std::endl;
  out<< "gather"<<std::endl;
  g4 = mpi::gather(g3);
  out << g4.data() << std::endl;
  out<< "allgather"<<std::endl;
  g4 = mpi::allgather(g3);
  out << g4.data() << std::endl;
 }

}

