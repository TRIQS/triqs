from wrap_generator import *

module = module_(full_name = "pytriqs.gf.local.gf", doc = "Local Green functions ...")
module.add_include("<triqs/gfs.hpp>")
module.add_include("<triqs/gfs/local/functions.hpp>")
module.add_include("<triqs/gfs/local/fit_tail.hpp>")
module.add_include("<triqs/gfs/local/pade.hpp>")
module.add_include("<triqs/gfs/local/legendre_matsubara.hpp>")
module.add_using("namespace triqs::arrays")
module.add_using("namespace triqs::gfs")
module.add_using("namespace triqs::gfs::local")
module.add_using("triqs::utility::mini_vector")

########################
##   TailGf
########################

t = class_( py_type = "TailGf",
        c_type = "local::tail_view",
        c_type_absolute = "triqs::gfs::local::tail_view",
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
               getter = cfunction("array_view<long,2> mask_view()"),
               doc = "Access to the mask")

t.add_method(name = "has_coef",
             calling_pattern = "bool result = (i >=self_c.order_min()) && (i<=self_c.order_max())",
             signature = "bool(int i)",
             doc = "A method which did not exist in C++")

# strange, I should not quality : ADL ??
t.add_method(name = "invert",
             calling_pattern = "self_c = local::inverse(self_c)",
             signature = "void()",
             doc = "Invert")

t.add_method(name = "zero",
             calling_pattern = "self_c = 0",
             signature = "void()",
             doc = "Sets the expansion to 0")

t.add_method_copy()
t.add_method_copy_from()

t.add_call(calling_pattern = "auto result = self_c.evaluate(u)",
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
##   enums
########################

module.add_enum(c_name = "statistic_enum",
                c_namespace = "triqs::gfs",
                values = ["Fermion","Boson"])

module.add_enum(c_name = "mesh_kind",
                c_namespace = "triqs::gfs",
                values = ["half_bins","full_bins","without_last"])

########################
##   Mesh generic
########################

def make_mesh( py_type, c_tag, has_kind=True, is_im=False) :
    m = class_( py_type = py_type,
            c_type = "gf_mesh<%s>"%c_tag,
            c_type_absolute = "triqs::gfs::gf_mesh<triqs::gfs::%s>"%c_tag,
            serializable= "tuple",
            is_printable= True,
           )

    if is_im :
        m.add_property(name = "beta",
                       getter = cfunction(calling_pattern="double result = self_c.domain().beta",
                       signature = "double()",
                       doc = "Inverse temperature"))

        m.add_property(name = "statistic",
                       getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
                       doc = "Statistic")

    m.add_len(calling_pattern = "int result = self_c.size()", doc = "Size of the mesh")
    m.add_iterator(c_cast_type = "dcomplex")

    if has_kind :
        m.add_property(name = "kind",
               getter = cfunction(calling_pattern="mesh_kind result = self_c.kind()", signature = "mesh_kind()"),
               doc = "")

    #def __richcmp__(MeshImFreq self, MeshImFreq other,int op) :
    #    if op ==2 : # ==
    #        return self._c == other._c

    return m

########################
##   MeshImFreq
########################

m = make_mesh( py_type = "MeshImFreq", c_tag = "imfreq", has_kind = False, is_im = True)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max=1025, bool positive_only=true)")

module.add_class(m)

########################
##   MeshImTime
########################

m = make_mesh(py_type = "MeshImTime", c_tag = "imtime", is_im = True)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max, mesh_kind kind)")

module.add_class(m)

########################
##   MeshLegendre
########################

m = make_mesh( py_type = "MeshLegendre", c_tag = "legendre", has_kind = False, is_im = True)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max=1025)")

module.add_class(m)

########################
##   MeshReFreq
########################

m = make_mesh(py_type = "MeshReFreq", c_tag = "refreq")
m.add_constructor(signature = "(double omega_min, double omega_max, int n_max, mesh_kind kind)")

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
m.add_constructor(signature = "(double t_min, double t_max, int n_max, mesh_kind kind)")

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
##   Gf Generic : common to all 5 one variable gf
########################

