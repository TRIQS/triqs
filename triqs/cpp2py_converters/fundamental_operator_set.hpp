#pragma once
#include "./generics.hpp"
#include <triqs/hilbert_space/fundamental_operator_set.hpp>

namespace cpp2py {

 // converters to/from a vector<vector<variant<int,string>>> using existing converters and the
 // reduction of fundamental_operator_set to/from it.
 template <>
 struct py_converter<triqs::hilbert_space::fundamental_operator_set>
     : py_converter_generic_cast_construct<triqs::hilbert_space::fundamental_operator_set,
                                           triqs::hilbert_space::fundamental_operator_set::reduction_t> {};
}

