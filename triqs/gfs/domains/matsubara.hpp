/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_GF_DOMAIN_MATSUBARA_H
#define TRIQS_GF_DOMAIN_MATSUBARA_H
#include "../tools.hpp"

namespace triqs { namespace gfs { 

 /// The domain
 template<bool IsComplex>
 struct matsubara_domain {
  typedef typename mpl::if_c<IsComplex, std::complex<double>, double>::type point_t;
  double beta;
  statistic_enum statistic;
  matsubara_domain (double Beta=1, statistic_enum s = Fermion): beta(Beta), statistic(s){
    if(beta<0)TRIQS_RUNTIME_ERROR<<"Matsubara domain construction :  beta <0 : beta ="<< beta <<"\n";
  }
  matsubara_domain(matsubara_domain const &) = default;
  matsubara_domain(matsubara_domain<!IsComplex> const &x): beta(x.beta), statistic(x.statistic) {}
  bool operator == (matsubara_domain const & D) const { return ((std::abs(beta - D.beta)<1.e-15) && (statistic == D.statistic));}

  /// Write into HDF5
  friend void h5_write (h5::group fg, std::string subgroup_name, matsubara_domain const & d) {
   h5::group gr =  fg.create_group(subgroup_name);
   h5_write(gr,"beta",d.beta);
   h5_write(gr,"statistic",(d.statistic==Fermion ? "F" : "B"));
  }

  /// Read from HDF5
  friend void h5_read  (h5::group fg, std::string subgroup_name, matsubara_domain & d){
   h5::group gr = fg.open_group(subgroup_name);
   double beta; std::string statistic;
   h5_read(gr,"beta",beta);
   h5_read(gr,"statistic",statistic);
   d = matsubara_domain(beta,(statistic=="F" ? Fermion : Boson));
  }
 
  //  BOOST Serialization
  friend class boost::serialization::access;
  template<class Archive>
   void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::make_nvp("beta",beta);
    ar & boost::serialization::make_nvp("statistic",statistic);
   }

 };

}}
#endif

