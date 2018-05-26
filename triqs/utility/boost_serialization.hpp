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
#ifndef TRIQS_SERIAL_H
#define TRIQS_SERIAL_H
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/view_tools.hpp>
//#include <vector>
//#include <string>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

namespace triqs {

  // serialize obj into an std::string
  template <typename T> std::string serialize(T const &obj) {
    std::string serial_str;
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string>> s(inserter);
    //boost::archive::binary_oarchive oa(s);
    boost::archive::text_oarchive oa(s);
    oa << obj;
    s.flush();
    return serial_str;
  }

  template <typename T> T deserialize(std::string const &serial_str) {
    T obj;
    // wrap buffer inside a stream and deserialize serial_str into obj
    boost::iostreams::basic_array_source<char> device(serial_str.data(), serial_str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char>> s(device);
    //boost::archive::binary_iarchive ia(s);
    boost::archive::text_iarchive ia(s);
    ia >> obj;
    return obj;
  }

  template <typename T> void deserialize_into_view(std::string const &serial_str, T &x) {
    typename regular_type_if_exists_else_type<T>::type obj;
    // wrap buffer inside a stream and deserialize serial_str into obj
    boost::iostreams::basic_array_source<char> device(serial_str.data(), serial_str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char>> s(device);
    boost::archive::text_iarchive ia(s);
    //boost::archive::binary_iarchive ia(s);
    ia >> obj;
    x = typename view_type_if_exists_else_type<T>::type(obj);
  }
} // namespace triqs
#endif
