/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
#include "bravais_lattice_and_brillouin_zone.hpp"
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <triqs/arrays/linalg/inverse.hpp>
#include <triqs/arrays/linalg/cross_product.hpp>
namespace triqs { namespace lattice_tools { 

 using namespace tqa;
 using namespace std;
 //using triqs::arrays::blas::dot;
 const double almost_zero(1E-10);

 bravais_lattice::bravais_lattice( units_type const & units__) : units_(3,3), dim_(units__.shape(0)) { 
  units_(range(0,dim_),range()) = units__();
  units_(range(dim_,3),range()) = 0;
  // First complete the basis. Add some tests for safety
  tqa::vector<double> ux(3),uy(3),uz(3); 
  assert(dim_==2);
  switch (dim_) {
   case 1:      
    ux = units_(0,range());
    uz() = 0; uz(1) = 1 ;
    uz = uz - dot(uz,ux)* ux;
    // no luck, ux was parallel to z, another one must work
    if (sqrt(dot(uz,uz))<almost_zero) {
     uz() = 0; uz(2) = 1; // 0,0,1;
     uz = uz - dot(uz,ux)* ux;
    }
    uz /= sqrt(dot(uz,uz));
    uy = cross_product(uz,ux);
    uy = uy/sqrt(dot(uy,uy)); // uy can not be 0
    units_(1,range()) = uz;
    units_(2,range()) = uy;
    break;
   case 2:
    uy() = 0; uy(2) = 1 ;
    uy = cross_product(units_(0,range()),units_(1,range()));
    double delta = sqrt(dot(uy,uy));
    if (abs(delta)<almost_zero) TRIQS_RUNTIME_ERROR<<"Tiling : the 2 vectors of unit are not independent";
    units_(2,range()) = uy /delta;
  }
 //cerr<<" Units = "<< units_<<endl; 
 }

 //------------------------------------------------------------------------------------

 brillouin_zone::brillouin_zone( bravais_lattice const & bl_) : lattice_(bl_), K_reciprocal(3,3) {
  bravais_lattice::units_type Units(lattice().units());
    std::cout << Units << std::endl;
  double delta = dot(Units(0,range()), cross_product(Units(1,range()),Units(2,range())));
    std::cout << dot(Units(0,range()), cross_product(Units(1,range()),Units(2,range())))<<std::endl;
    std::cout << cross_product(Units(1,range()),Units(2,range()))<<std::endl;
  if (abs(delta)<almost_zero) TRIQS_RUNTIME_ERROR<<"Tiling : the 3 vectors of Units are not independant";
  K_reciprocal(0,range()) = cross_product(Units(1,range()),Units(2,range())) / delta;
  K_reciprocal(1,range()) = cross_product(Units(2,range()),Units(0,range())) / delta;
  K_reciprocal(2,range()) = cross_product(Units(0,range()),Units(1,range())) / delta;
  //for (size_t i =0; i< lattice().dim();i++) std::cerr << " K_reciprocal(" << i << ")/(2pi) =  " << K_reciprocal(i,range())<< std::endl;
  const double pi = acos(-1.0);
  K_reciprocal = K_reciprocal*2*pi;
  K_reciprocal_inv =  inverse(K_reciprocal);
 }

 K_view_type brillouin_zone::lattice_to_real_coordinates (K_view_type const & k) const { 
  if (k.size()!=lattice().dim()) TRIQS_RUNTIME_ERROR<<"latt_to_real_k : dimension of k must be "<<lattice().dim();
  K_type res(3); res()=0; int dim = lattice().dim();
  for (int i =0; i< dim;i++) res += k (i) * K_reciprocal(i,range());
  return(res);
 }

 K_view_type brillouin_zone::real_to_lattice_coordinates (K_view_type const & k) const {
  if (k.size()!=lattice().dim()) TRIQS_RUNTIME_ERROR<<"latt_to_real_k : dimension of k must be "<<lattice().dim();
  K_type res(3);res()=0; int dim = lattice().dim();
  for (int i =0; i< dim;i++) res += k (i) * K_reciprocal_inv(i,range());
  return(res);
 }
}}//namespaces

