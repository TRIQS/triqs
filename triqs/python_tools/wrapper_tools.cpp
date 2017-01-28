#include <triqs/utility/first_include.hpp>
#ifdef TRIQS_WITH_PYTHON_SUPPORT

// REMOVE THIS 
#include <triqs/arrays.hpp>

#include "./wrapper_tools.hpp"
#include "./converters/function.hpp"
#include "./converters/h5.hpp"
namespace triqs { namespace py_tools { 

pyref py_converter<triqs::h5::group>::group_type;

}}

#endif