def make_gf( py_type, c_tag, is_complex_data = True, is_im = False, has_tail = True) :

    data_type = "std::complex<double>" if is_complex_data else "double"

    g = class_(
            py_type = py_type,
            c_type = "gf_view<%s>"%c_tag,
            c_type_absolute = "triqs::gfs::gf_view<triqs::gfs::%s>"%c_tag,
            #serializable= "boost",
            serializable= "tuple",
            is_printable= True,
            hdf5 = True,
            arithmetic = ("algebra",data_type, "with_inplace_operators")
            )

    g.add_constructor(signature = "(gf_mesh<%s> mesh, mini_vector<size_t,2> shape, std::vector<std::vector<std::string>> indices = std::vector<std::vector<std::string>>{}, std::string name = "")"%c_tag, python_precall = "pytriqs.gf.local._gf_%s.init"%c_tag)

    g.add_method_copy()
    g.add_method_copy_from()

    # properties
    g.add_member(c_name = "name", c_type  ="std::string",  doc = "Name of the Green function (used for plotting only)")

    if is_im :
        g.add_property(name = "beta",
                       getter = cfunction(calling_pattern="double result = self_c.domain().beta", signature = "double()"),
                       doc = "Inverse temperature")

        g.add_property(name = "statistic",
                       getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
                       doc = "Statistic")

    g.add_property(getter = cfunction("gf_mesh<%s> mesh()"%c_tag),
                   doc ="The mesh")

    g.add_property(name = "data",
                   getter = cfunction(calling_pattern="auto result = self_c.data()", signature = "array_view<%s,3>()"%data_type),
                   doc ="The data ")

    g.add_property(name = "target_shape",
                   getter = cfunction(calling_pattern="auto result = get_target_shape(self_c)", signature = "shape_type()"),
                   doc = "")

    if has_tail:
        g.add_property(name = "tail",
                       getter = cfunction(c_name="singularity", signature = "local::tail_view()"),
                       doc ="The high frequency tail")

    g.add_property(name = "indices",
                   getter = cfunction(calling_pattern="auto result = self_c.indices()[0]", signature = "std::vector<std::string>()"),
                   doc ="The indices(L)")

    g.add_property(name = "indicesL",
                   getter = cfunction(calling_pattern="auto result = self_c.indices()[0]", signature = "std::vector<std::string>()"),
                   doc ="The indices(L)")

    g.add_property(name = "indicesR",
                   getter = cfunction(calling_pattern="auto result = self_c.indices()[1]", signature = "std::vector<std::string>()"),
                   doc ="The indices(R)")

    # backward compatibility
    g.add_property(name = "N1",
                   getter = cfunction(calling_pattern="int result = get_target_shape(self_c)[0]", signature = "int()"),
                   doc = "[Deprecated] equivalent to target_shape[0]")
    g.add_property(name = "N2",
                   getter = cfunction(calling_pattern="int result = get_target_shape(self_c)[1]", signature = "int()"),
                   doc = "[Deprecated] equivalent to target_shape[1]")

    # []
    g.add_getitem(signature = "gf_view<%s>(range r1, range r2)"%c_tag,
                  calling_pattern= "auto result = slice_target(self_c,r1,r2)",
                  doc = "Returns a sliced view of the Green function")

    g.add_getitem(signature = "gf_view<%s>(std::string i1, std::string i2)"%c_tag,
                  calling_pattern= "auto result = slice_target(self_c,self_c.indices().convert_index(i1,0),self_c.indices().convert_index(i2,1))",
                  doc = "Returns a sliced view of the Green function")

    g.add_setitem(signature = "void(PyObject* r1, PyObject* r2, PyObject* val)",
                  calling_pattern=
                  """
                   pyref gs_py = PyObject_GetItem(self,Py_BuildValue("(NN)", r1,r2));  // gs = self[r1,r2]
                   pyref res = PyNumber_InPlaceLshift(gs_py,val);                      // gs <<= val
                  """,
                  no_self_c = True, # avoid a warning
                  doc = "g[....] <<= what_ever : fills the slice of the Green function with what_ever")

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

    g.number_protocol['inplace_lshift'] = pyfunction(name ="__inplace_lshift__", python_precall = "pytriqs.gf.local._gf_common._ilshift_", arity = 2)

    g.add_method(name = "invert", calling_pattern = "invert_in_place(self_c)" , signature = "void()", doc = "Invert (in place)")

    g.add_method(name = "transpose",
                 calling_pattern = "auto result = transpose(self_c)",
                 signature = "gf<%s>()"%c_tag,
                 doc = "Returns a NEW gf, with transposed data, i.e. it is NOT a transposed view.")

    if c_tag not in [ "imtime", "legendre"] :
        g.add_method(name = "conjugate", calling_pattern = "auto result = conj(self_c)" , signature = "gf<%s>()"%c_tag, doc = "Return a new function, conjugate of self.")

    g.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "gf<%s>(matrix<%s> x,gf<%s> y)"%(c_tag,data_type,c_tag)) 
    g.number_protocol['multiply'].add_overload(calling_pattern = "*", signature = "gf<%s>(gf<%s> x,matrix<%s> y)"%(c_tag,c_tag,data_type)) 

    g.add_method(name = "from_L_G_R",
                 calling_pattern = "self_c = L_G_R(l,g,r)",
                 signature = "void(matrix<%s> l,gf<%s> g,matrix<%s> r)"%(data_type,c_tag,data_type),
                 doc = "self <<= l * g * r")

    g.add_method(name = "zero",
                 calling_pattern = "self_c = 0",
                 signature = "void()",
                 doc = "Put the Green function to 0")

    # Pure python methods
    g.add_pure_python_method("pytriqs.gf.local._gf_%s.plot"%c_tag, rename = "_plot_")

    return g

