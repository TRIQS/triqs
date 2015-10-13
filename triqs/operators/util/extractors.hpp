#pragma once
#include <triqs/utility/first_include.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/arrays.hpp>

namespace triqs { namespace operators { namespace util {
using namespace triqs::utility;
using namespace triqs;
using namespace triqs::hilbert_space;

using Op = many_body_operator<double>;
using U_dict_t = std::map<std::pair<Op::indices_t,Op::indices_t>, double>;
using gf_struct_t = std::map<std::string, std::vector<triqs::utility::variant_int_string>> ;
using triqs::arrays::matrix;

///return dictionary of density-density interactions starting from a hamiltonian
/**
  * throw exception if non-density-density terms
  * otherwise return std::map<std::pair<op::indices_t,op::indices_t>, double>
  */
U_dict_t extract_U_dict2(Op const & h, bool ignore_irrelevant=false);

///converts dictionary of interactions to matrix, given the structure of the gf
matrix<double> dict_to_matrix(U_dict_t const & U_dict, gf_struct_t const & gf_struct);

}}}
