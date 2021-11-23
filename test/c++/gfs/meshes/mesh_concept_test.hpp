
#pragma once

#include <triqs/mesh/mesh_concepts.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <h5/h5.hpp>
#include <iterator>

#include <string_view>

using namespace triqs::mesh;

template <Domain T> constexpr auto type_name_domain() {
  std::string_view name;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
#endif
  return name;
}
