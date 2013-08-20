
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
#ifndef TRIQS_LATTICE_FUNCTORS_H
#define TRIQS_LATTICE_FUNCTORS_H

namespace triqs { namespace lattice_tools { 

 const double pi = acos(-1.0);
 const std::complex<double> I(0,1);

 template<typename F> struct minus_chech_impl {
  typedef typename F::arg_type arg_type;
  typedef typename F::return_type return_type;
  F f;
  minus_chech_impl(F const & f_):f(f_){}
  brillouin_zone const & bz() const {return f.bz();}
  return_type operator()(arg_type const & x) const { return_type res(f(x)); res *=-1; return res;} 
 };

 namespace result_of { template<typename F> struct minus_chech{ typedef minus_chech_impl<F> type;}; }

 /**
  *  Given f of type F which models FunctionOnBravaisLattice, minus_check(f) : 
  *   - returns -f(-args)
  *   - its type models Function
  *
  */
 template<typename F> minus_chech_impl<F> minus_chech(F const & f) { return minus_chech_impl<F> (f);}

 template<typename F >//, typename Enabler = boost::enable_if< Tag::check<Tag::ShortRangeFunctionOnBravaisLattice, F> > >
  class fourier_impl {
   F f;
   brillouin_zone bz_;
  // deduce the return type from decltype(begin()->second)
   public: 
   typedef typename regular_type_if_exists_else_type< decltype(f.begin()->second)>::type return_construct_type;
   typedef typename view_type_if_exists_else_type<return_construct_type>::type return_type;
   typedef K_view_type arg_type;

   fourier_impl (F f_):f(f_), bz_(f_.lattice()), res(f.n_bands(),f.n_bands()) {}

   //brillouin_zone const & bz() const { return bz_; }

   return_type operator()(K_view_type const & k) {
    res()=0;
    for (auto const & pdm : f) { res += pdm.second * exp( 2*pi*I* this->dot_product(k,pdm.first)); }
    return res;
   }

   protected:
   inline double dot_product(K_view_type const & a, typename F::arg_type const & b) const {
    assert(b.size()>= this->bz_.lattice().dim()); 
    double r=0; for (size_t i=0; i< this->bz_.lattice().dim();++i) r += a(i) * b[i];
    return r;
   }
   return_construct_type res;
   //typename F::return_construct_type res;
  };

 /**
  *  Given f of type F which models ShortRangeFunctionOnBravaisLattice, Fourier(f) returns 
  *   - a type that models FunctionOnBravaisLattice
  *   - and returns the Fourier transform f(k)
  */
 template<typename F> fourier_impl<F> Fourier(F f) { return fourier_impl<F> (f);}
 //namespace result_of { template<typename F> struct Fourier { typedef fourier_impl<F> type;}; }

}}

#endif

