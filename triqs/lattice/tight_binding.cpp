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
#include "tight_binding.hpp"
#include <triqs/arrays/algorithms.hpp>
#include <triqs/arrays/linalg/eigenelements.hpp>
#include "grid_generator.hpp"
#include "functors.hpp"
namespace triqs { namespace lattice_tools { 
using namespace std;
 using namespace tqa;

 //------------------------------------------------------

 array_view <dcomplex,3> hopping_stack (tight_binding const & TB, array_view<double,2> const & k_stack) {
  auto TK = Fourier(TB);  
  array<dcomplex,3> res(TB.n_bands(), TB.n_bands(), k_stack.shape(1));
  for(size_t i = 0; i<k_stack.shape(1); ++i) res(range(), range(), i) = TK(k_stack(range(),i));
  return res;
 }

 //------------------------------------------------------

 array_view<double,2> energies_on_bz_path(tight_binding const & TB, K_view_type K1, K_view_type K2, size_t n_pts) {
  auto TK = Fourier(TB);
  const size_t norb=TB.lattice().n_orbitals();
  const size_t ndim=TB.lattice().dim();
  array<double,2> eval(norb,n_pts);
  K_type dk = (K2 - K1)/double(n_pts), k = K1;
  for (size_t i =0; i<n_pts; ++i, k += dk) {
   eval(range(),i) = linalg::eigenvalues( TK( k (range(0,ndim))), false);
  }
  return eval;
 }

 //------------------------------------------------------
 array_view<double,2> energies_on_bz_grid(tight_binding const & TB, size_t n_pts) {

  auto TK = Fourier(TB);
  const size_t norb=TB.lattice().n_orbitals();
  const size_t ndim=TB.lattice().dim();
  grid_generator grid(ndim,n_pts);
  array<double,2> eval(norb,grid.size());
  for (; grid ; ++grid)  {
   eval(range(),grid.index()) = linalg::eigenvalues( TK( (*grid) (range(0,ndim))), false);
  }
  return eval;
 }

 //------------------------------------------------------

 std::pair<array<double,1>, array<double,2> > dos(tight_binding const & TB, size_t nkpts, size_t neps) {

  // The Fourier transform of TK
  // auto TK = Fourier(TB); // C++0x .... 
  auto TK = Fourier(TB);

  // loop on the BZ
  const size_t ndim=TB.lattice().dim();
  const size_t norb=TB.lattice().n_orbitals();
  grid_generator grid(ndim,nkpts);
  array<double,1> tempeval(norb);
  array<dcomplex,3> evec(norb,norb,grid.size());
  array<double,2> eval(norb,grid.size());
  if (norb ==1) 
   for (; grid ; ++grid)  {
    double ee = real(TK( (*grid) (range(0,ndim)))(0,0));
    eval(0,grid.index()) =ee;
    evec(0,0,grid.index()) =1;
   }
  else 
   for (; grid ; ++grid)  {
    //cerr<<" index = "<<grid.index()<<endl;
    array_view <double,1> eval_sl = eval(range(),grid.index());
    array_view <dcomplex,2> evec_sl =  evec(range(),range(),grid.index());
    std::tie (eval_sl,evec_sl) = linalg::eigenelements( TK( (*grid) (range(0,ndim)))); //,  true);
    //cerr<< " point "<< *grid <<  " value "<< eval_sl<< endl; //" "<< (*grid) (range(0,ndim)) << endl;
   }

  // define the epsilon mesh, etc.
  array<double,1> epsilon(neps); 
  double epsmax = tqa::max_element(eval);
  double epsmin = tqa::min_element(eval);
  double deps=(epsmax-epsmin)/neps;
  //for (size_t i =0; i< neps; ++i) epsilon(i)= epsmin+i/(neps-1.0)*(epsmax-epsmin);
  for (size_t i =0; i< neps; ++i) epsilon(i)=epsmin+(i+0.5)*deps;

  // bin the eigenvalues according to their energy
  // NOTE: a is defined as an integer. it is the index for the DOS.
  //REPORT <<"Starting Binning ...."<<endl;
  array<double,2> rho (neps,norb);rho()=0;
  for(size_t l=0;l<norb;l++){
   for (size_t j=0;j<grid.size();j++){
    for (size_t k=0;k<norb;k++){
     int a=int((eval(k,j)-epsmin)/deps);
     if(a==int(neps)) a=a-1;
     rho(a,l) += real(conj(evec(l,k,j))*evec(l,k,j));
     //dos(a) +=  real(conj(evec(l,k,j))*evec(l,k,j));
    }
   }
  }
  //rho = rho / double(grid.size()*deps);
  rho /= grid.size()*deps;
  return std::make_pair( epsilon, rho);
 }

