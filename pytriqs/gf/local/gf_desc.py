from wrap_generator import *
import re

module = module_(full_name = "pytriqs.gf.local.gf", doc = "Local Green functions ...", app_name="triqs")
module.use_module('lattice_tools')
module.add_include("<triqs/gfs.hpp>")
module.add_include("<triqs/gfs/functions/functions.hpp>")
module.add_include("<triqs/gfs/singularity/fit_tail.hpp>")
module.add_include("<triqs/gfs/transform/pade.hpp>")
module.add_include("<triqs/gfs/legacy_for_python_api.hpp>")
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
namespace triqs { namespace gfs {
template <int R> struct gf_h5_name<legendre, tensor_valued<R>, tail_zero<typename tensor_valued<R>::value_type>> :
gf_h5_name<legendre, tensor_valued<R>, nothing> {};
}}
""")

########################
##   TailGf
########################

t = class_( py_type = "TailGf",
        c_type = "tail_view",
        c_type_absolute = "triqs::gfs::tail_view",
        serializable= "tuple",
        is_printable= True,
        arithmetic = ("algebra","double")
       )

t.add_constructor(signature = "(int N1, int N2, int n_order=10, int order_min=-1)",
                  doc = "Constructs a new tail, of matrix size N1xN2, with n_order expansion starting at order_min")

t.add_property(name = "data",
               getter = cfunction("array_view<dcomplex,3> data()"),
               doc = "Access to the data array")

#t.add_property(name = "shape", getter = cfunction("int shape()", doc = "Shape"))
t.add_property(getter = cfunction("int size()"),
               doc = "size")

t.add_property(getter = cfunction("int order_min()"),
               doc = "Min order of the expansion")

t.add_property(getter = cfunction("int order_max()"),
               doc = "Max order of the expansion")

t.add_property(name = "mask",
               getter = cfunction("array_view<int,2> mask()"),
               doc = "Access to the mask")

t.add_method(name = "has_coef",
             calling_pattern = "bool result = (i >=self_c.order_min()) && (i<=self_c.order_max())",
             signature = "bool(int i)",
             doc = "A method which did not exist in C++")

# strange, I should not quality : ADL ??
t.add_method(name = "invert",
             calling_pattern = "self_c = inverse(self_c)",
             signature = "void()",
             doc = "Invert")

t.add_method(name = "zero",
             calling_pattern = "self_c = 0",
             signature = "void()",
             doc = "Sets the expansion to 0")

t.add_method_copy()
t.add_method_copy_from()

t.add_call(calling_pattern = "auto result = evaluate(self_c,u)",
           signature = "dcomplex(dcomplex u)",
           doc = "")

t.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "tail(matrix<dcomplex> x,tail_view y)")
t.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "tail(tail_view x,matrix<dcomplex> y)")

# ok, but MISSING CHECK SIZE
t.add_getitem(signature = "matrix_view<dcomplex> operator()(int i)",
              doc = "Returns the i-th coefficient of the expansion, or order Om^i")

t.add_setitem(calling_pattern = "self_c(i) = m",
              signature = "void(int i, matrix<dcomplex> m)", # I use matrix, not view. It makes a copy, but ensure I can pass double, int, and numpy will convert.
              doc = "Sets the i-th coefficient of the expansion, or order Om^i")

module.add_class(t)

# Change C++ to make the same
#   def __repr__ (self) :
#        omin = self.order_min
#        while ((omin <= self.order_max) and (numpy.max(numpy.abs(self.data[omin-self.order_min,:,:])) < 1e-8)):
#          omin = omin+1
#        if omin == self.order_max+1:
#          return "%s"%numpy.zeros(self.shape)
#        else:
#          return string.join([ "%s"%self[r]+ (" /" if r>0 else "") + " Om^%s"%(abs(r)) for r in range(omin, self.order_max+1) ] , " + ")
########################
##   TailZero
########################
for c_type, suffix in [("std::complex<double>","_s"),("triqs::arrays::matrix<std::complex<double>>",""),("triqs::arrays::array<std::complex<double>,3>", "_3"), ("triqs::arrays::array<std::complex<double>,4>", "_4")]:
  c = class_( py_type = "TailZero%s"%suffix,
        c_type = "tail_zero<%s>"%c_type,
        c_type_absolute = "triqs::gfs::tail_zero<%s>"%c_type,
        serializable= "tuple",
        is_printable= False,
        hdf5 = True,
        arithmetic = ("algebra","double")
       )
  c.add_method_copy()
  c.add_method_copy_from()
  module.add_class(c)

  module.add_function ("%s evaluate (tail_zero<%s> t, int i)"%(c_type, c_type), doc = """""")

########################
##   enums
########################

module.add_enum(c_name = "statistic_enum",
                c_namespace = "triqs::gfs",
                values = ["Fermion","Boson"])
module.add_enum(c_name = "matsubara_mesh_opt",
                c_namespace = "triqs::gfs",
                values = ["all_frequencies","positive_frequencies_only"])

########################
##   Mesh generic
########################

def make_mesh( py_type, c_tag, is_im=False) :
    m = class_( py_type = py_type,
            c_type = "gf_mesh<%s>"%c_tag,
            c_type_absolute = "triqs::gfs::gf_mesh<triqs::gfs::%s>"%c_tag,
            serializable= "tuple",
            is_printable= True,
            comparisons = "== !="
           )

    if is_im and not c_tag=="brillouin_zone":
        m.add_property(name = "beta",
                       getter = cfunction(calling_pattern="double result = self_c.domain().beta",
                       signature = "double()",
                       doc = "Inverse temperature"))

        m.add_property(name = "statistic",
                       getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
                       doc = "Statistic")

    m.add_len(calling_pattern = "int result = self_c.size()", doc = "Size of the mesh")
    c_cast_type = "dcomplex" if not c_tag == "brillouin_zone" else "triqs::arrays::vector<double>"
    m.add_iterator(c_cast_type = c_cast_type)

    return m

########################
##   MeshImFreq
########################

m = make_mesh( py_type = "MeshImFreq", c_tag = "imfreq", is_im = True)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max=1025)")
m.add_method("""int last_index()""")
m.add_method("""int first_index()""")

module.add_class(m)

########################
##   MeshImTime
########################

m = make_mesh(py_type = "MeshImTime", c_tag = "imtime", is_im = True)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")

module.add_class(m)

########################
##   MeshLegendre
########################

m = make_mesh( py_type = "MeshLegendre", c_tag = "legendre", is_im = True)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max=1025)")

module.add_class(m)

########################
##   MeshReFreq
########################

m = make_mesh(py_type = "MeshReFreq", c_tag = "refreq")
m.add_constructor(signature = "(double omega_min, double omega_max, int n_max)")

m.add_property(name = "omega_min",
               getter = cfunction(calling_pattern="double result = self_c.x_min()",
               signature = "double()",
               doc = "Inverse temperature"))

m.add_property(name = "omega_max",
               getter = cfunction(calling_pattern="double result = self_c.x_max()",
               signature = "double()",
               doc = "Inverse temperature"))

module.add_class(m)

########################
##   MeshReTime
########################

m = make_mesh(py_type = "MeshReTime", c_tag = "retime")
m.add_constructor(signature = "(double t_min, double t_max, int n_max)")

m.add_property(name = "t_min",
               getter = cfunction(calling_pattern="double result = self_c.x_min()",
               signature = "double()",
               doc = "Inverse temperature"))

m.add_property(name = "t_max",
               getter = cfunction(calling_pattern="double result = self_c.x_max()",
               signature = "double()",
               doc = "Inverse temperature"))

module.add_class(m)

########################
##   MeshBrillouinZone
########################

m = make_mesh( py_type = "MeshBrillouinZone", c_tag = "brillouin_zone", is_im = True)
m.add_constructor(signature = "(triqs::lattice::brillouin_zone b, int n_k)")
module.add_class(m)

########################
##   Gf Generic : common to all 5 one variable gf
########################

def make_gf( py_type, c_tag, is_im = False, has_tail = True, target_type = "matrix_valued", serializable=True) :
    """
    :param py_type: python type name
    :param c_tag: descriptor name (in c++) (e.g. imfreq)
    :param is_im: True if imaginary-time/imaginary frequency/Legendre
    :param has_tail: True if has a tail
    :param target_type: matrix_valued/tensor_valued<N>
    :param serializable: bool
    """

    rank = 2 if target_type == "matrix_valued" else int(re.compile("tensor_valued<(.*)>").match(target_type).groups()[0])

    data_type = "std::complex<double>"
    return_type = "matrix<dcomplex>" if target_type=="matrix_valued" else "array<dcomplex,%s>"%(rank)
    if has_tail :
     tail_type = "tail"
    elif rank<3:
     tail_type = "no_tail"
    else:
     tail_type = "tail_zero<triqs::arrays::array<std::complex<double>,%s>>"%rank
    g = class_(
            py_type = py_type,
            c_type = "gf_view<%s, %s, %s>"%(c_tag, target_type, tail_type),
            c_type_absolute = "triqs::gfs::gf_view<triqs::gfs::%s, triqs::gfs::%s, triqs::gfs::%s>"%(c_tag, target_type, tail_type),
            #serializable= "boost",
            serializable= "tuple" if serializable else None,
            #is_printable= True,
            #comparisons = "==",
            hdf5 = True,
            arithmetic = ("algebra",data_type, "with_inplace_operators", "with_unary_minus")
            )

    g.add_constructor(signature = "(gf_mesh<%s> mesh, mini_vector<size_t,%s> shape, std::vector<std::vector<std::string>> indices = std::vector<std::vector<std::string>>{}, std::string name = "")"%(c_tag, rank), python_precall = "pytriqs.gf.local._gf_%s.init%s"%(c_tag, "" if rank==2 else "_tv"))

    g.add_method_copy()
    g.add_method_copy_from()

    # properties
    g.add_member(c_name = "name", c_type  ="std::string",  doc = "Name of the Green function (used for plotting only)")

    if is_im and not c_tag=="brillouin_zone":
        g.add_property(name = "beta",
                       getter = cfunction(calling_pattern="double result = self_c.domain().beta", signature = "double()"),
                       doc = "Inverse temperature")

        g.add_property(name = "statistic",
                       getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
                       doc = "Statistic")

    g.add_property(getter = cfunction("gf_mesh<%s> mesh()"%c_tag),
                   doc ="The mesh")

    g.add_property(name = "data",
                   getter = cfunction(calling_pattern="auto result = self_c.data()", signature = "array_view<%s,%s>()"%(data_type,rank+1)),
                   doc ="The data ")

    g.add_property(name = "target_shape",
                   getter = cfunction(calling_pattern="auto result = get_target_shape(self_c)", signature = "shape_type()"),
                   doc = "")

    if has_tail:
        g.add_property(name = "tail",
                       getter = cfunction(c_name="singularity", signature = "tail_view()"),
                       setter = cfunction(signature = "void(tail_view value)", calling_pattern="self_c.singularity() = value_c"),
                       doc ="The high frequency tail")

    g.add_property(name = "indices",
                   getter = cfunction(calling_pattern="auto result = self_c.indices()[0]", signature = "gf_indices()"),
                   doc ="The indices(L)")

    g.add_property(name = "indicesL",
                   getter = cfunction(calling_pattern="auto result = self_c.indices()[0]", signature = "gf_indices()"),
                   doc ="The indices(L)")

    g.add_property(name = "indicesR",
                   getter = cfunction(calling_pattern="auto result = self_c.indices()[1]", signature = "gf_indices()"),
                   doc ="The indices(R)")

    # backward compatibility
    g.add_property(name = "N1",
                   getter = cfunction(calling_pattern="int result = get_target_shape(self_c)[0]", signature = "int()"),
                   doc = "[Deprecated] equivalent to target_shape[0]")
    g.add_property(name = "N2",
                   getter = cfunction(calling_pattern="int result = get_target_shape(self_c)[1]", signature = "int()"),
                   doc = "[Deprecated] equivalent to target_shape[1]")

    # ()
    g.add_method(name = "__call__",
                 calling_pattern = "%s result = self_c(n)"%(return_type),
                 signature = "%s(double n)"%return_type,
                 doc = "Call operator")

    # []
    # r1, r2 can be a string, an int or slice :
    # a string :
    # an int or a slice : we ask Python to interpret the slice into a
    # range (range_from_slice) , but we need to have the length of the target of the gf to do that.
    g.add_getitem(signature = "gf_view<%s>("%c_tag+", ".join(["PyObject * r%s"%i for i in range(1,rank+1)])+")",
                  calling_pattern= """
                   auto sh = get_target_shape(self_c);
                   auto f = [&sh, &self_c](PyObject * r, int u) {
                       if (convertible_from_python<std::string>(r, false))
                         return self_c.indices().convert_index(convert_from_python<std::string>(r),u);
                       return range_from_slice(r,sh[u]);
                       };
                   auto result = slice_target(self_c,"""+""", """.join(["""f(r%s,%s)"""%(i,i-1) for i in range(1,rank+1)])+""");
                   """,
                  doc = "Returns a sliced view of the Green function")

    g.add_setitem(signature = "void("+", ".join(["PyObject* r%s"%i for i in range(1,rank+1)])+", PyObject* val)",
                  calling_pattern=
                  """
                   pyref __arg = Py_BuildValue("(OO)", """+""", """.join(["""r%s"""%i for i in range(1,rank+1)])+""");
                   pyref gs_py = PyObject_GetItem(self, __arg);  // gs = self[r1,r2]
                   pyref res = PyNumber_Lshift(gs_py, val);      // gs << val
                  """,
                  no_self_c = True, # avoid a warning
                  doc = "g[....] << what_ever : fills the slice of the Green function with what_ever")

    # Plot
    g.add_property(name = "real",
                   getter = "pytriqs.gf.local._gf_common._real_plot",
                   doc ="real option for plotting")

    g.add_property(name = "imag",
                   getter = "pytriqs.gf.local._gf_common._imag_plot",
                   doc ="imag option for plotting")

    # Lazy system
    g.add_pure_python_method("pytriqs.gf.local._gf_common.LazyCTX", rename = "__lazy_expr_eval_context__")

    # For basic ops, if the other operand is a lazy expression, build a lazy
    # expression : this is done by this little external functions, for backward
    # compatibility
    g.number_protocol['add'].python_precall      = "pytriqs.gf.local._gf_common.add_precall"
    g.number_protocol['subtract'].python_precall = "pytriqs.gf.local._gf_common.sub_precall"
    g.number_protocol['multiply'].python_precall = "pytriqs.gf.local._gf_common.mul_precall"
    g.number_protocol['divide'].python_precall   = "pytriqs.gf.local._gf_common.div_precall"

    g.number_protocol['lshift'] = pyfunction(name ="__lshift__", python_precall = "pytriqs.gf.local._gf_common._lshift_", arity = 2)

    if target_type=="matrix_valued":
      # For backward compatibility
      g.number_protocol['inplace_lshift'] = pyfunction(name ="__inplace_lshift__", python_precall = "pytriqs.gf.local._gf_common._ilshift_", arity = 2)

      g.add_method(name = "invert", calling_pattern = "invert_in_place(self_c)" , signature = "void()", doc = "Invert (in place)")

      g.add_method(name = "transpose",
                   calling_pattern = "auto result = transpose(self_c)",
                   signature = "gf<%s>()"%c_tag,
                   doc = "Returns a NEW gf, with transposed data, i.e. it is NOT a transposed view.")

      if c_tag not in [ "imtime", "legendre", "brillouin_zone"] and has_tail:
          g.add_method(name = "conjugate", calling_pattern = "auto result = conj(self_c)" , signature = "gf<%s>()"%c_tag, doc = "Return a new function, conjugate of self.")

      g.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "gf<%s>(matrix<%s> x,gf<%s> y)"%(c_tag,data_type,c_tag))
      g.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "gf<%s>(gf<%s> x,matrix<%s> y)"%(c_tag,c_tag,data_type))

#  do we really want to have the view case separately ? the matrix can not be so big.
#    g.add_method(name = "from_L_G_R",
#                 calling_pattern = "set_from_L_G_R(self_c,l,g(),r)",
#                 signature = "void(matrix_view<%s> l,gf<%s> g,matrix_view<%s> r)"%(data_type,c_tag,data_type),
#                 doc = "self << l * g * r")

      g.add_method(name = "from_L_G_R",
                   calling_pattern = "set_from_L_G_R(self_c,l,g(),r)",
                   signature = "void(matrix<%s> l,gf<%s> g,matrix<%s> r)"%(data_type,c_tag,data_type),
                   doc = "self << l * g * r")

      g.add_method(name = "zero",
                   calling_pattern = "self_c = 0",
                   signature = "void()",
                   doc = "Put the Green function to 0")

    # Pure python methods
    g.add_pure_python_method("pytriqs.gf.local._gf_%s.plot"%c_tag, rename = "_plot_")
    if c_tag != "brillouin_zone":
     g.add_pure_python_method("pytriqs.gf.local._gf_plot.x_data_view", rename = "x_data_view")

    return g

########################
##   GfImFreq
########################
for py_type, has_tail in [("GfImFreq", True) ,("GfImFreqNoTail",False)]:
 g = make_gf(py_type = py_type, c_tag = "imfreq", is_im = True, has_tail = has_tail)
 tail_type = "tail" if has_tail else "no_tail"

 if has_tail :
  g.add_method(name = "make_real_in_tau",
              calling_pattern = "auto result = make_real_in_tau(self_c)",
              signature = "gf_view<imfreq, matrix_valued, tail>()",
              doc = "Ensures that the Fourier transform of the Gf, in tau, is real, hence G(-i \omega_n)* =G(i \omega_n)")

  g.add_method(name = "density",
              calling_pattern = "auto result = density(self_c)",
              signature = "matrix_view<double>()",
              doc = "Density, as a matrix, computed from a Matsubara sum")

  g.add_method(name = "total_density",
              calling_pattern = "auto result = trace(density(self_c))",
              signature = "double()",
              doc = "Trace of density")

  g.add_method(name = "set_from_fourier",
              signature = "void(gf_view<imtime, matrix_valued, tail> gt)",
              calling_pattern = "self_c = fourier(*gt)",
              doc = """Fills self with the Fourier transform of gt""")

  g.add_method(name = "set_from_legendre",
              signature = "void(gf_view<legendre, matrix_valued, no_tail> gl)",
              calling_pattern = "self_c = legendre_to_imfreq(*gl)",
              doc = """Fills self with the legendre transform of gl""")

  g.add_method(name = "fit_tail",
              signature = "void(tail_view known_moments, int max_moment, int n_min, int n_max, bool replace_by_fit = true)",
              calling_pattern = "fit_tail(self_c, *known_moments, max_moment, n_min, n_max, replace_by_fit)",
              doc = """Set the tail by fitting""")

  g.add_method(name = "fit_tail",
              signature = "void(tail_view known_moments, int max_moment, int neg_n_min, int neg_n_max, int pos_n_min, int pos_n_max, bool replace_by_fit = true)",
              calling_pattern = "fit_tail(self_c, *known_moments, max_moment, neg_n_min, neg_n_max, pos_n_min, pos_n_max, replace_by_fit)",
              doc = """Set the tail by fitting""")

# Pure python methods
 g.add_pure_python_method("pytriqs.gf.local._gf_imfreq.replace_by_tail_depr")
 g.add_pure_python_method("pytriqs.gf.local._gf_imfreq.fit_tail_depr")

# For legacy Python code : authorize g + Matrix
 g.number_protocol['inplace_add'].add_overload(calling_pattern = "+=", signature = "void(gf_view<imfreq> x,matrix<std::complex<double>> y)")
 g.number_protocol['inplace_subtract'].add_overload(calling_pattern = "-=", signature = "void(gf_view<imfreq> x,matrix<std::complex<double>> y)")

 g.number_protocol['inplace_add'].add_overload(calling_pattern = "+=", signature = "void(gf_view<imfreq> x,std::complex<double> y)")
 g.number_protocol['inplace_subtract'].add_overload(calling_pattern = "-=", signature = "void(gf_view<imfreq> x,std::complex<double> y)")

 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<imfreq>(gf<imfreq> x,matrix<std::complex<double>> y)")
 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<imfreq>(matrix<std::complex<double>> y,gf<imfreq> x)")
 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<imfreq>(gf<imfreq> x,std::complex<double> y)")
 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<imfreq>(std::complex<double> y, gf<imfreq> x)")

 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<imfreq>(gf<imfreq> x,matrix<std::complex<double>> y)")
 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<imfreq>(matrix<std::complex<double>> y,gf<imfreq> x)")
 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<imfreq>(gf<imfreq> x,std::complex<double> y)")
 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<imfreq>(std::complex<double> y, gf<imfreq> x)")

 module.add_class(g)

module.add_function("bool is_gf_real_in_tau(gf_view<imfreq, matrix_valued, tail> g, double tolerance = 1.e-13)")

#############################
##   Tensor-valued Gfs [ImFreq, ImTime, ReFreq, Legendre]
##############################
for c_tag, py_tag in [("imfreq","ImFreq"), ("imtime","ImTime"), ("refreq", "ReFreq"), ("legendre","Legendre")]:
 for rank in [3, 4]:

  g = make_gf(
       py_type = "Gf%sTv%s"%(py_tag, rank),
       c_tag = c_tag,
       is_im = (c_tag in ("imfreq","imtime","legendre")),
       target_type = "tensor_valued<%s>"%rank,
       has_tail=False,
       serializable=True,
       )
  module.add_class(g)

#############################
##    Gfs [BrillouinZone]
##############################

g = make_gf(
       py_type = "GfBrillouinZone",
       c_tag = "brillouin_zone",
       is_im = True,
       target_type = "matrix_valued",
       has_tail=False,
       serializable=True,
       )
module.add_class(g)

########################
##   GfImTime
########################

g = make_gf(py_type = "GfImTime", c_tag = "imtime", is_im = True)

g.add_method(name = "set_from_inverse_fourier",
             signature = "void(gf_view<imfreq, matrix_valued, tail> gw)",
             calling_pattern = "self_c = inverse_fourier(*gw)",
             doc = """Fills self with the Inverse Fourier transform of gw""")

g.add_method(name = "set_from_legendre",
             signature = "void(gf_view<legendre, matrix_valued, no_tail> gl)",
             calling_pattern = "self_c = legendre_to_imtime(*gl)",
             doc = """Fills self with the legendre transform of gl""")

# add the call operator using the interpolation
g.add_call(signature = "matrix<dcomplex>(double tau)", doc = "G(tau) using interpolation")

module.add_class(g)

module.add_function(name = "rebinning_tau",
                    signature = "gf<imtime, matrix_valued>(gf_view<imtime,matrix_valued,tail> g, int new_n_tau)",
                    doc = "Rebins the data of a GfImTime on a sparser mesh")

########################
##   GfLegendre
########################

# the domain
dom = class_( py_type = "GfLegendreDomain",
        c_type = "legendre_domain",
        c_type_absolute = "triqs::gfs::legendre_domain",
        serializable= "tuple",
       )

dom.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")
module.add_class(dom)

g = make_gf(py_type = "GfLegendre", c_tag = "legendre", is_im = True, has_tail =False)

g.add_method(name = "density",
             calling_pattern = "auto result = density(self_c)",
             signature = "matrix_view<double>()",
             doc = "Density, as a matrix, computed from a Matsubara sum")

g.add_method(name = "total_density",
             calling_pattern = "auto result = trace(density(self_c))",
             signature = "double()",
             doc = "Trace of density")

g.add_method(name = "set_from_imtime",
             signature = "void(gf_view<imtime, matrix_valued, tail> gt)",
             calling_pattern = "self_c = imtime_to_legendre(*gt)",
             doc = """Fills self with the legendre transform of gt""")

g.add_method(name = "set_from_imfreq",
             signature = "void(gf_view<imfreq, matrix_valued, tail> gw)",
             calling_pattern = "self_c = imfreq_to_legendre(*gw)",
             doc = """Fills self with the legendre transform of gw""")

g.add_method(name = "enforce_discontinuity",
             signature = "void(matrix_view<double> disc)",
             calling_pattern = "enforce_discontinuity(self_c, disc)",
             doc = """Modify the coefficient to adjust discontinuity""")

module.add_class(g)

########################
##   GfReFreq
########################

for py_type, has_tail in [("GfReFreq", True) ,("GfReFreqNoTail",False)]:
 g = make_gf(py_type = py_type, c_tag = "refreq", has_tail=has_tail)
 if has_tail:
  g.add_method(name = "set_from_fourier",
              signature = "void(gf_view<retime, matrix_valued, tail> gt)",
              calling_pattern = "self_c = fourier(*gt)",
              doc = """Fills self with the Fourier transform of gt""")

  g.add_method(name = "set_from_pade",
              signature = "void(gf_view<imfreq, matrix_valued, tail> gw, int n_points = 100, double freq_offset = 0.0)",
              calling_pattern = "pade(self_c,*gw,n_points, freq_offset)",
              doc = """TO BE WRITTEN""")
  g.add_method(name = "density",
              signature = "matrix_view<double>()",
              calling_pattern = "auto result = density(self_c)",
              doc = "Density, as a matrix, computed from a Matsubara sum")

  g.add_method(name = "total_density",
              signature = "double()",
              calling_pattern = "auto result = trace(density(self_c))",
              doc = "Trace of density")

# For legacy Python code : authorize g + Matrix
 g.number_protocol['inplace_add'].add_overload(calling_pattern = "+=", signature = "void(gf_view<refreq> x,matrix<std::complex<double>> y)")
 g.number_protocol['inplace_subtract'].add_overload(calling_pattern = "-=", signature = "void(gf_view<refreq> x,matrix<std::complex<double>> y)")

 g.number_protocol['inplace_add'].add_overload(calling_pattern = "+=", signature = "void(gf_view<refreq> x,std::complex<double> y)")
 g.number_protocol['inplace_subtract'].add_overload(calling_pattern = "-=", signature = "void(gf_view<refreq> x,std::complex<double> y)")

 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<refreq>(gf<refreq> x,matrix<std::complex<double>> y)")
 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<refreq>(matrix<std::complex<double>> y,gf<refreq> x)")
 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<refreq>(gf<refreq> x,std::complex<double> y)")
 g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<refreq>(std::complex<double> y, gf<refreq> x)")

 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<refreq>(gf<refreq> x,matrix<std::complex<double>> y)")
 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<refreq>(matrix<std::complex<double>> y,gf<refreq> x)")
 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<refreq>(gf<refreq> x,std::complex<double> y)")
 g.number_protocol['subtract'].add_overload(calling_pattern = "-", signature = "gf<refreq>(std::complex<double> y, gf<refreq> x)")


 module.add_class(g)

########################
##   GfReTime
########################

g = make_gf(py_type = "GfReTime", c_tag = "retime")

g.add_method(name = "set_from_inverse_fourier",
             signature = "void(gf_view<refreq, matrix_valued, tail> gw)",
             calling_pattern = "self_c = inverse_fourier(*gw)",
             doc = """Fills self with the Inverse Fourier transform of gw""")

module.add_class(g)

# EXPERIMENTAL : global fourier functions....
# To be replaced by make_gf(fourier...)).

module.add_function(name = "make_gf_from_inverse_fourier", signature="gf_view<retime, matrix_valued, tail>(gf_view<refreq, matrix_valued, tail> gw)", doc ="")

########################
##   Code generation
########################

module.generate_code()

