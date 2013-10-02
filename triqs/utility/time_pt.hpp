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
#ifndef TRIQS_CTQMC_KRYLOV_TIME_PT_HPP
#define TRIQS_CTQMC_KRYLOV_TIME_PT_HPP

#include <triqs/utility/first_include.hpp>
#include <limits>
#include <iostream>
#include <cmath>

namespace triqs { namespace utility {

 /// A point on a very thin grid, as uint64_t
 struct time_pt {

  time_pt() { beta = 1; val =0; n = 0;}
  explicit time_pt(double b) { beta = b; val =0; n = 0;}

  private : 
  // too dangerous because of rounding to be left public
  time_pt(double v, double beta_) { beta = beta_; val =v; n = std::floor(Nmax*(v/beta));}
  time_pt(uint64_t n_, double beta_, bool) { beta = beta_; val = beta*(double(n_)/Nmax); n = n_;}

  public : 

 // random case : to be improved, using rng only for integer for reproducibility....
  template<typename RNG, typename T1, typename T2> static time_pt random(RNG & rng, T1 l, T2 beta_) { return time_pt(rng(double(l)), double(beta_));}

  time_pt (time_pt const &) = default;
  time_pt (time_pt && x) = default;
  time_pt & operator = (time_pt const &) = default ;
  time_pt & operator = (time_pt && x) = default;

  //this is also dangerous for reproducibility
  time_pt & operator = (double v) = delete;

  // factories
  static time_pt make_beta(double beta_) { time_pt r; r.beta = beta_; r.n = Nmax; r.val = beta_; return r;}
  static time_pt make_zero(double beta_) { time_pt r; r.beta = beta_; r.n = 0; r.val = 0; return r;}
  static time_pt make_from_double(double x, double beta_) { return time_pt(x,beta_);}
  static time_pt epsilon(double beta) { return time_pt(1,beta,true);}
  static time_pt epsilon(time_pt const & beta) { return time_pt(1,beta.beta,true);}
 
  bool operator == (const time_pt & tp) const { return n == tp.n; }
  bool operator != (const time_pt & tp) const { return n != tp.n; }
  bool operator <  (const time_pt & tp) const { return n < tp.n; }
  bool operator <= (const time_pt & tp) const { return n <= tp.n; }
  bool operator >  (const time_pt & tp) const { return n > tp.n; }
  bool operator >= (const time_pt & tp) const { return n >= tp.n; }

  // adding and substracting is cyclic on [0, beta]
  inline friend time_pt operator+(time_pt const & a, time_pt const & b) { return time_pt(a.n + b.n, a.beta, true); }
  inline friend time_pt operator-(time_pt const & a, time_pt const & b) { uint64_t n = (a.n>= b.n ? a.n - b.n : Nmax - (b.n - a.n)); return time_pt(n, a.beta,true); }

  //unary
  inline friend time_pt operator-(time_pt const & a) { uint64_t n = Nmax - a.n; return time_pt(n, a.beta,true); }
  
  // division by integer
  inline friend time_pt div_by_int(time_pt const & a, size_t b) { return time_pt(a.n/ b, a.beta, true); }
  
  // floor_div(x,y) = floor (x/y), but computed on the grid.
  inline friend size_t floor_div(time_pt const & a, time_pt const & b){return a.n/b.n;}

  // only EXPLICIT cast
  explicit operator double() const {return val;} // cast to a double

  friend std::ostream & operator<< (std::ostream & out, time_pt const & p) { return out << p.val << " [time_pt : beta = "<< p.beta<< " n = "<< p.n<<"]" ; }
 
  private:
  static constexpr uint64_t Nmax = std::numeric_limits<uint64_t>::max();
  uint64_t n;
  double val, beta;
 };

  // all operations below decay to double
  inline double operator*(time_pt const & a, time_pt const & b) { return double(a)*double(b); }
  inline double operator/(time_pt const & a, time_pt const & b) { return double(a)/double(b); }

#define IMPL_OP(OP) \
  inline double operator OP(time_pt const & x, double y) {return static_cast<double>(x) OP y;} \
  inline double operator OP(double y, time_pt const & x) {return y OP static_cast<double>(x);} 
  IMPL_OP(+); IMPL_OP(-); IMPL_OP(*); IMPL_OP(/);
#undef IMPL_OP


  // all other operations : first cast into a double and do the operation
/*#define IMPL_OP(OP) \
  template<typename T> auto operator OP(time_pt const & x, T y) -> decltype(double(0) OP y) {return static_cast<double>(x) OP y;} \
  template<typename T> auto operator OP(T y, time_pt const & x) -> decltype(y OP double(0)) {return y OP static_cast<double>(x);} \
  IMPL_OP(+); IMPL_OP(-); IMPL_OP(*); IMPL_OP(/);
#undef IMPL_OP*/

}}
#endif

