/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018, N. Wentzell, I. Krivenko, O. Parcollet
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

#include <variant>

namespace std {

  // == ostream operator<< for variant and vector of variant

  template <typename T1, typename... T> std::ostream &operator<<(std::ostream &os, std::variant<T1, T...> const &v) {
    visit([&os](auto const &x) { os << x; }, v);
    return os;
  }

  inline std::ostream &operator<<(std::ostream &os, std::vector<std::variant<int, std::string>> const &fs) {
    int u = 0;
    for (auto const &i : fs) {
      if (u++) os << ",";
      os << i;
    }
    return os;
  }

  // == Make std::to_string available for both string and variant

  inline string to_string(string const &str) { return str; }
  template <typename T1, typename T2> inline string to_string(variant<T1, T2> const &var) {
    stringstream ss;
    ss << var;
    return ss.str();
  }

} // namespace std
