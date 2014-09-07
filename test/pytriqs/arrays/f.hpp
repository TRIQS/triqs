#include <triqs/arrays.hpp>

using namespace triqs::arrays;

void f(array_view<long,3> A) {
 
 std::cout  << " A(range(),range(),0) = "<< A(range(),range(),0) <<std::endl;
 std::cout  << " A(range(),range(),1) = "<< A(range(),range(),1) <<std::endl;
 std::cout  << " memory_layout (permutation of indices) = "<< A.indexmap().get_memory_layout() << std::endl;
 std::cout  << " strides = "<< A.indexmap().strides() <<std::endl;
 std::cout  << " is_contiguous = "<< A.indexmap().is_contiguous() <<std::endl;

 A(range(),range(),1) *=2;

}

