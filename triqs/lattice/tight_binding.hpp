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
#ifndef TRIQS_LATTICE_TIGHTBINDINGS_H
#define TRIQS_LATTICE_TIGHTBINDINGS_H
#include "bravais_lattice_and_brillouin_zone.hpp"

namespace triqs { namespace lattice_tools { 

 /**
   For tightbinding Hamiltonian with fully localised orbitals
   Model the ShortRangeFunctionOnBravaisLattice concept.
   Overlap between orbital is taken as unit matrix. 
  */
 class tight_binding { 
   typedef std::vector<std::pair<std::vector<long>, matrix<dcomplex>>> displ_value_stack_t;
   displ_value_stack_t displ_value_stack;
   bravais_lattice bl_;
  public : 
   typedef std::vector<long>                                      arg_type;

   ///
   tight_binding (bravais_lattice const & bl) : bl_(bl) {};
  
   /// Underlying lattice 
   bravais_lattice const & lattice() const { return bl_;}

   /// Number of bands, i.e. size of the matrix t(k)
   size_t n_bands() const { return bl_.n_orbitals();}
   
   /**
    * Push_back mechanism of a pair displacement -> matrix
    * VectorIntType is anything from which a std::vector<long> can be constructed
    * MatrixDComplexType is anything from which a matrix<dcomplex> can be constructed
    */
   template<typename VectorIntType, typename MatrixDComplexType>
    void push_back(VectorIntType && v, MatrixDComplexType && m) {
     std::vector<long> V(std::forward<VectorIntType>(v)); 
     if (v.size() != bl_.dim()) TRIQS_RUNTIME_ERROR<<"tight_binding : displacement of incorrect size : got "<< v.size() << "instead of "<< bl_.dim();
     matrix<dcomplex> M(std::forward<MatrixDComplexType>(m));
     if (first_dim(M) != n_bands()) TRIQS_RUNTIME_ERROR<<"tight_binding : the first dim matrix is of size "<<  first_dim(M) <<" instead of "<< n_bands();
     if (second_dim(M) != n_bands()) TRIQS_RUNTIME_ERROR<<"tight_binding : the first dim matrix is of size "<<  second_dim(M) <<" instead of "<< n_bands();
     displ_value_stack.push_back(std::make_pair(std::move(V), std::move(M)));
    }

   void reserve(size_t n) { displ_value_stack.reserve(n);}

   // iterators
   typedef displ_value_stack_t::const_iterator const_iterator; 
   typedef displ_value_stack_t::iterator iterator; 
   const_iterator begin() const { return displ_value_stack.begin();} 
   const_iterator end() const   { return displ_value_stack.end();} 
   iterator begin() { return displ_value_stack.begin();} 
   iterator end()   { return displ_value_stack.end();} 
 };

 /**
   Factorized version of hopping (for speed)
   k_in[:,n] is the nth vector
   In the result, R[:,:,n] is the corresponding hopping t(k)
   */
 array_view <dcomplex,3> hopping_stack (tight_binding const & TB, array_view<double,2> const & k_stack);
 // not optimal ordering here

 std::pair<array<double,1>, array<double,2> > dos(tight_binding const & TB, size_t nkpts, size_t neps); 
 std::pair<array<double,1>, array<double,1> > dos_patch(tight_binding const & TB, const array<double,2> & triangles, size_t neps, size_t ndiv);
 array_view<double,2> energies_on_bz_path(tight_binding const & TB, K_view_type K1, K_view_type K2, size_t n_pts);
 array_view<double,2> energies_on_bz_grid(tight_binding const & TB, size_t n_pts);

}}

#endif


