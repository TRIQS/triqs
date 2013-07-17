include "arrays.pxd"
include "dcomplex.pxd"

cdef extern from "<triqs/arrays/matrix_stack_view.hpp>" namespace "triqs::arrays":

  cdef cppclass matrix_stack "triqs::arrays::matrix_stack_view" [T]:

    matrix_stack(object)

    void iadd "operator +=" (matrix_view[T] &) except +
    void isub "operator -=" (matrix_view[T] &) except +

    void imul "operator *=" (matrix_view[T] &) except +
    void idiv "operator /=" (matrix_view[T] &) except +

    void invert() except +
    void onsite_matmul_L_R(matrix_view[T] &, matrix_stack &, matrix_view[T] &) except +