 //----------------------------------------------------------------------------------

 std::pair<array<double,1>, array<double,1> > dos_patch(tight_binding const & TB, const array<double,2> & triangles, size_t neps, size_t ndiv) {
  // WARNING: This version only works for a single band Hamiltonian in 2 dimensions!!!!
  // triangles is an array of points defining the triangles of the patch
  // neps in the number of bins in energy
  // ndiv in the number of divisions used to divide the triangles

  //const size_t ndim=TB.lattice().dim();
  //const size_t norb=TB.lattice().n_orbitals();
  int ntri = triangles.shape(0)/3;
  array<double,1> dos(neps);

  // Check consistency
  const size_t ndim=TB.lattice().dim();
  //const size_t norb=TB.lattice().n_orbitals();
  if (ndim !=2)  TRIQS_RUNTIME_ERROR<<"dos_patch : dimension 2 only !";
  if (triangles.shape(1) != ndim) TRIQS_RUNTIME_ERROR<<"dos_patch : the second dimension of the 'triangle' array in not "<<ndim;

  // Every triangle has ndiv*ndiv k points
  size_t nk = ntri*ndiv*ndiv;
  size_t k_index = 0;
  double epsmax=-100000,epsmin=100000;
  array<dcomplex,2> thop(1,1);
  array<double,1> energ(nk), weight(nk);

  // a, b, c are the corners of the triangle
  // g the center of gravity taken from a
  array<double,1> a(ndim), b(ndim), c(ndim), g(ndim), rv(ndim);
  int pt = 0;
  double s, t;

  // The Fourier transform of TK
  auto TK = Fourier(TB); 

  // loop over the triangles
  for (int tri = 0; tri < ntri; tri++) {
   a = triangles(pt,range());
   pt++;
   b = triangles(pt,range());
   pt++;
   c = triangles(pt,range());
   pt++;
   g = ((a+b+c)/3.0-a)/double(ndiv);

   // the area around a k point might be different from one triangle to the other
   // so I use it to weight the sum in the dos
   double area = abs(0.5*((b(0)-a(0))*(c(1)-a(1))-(b(1)-a(1))*(c(0)-a(0)))/(ndiv*ndiv));

   for (size_t i = 0; i<ndiv; i++) {
    s = i/double(ndiv);
    for (size_t j = 0; j<ndiv-i; j++) {
     t = j/double(ndiv);
     for (size_t k = 0; k<2; k++) {

      rv = a+s*(b-a)+t*(c-a)+(k+1.0)*g;

      if(k==0 || j < ndiv-i-1) {

       energ(k_index)  = real(TK(rv)(0,0)); 
       //compute(rv);
       //energ(k_index) = real(tk_for_eval(1,1)); //tk_for_eval is Fortran array
       weight(k_index) = area;
       if (energ(k_index)>epsmax) epsmax=energ(k_index);
       if (energ(k_index)<epsmin) epsmin=energ(k_index);
       k_index++;
      }
     }
    }
   }
  }
  // check consistency
  assert(k_index == nk);

  // define the epsilon mesh, etc.
  array<double,1> epsilon(neps); 
  double deps=(epsmax-epsmin)/neps;
  for (size_t i =0; i< neps; ++i) epsilon(i)= epsmin+i/(neps-1.0)*(epsmax-epsmin);

  // bin the eigenvalues according to their energy
  int ind;
  double totalweight(0.0);
  dos() = 0.0;
  for (size_t j = 0; j < nk; j++) {
   ind=int((energ(j)-epsmin)/deps);
   if (ind == int(neps)) ind--;
   dos(ind) += weight(j);
   totalweight += weight(j);
  }
  dos /= deps;// Normalize the DOS
  return std::make_pair(epsilon, dos);
 }





 }}

