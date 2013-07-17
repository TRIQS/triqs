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
#ifndef TRIQS_UTILITY_REPORT_STREAM_H
#define TRIQS_UTILITY_REPORT_STREAM_H
#include <ostream>
#include <sstream>

namespace triqs { namespace utility {

 /**
  * \brief Output stream with flexible verbosity level.
  *
  * This class behaves pretty much like a standard ostream but you can also
  * set some verbosity level
  *
  * Example:
  *
  * report_stream rep(2);        // this creates a report stream w/ verbosity 2
  * rep << "Hello" << endl;      // this prints Hello
  * rep(2) << "Hello2" << endl;   // this prints Hello2
  * rep(3) << "Hello3" << endl;   // this doesn't print anything because the verbosity < 3
  *
  */
 class report_stream {

  std::ostream * out;
  int verbosity;

  public:

  report_stream(std::ostream * out_, int verbosity_ = 1): out(out_), verbosity(verbosity_) {}

  report_stream operator () (int n) {
   return report_stream(out, verbosity-n+1);
  }

  template<class T>
   report_stream & operator << (T const & x) {
    if (verbosity > 0) (*out)<<x;
    return *this;
   }

  // this is the type of std::cout
  typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

  // this is the function signature of std::endl
  typedef CoutType& (*StandardEndLine)(CoutType&);

  // define an operator<< to take in std::endl
  report_stream & operator << (StandardEndLine manip) {
   // call the function, but we cannot return it's value
   if (verbosity > 0) manip(*out);
   return *this;
  }

 };

}}
#endif
