from wrap_generator import *
import re

module = module_(full_name = "pytriqs.gf.singularities", doc = "Wrapped ...", app_name="triqs")
module.use_module('lattice_tools')
module.add_include("<triqs/gfs.hpp>")
module.add_include("<triqs/gfs/singularity/fit_tail.hpp>")
#module.add_include("<triqs/gfs/transform/pade.hpp>")
#module.add_include("<triqs/gfs/legacy_for_python_api.hpp>")
module.add_include("<triqs/python_tools/converters/string.hpp>")
module.add_include("<triqs/python_tools/converters/arrays.hpp>")
module.add_include("<triqs/python_tools/converters/h5.hpp>")
module.add_include("<triqs/python_tools/converters/vector.hpp>")
module.add_include("<triqs/python_tools/converters/function.hpp>")
module.add_include("<triqs/python_tools/converters/gf.hpp>")
module.add_using("namespace triqs::arrays")
module.add_using("namespace triqs::gfs")
module.add_using("triqs::utility::mini_vector")
module.add_preamble("""
""")

########################
##   Indices
########################

# Wrapping indices
c = class_( py_type = "GfIndices",
        c_type = "gf_indices",
        c_type_absolute = "triqs::gfs::gf_indices",
        hdf5 = True,
        serializable= "tuple",
       )

c.add_constructor("(std::vector<std::vector<std::string>> indices)", doc = "Constructs indices, from a list(list(string)))")
c.add_property(getter = cfunction("std::vector<std::vector<std::string>> data()"), doc = "Get a copy of the list of list of strings")
c.add_method("gf_indices transpose()", doc = "Transposing")
c.add_method("triqs::arrays::range convert_index(std::string s, int i)", doc = "index -> position conversion")
c.add_method_copy()
c.add_getitem(signature = "std::vector<std::string> operator[](int d)", doc = "The list of indices for dimension d")

c.add_len(calling_pattern = "int result = self_c.rank()", doc = "")
c.add_iterator(c_cast_type = "std::vector<std::string>")

module.add_class (c)

########################
##   TailGf
########################

# FIXME : Names are not regular
for Target, Rvt, Rt, ext, n in zip(['scalar_valued', 'matrix_valued', 'tensor_valued<3>', 'tensor_valued<4>'],
                          ['dcomplex', 'matrix_view<dcomplex>', 'array_view<dcomplex,3>', 'array_view<dcomplex,4>'],
                          ['dcomplex', 'matrix<dcomplex>', 'array<dcomplex,3>', 'array<dcomplex,4>'],
                          ['_s', '', 'Tv3', 'Tv4'],
                          [0,2,3,4]):
   
    c_type = '__tail_view<triqs::gfs::%s>'%Target

    t = class_( py_type = "TailGf"+ext,
            c_type = c_type,
            c_type_absolute = "triqs::gfs::" + c_type,
            hdf5 = True,
            serializable= "tuple",
            is_printable= True,
            arithmetic = ("algebra","dcomplex")
           )
    
    t.add_constructor(signature = "(triqs::utility::mini_vector<int,%s> target_shape, int n_order=10, int order_min=-1)"%n,
                          doc = "Constructs a new tail, of matrix size N1xN2")
 
    module.add_function("%s _make_tail_view_from_data(triqs::arrays::array_view<dcomplex, %s + 1> data)"%(c_type,n), 
                        calling_pattern = "%s result {data}"%c_type, doc = "Makes a new tail from a view of the data")

    # backward compat
    if Target == "matrix_valued":
        t.add_constructor(signature = "(int N1, int N2, int n_order=10, int order_min=-1)",
                          doc = "Constructs a new tail, of matrix size N1xN2")

    t.add_property(name = "data",
                   getter = cfunction("array_view<dcomplex,%s+1> data()"%n),
                   doc = "Access to the data array")

    #t.add_property(name = "shape", getter = cfunction("int shape()", doc = "Shape"))
    t.add_property(getter = cfunction("int n_valid_orders()"),
                   doc = "size")

    t.add_property(getter = cfunction("int order_min()"),
                   doc = "Min order of the expansion")

    t.add_property(getter = cfunction("int order_max()"),
                   doc = "Max order of the expansion")

    t.add_method(name = "has_coef",
                 calling_pattern = "bool result = (i >=self_c.order_min()) && (i<=self_c.order_max())",
                 signature = "bool(int i)",
                 doc = "")

    t.add_method(name = "invert",
                 calling_pattern = "self_c = inverse(self_c)",
                 signature = "void()",
                 doc = "Invert")

    t.add_method(name = "inverse",
                 calling_pattern = "auto result = inverse(self_c)",
                 signature = "%s()"%c_type,
                 doc = "Return inverse")

    t.add_method("%s conjugate(bool is_matsubara)"%c_type, calling_pattern = "auto result = conj(self_c, is_matsubara)")
    
    if Target == "matrix_valued":
        t.add_method("%s transpose()"%c_type, calling_pattern = "auto result = transpose(self_c)")

    if Target == "matrix_valued":
        t.add_method(name = "from_L_G_R", calling_pattern = "self_c = l * (*t) * r",
                           signature = "void (matrix<dcomplex> l, %s t, matrix<dcomplex> r)"%c_type)

    t.add_method("void zero()", doc = "Sets the expansion to 0")
    t.add_method("void reset(int n)", doc = "Sets the expansion to 0 until order n, to NaN afterwards.")

    t.add_method_copy()
    t.add_method_copy_from()

    t.add_call(calling_pattern = "auto result = evaluate(self_c,u)",
               signature = "dcomplex(dcomplex u)",
               doc = "")

    if Target == "matrix_valued":
        t.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "tail(matrix<dcomplex> x,%s y)"%c_type)
        t.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "tail(%s x,matrix<dcomplex> y)"%c_type)

    # ok, but MISSING CHECK SIZE
    t.add_getitem(signature = "%s operator()(int i)"%Rvt,
                  doc = "Returns the i-th coefficient of the expansion, or order Om^i")

    t.add_setitem(calling_pattern = "self_c(i) = m",
                  signature = "void(int i, %s m)"%Rt, # I use matrix, not view. It makes a copy, but ensure I can pass double, int, and numpy will convert.
                  doc = "Sets the i-th coefficient of the expansion, or order Om^i")

    module.add_class(t)

    # slice_target_sing for all cases
    #int_int = ', '.join('int n%s'%i for i in range(n))
    #range_range = ', '.join('range(n%s, n%s + 1)'%(i,i) for i in range(n))
    #if Target == "matrix_valued":
    #if n>0 : 
    #    #print "adding slice_target_sing", c_type, int_int, range_range
    #    module.add_function("%s slice_target_sing(%s t, %s)"%(c_type, c_type, int_int), 
    #        calling_pattern = "%s result = slice_target_sing(*t, %s)"%(c_type, range_range))

##   Code generation
module.generate_code()