########################
##   GfImFreq
########################

g = make_gf(py_type = "GfImFreq", c_tag = "imfreq", is_im = True)

g.add_method(name = "density",
             calling_pattern = "auto result = density(self_c)",
             signature = "matrix_view<double>()",
             doc = "Density, as a matrix, computed from a Matsubara sum")

g.add_method(name = "total_density",
             calling_pattern = "auto result = trace(density(self_c))",
             signature = "double()",
             doc = "Trace of density")

g.add_method(name = "set_from_fourier",
             signature = "void(gf_view<imtime> gt)",
             calling_pattern = "self_c = fourier(*gt)",
             doc = """Fills self with the Fourier transform of gt""")

g.add_method(name = "set_from_legendre",
             signature = "void(gf_view<legendre> gl)",
             calling_pattern = "self_c = legendre_to_imfreq(*gl)",
             doc = """Fills self with the legendre transform of gl""")

g.add_method(name = "fit_tail",
             signature = "void(tail_view known_moments, int n_moments, int n_min, int n_max, bool replace_by_fit = true)",
             calling_pattern = "fit_tail(self_c, known_moments, n_moments, n_min, n_max, replace_by_fit)",
             doc = """Set the tail by fitting""")
 
# Pure python methods
g.add_pure_python_method("pytriqs.gf.local._gf_imfreq.replace_by_tail_depr")
g.add_pure_python_method("pytriqs.gf.local._gf_imfreq.fit_tail_depr")

# For legacy Python code : authorize g + Matrix
#g.number_protocol['add'].add_overload(calling_pattern = "+", signature = "gf<imfreq>(gf<imfreq> x,matrix<std:complex<double>> y)")
g.number_protocol['inplace_add'].add_overload(calling_pattern = "+=", signature = "void(gf_view<imfreq> x,matrix<std::complex<double>> y)")
g.number_protocol['inplace_subtract'].add_overload(calling_pattern = "-=", signature = "void(gf_view<imfreq> x,matrix<std::complex<double>> y)")

module.add_class(g)

########################
##   GfImTime
########################

g = make_gf(py_type = "GfImTime", c_tag = "imtime", is_complex_data = False, is_im = True)

g.add_method(name = "set_from_inverse_fourier",
             signature = "void(gf_view<imfreq> gw)",
             calling_pattern = "self_c = inverse_fourier(*gw)",
             doc = """Fills self with the Inverse Fourier transform of gw""")

g.add_method(name = "set_from_legendre",
             signature = "void(gf_view<legendre> gl)",
             calling_pattern = "self_c = legendre_to_imtime(*gl)",
             doc = """Fills self with the legendre transform of gl""")
 
# add the call operator using the interpolation
g.add_call(signature = "matrix<double>(double tau)", doc = "G(tau) using interpolation")

module.add_class(g)

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

g = make_gf(py_type = "GfLegendre", c_tag = "legendre", is_im = True, is_complex_data = False, has_tail =False)

g.add_method(name = "density",
             calling_pattern = "auto result = density(self_c)",
             signature = "matrix_view<double>()",
             doc = "Density, as a matrix, computed from a Matsubara sum")

g.add_method(name = "total_density",
             calling_pattern = "auto result = trace(density(self_c))",
             signature = "double()",
             doc = "Trace of density")

g.add_method(name = "set_from_imtime",
             signature = "void(gf_view<imtime> gt)",
             calling_pattern = "self_c = imtime_to_legendre(*gt)",
             doc = """Fills self with the legendre transform of gt""")

g.add_method(name = "set_from_imfreq",
             signature = "void(gf_view<imfreq> gw)",
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

g = make_gf(py_type = "GfReFreq", c_tag = "refreq")

g.add_method(name = "set_from_fourier",
             signature = "void(gf_view<retime> gt)",
             calling_pattern = "self_c = fourier(*gt)",
             doc = """Fills self with the Fourier transform of gt""")

g.add_method(name = "set_from_pade",
             signature = "void(gf_view<imfreq> gw, int n_points = 100, double freq_offset = 0.0)",
             calling_pattern = "pade(self_c,*gw,n_points, freq_offset)",
             doc = """TO BE WRITTEN""")

module.add_class(g)

########################
##   GfReTime
########################

g = make_gf(py_type = "GfReTime", c_tag = "retime")

g.add_method(name = "set_from_inverse_fourier",
             signature = "void(gf_view<refreq> gw)",
             calling_pattern = "self_c = inverse_fourier(*gw)",
             doc = """Fills self with the Inverse Fourier transform of gw""")

module.add_class(g)

# EXPERIMENTAL : global fourier functions....
# To be replaced by make_gf(fourier...)).

module.add_function(name = "make_gf_from_inverse_fourier", signature="gf_view<retime>(gf_view<refreq> gw)", doc ="")

########################
##   Code generation
########################

module.generate_code()

