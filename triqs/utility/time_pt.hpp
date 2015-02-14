/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by M. Ferrero, O. Parcollet, I. Krivenko
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
#pragma once
#include <triqs/utility/first_include.hpp>
#include <triqs/h5.hpp>
#include <limits>
#include <iostream>
#include <cmath>

namespace triqs { namespace utility {

 struct time_segment;

 /// A point on a very thin grid, as uint64_t
 struct time_pt {

  friend time_segment;

  time_pt() { beta = 0; val = 0; n = 0; }
  //explicit time_pt(double b) { beta = b; val =0; n = 0;}

  private : 
  // too dangerous because of rounding to be left public
  time_pt(double v, double beta_) { beta = beta_; val = v; n = std::floor(Nmax*(v/beta));}
  time_pt(uint64_t n_, double beta_, bool) { beta = beta_; val = beta*(double(n_)/Nmax); n = n_;}
  
  public : 

  time_pt (time_pt const &) = default;
  time_pt (time_pt && x) = default;
  time_pt & operator = (time_pt const &) = default ;
  time_pt & operator = (time_pt && x) = default;

  // this is also dangerous for reproducibility
  time_pt & operator = (double v) = delete;

  bool operator == (const time_pt & tp) const { return n == tp.n; }
  bool operator != (const time_pt & tp) const { return n != tp.n; }
  bool operator <  (const time_pt & tp) const { return n < tp.n; }
  bool operator <= (const time_pt & tp) const { return n <= tp.n; }
  bool operator >  (const time_pt & tp) const { return n > tp.n; }
  bool operator >= (const time_pt & tp) const { return n >= tp.n; }

  // adding and substracting is cyclic on [0, beta]
  inline friend time_pt operator+(time_pt const & a, time_pt const & b) {
    bool wrapped = ((Nmax-std::max(a.n,b.n)) < std::min(a.n,b.n));
    if (!wrapped) return time_pt((a.n+b.n)%Nmax, a.beta, true);
    else return time_pt((a.n+b.n)+1, a.beta, true);
  }
  inline friend time_pt operator-(time_pt const & a, time_pt const & b) {
    uint64_t n = (a.n>=b.n ? a.n-b.n : Nmax-(b.n-a.n));
    return time_pt(n, a.beta, true);
  }

  // unary
  inline friend time_pt operator-(time_pt const & a) { return time_pt(Nmax-a.n, a.beta,true); }
  
  // division by integer
  inline friend time_pt div_by_int(time_pt const & a, size_t b) { return time_pt(a.n/b, a.beta, true); }
  inline friend time_pt mult_by_int(time_pt const & a, size_t b) { return time_pt(a.n*b, a.beta, true); }
  
  // floor_div(x,y) = floor (x/y), but computed on the grid.
  inline friend size_t floor_div(time_pt const & a, time_pt const & b) { return a.n/b.n; }

  // only EXPLICIT cast
  explicit operator double() const { return val; } // cast to a double

  friend std::ostream & operator<< (std::ostream & out, time_pt const & p) { return out << p.val << " [time_pt : beta = "<< p.beta<< " n = "<< p.n<<"]" ; }
 
  static constexpr uint64_t Nmax = std::numeric_limits<uint64_t>::max();

  private:
  uint64_t n;
  double val, beta;

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &TRIQS_MAKE_NVP("n", n) & TRIQS_MAKE_NVP("val", val) & TRIQS_MAKE_NVP("beta", beta);
  }

  /// Write into HDF5
  //friend void h5_write (h5::group fg, std::string name, time_pt const & g) = delete; // not implemented

  /// Read from HDF5
  //friend void h5_read(h5::group fg, std::string name, time_pt &g) = delete; // not implemented
 };

 // all operations below decay to double
 inline double operator*(time_pt const & a, time_pt const & b) { return double(a)*double(b); }
 inline double operator/(time_pt const & a, time_pt const & b) { return double(a)/double(b); }

#define IMPL_OP(OP) \
 inline double operator OP(time_pt const & x, double y) {return static_cast<double>(x) OP y;} \
 inline double operator OP(double y, time_pt const & x) {return y OP static_cast<double>(x);} 
 IMPL_OP(+); IMPL_OP(-); IMPL_OP(*); IMPL_OP(/);
#undef IMPL_OP


 // a segment [0, beta[ in time
 struct time_segment {

  const double beta;

  time_segment(double beta_) : beta(beta_) {}

  // get a random point in [0, tp[
  template <typename RNG> time_pt get_random_pt(RNG &rng, time_pt tp) const { return time_pt(rng(tp.n), beta, true); }
  template <typename RNG> time_pt get_random_pt(RNG &rng) const { return time_pt(rng(time_pt::Nmax), beta, true); }

  // get special points
  time_pt get_upper_pt() const { return time_pt(time_pt::Nmax, beta, true); }
  time_pt get_lower_pt() const { return time_pt(0, beta, true); }
  time_pt get_epsilon() const { return time_pt(1, beta, true); }
 };
}}

