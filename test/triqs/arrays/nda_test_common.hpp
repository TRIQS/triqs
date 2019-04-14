#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <cmath>
#include <limits>
#include <triqs/arrays.hpp>

using triqs::arrays::array;

// FIXME replace by shape_t when the lib is cleaned
// currently triqs::arrays::shape_t is a minivec of long, and the shape of an array is a mininvec of size_t
// FIXME
template <int R> using myshape_t = mini_vector<size_t, R>;

// variables for the test
range _;
ellipsis ___;


