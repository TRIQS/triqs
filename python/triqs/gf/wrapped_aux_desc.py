from cpp2py.wrap_generator import *

# This modules contains utilities that will only be called internally, not by users.
m = module_(full_name = "triqs.gf.wrapped_aux", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")

m.add_imports("triqs.gf.meshes", "triqs.gf.gf_fnt")

m.add_include("<triqs/gfs.hpp>")
m.add_include("<triqs/mesh.hpp>")
m.add_include("<triqs/gfs/transform/pade.hpp>")

m.add_include("<cpp2py/converters/vector.hpp>")
m.add_include("<cpp2py/converters/std_array.hpp>")
m.add_include("<triqs/cpp2py_converters.hpp>")

m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")
m.add_using("namespace triqs::arrays")
m.add_using("namespace triqs::gfs")
m.add_preamble("""
""")

#------------------------------------------------------------
# Call operators
#------------------------------------------------------------
def all_calls():
    validargs = {
        'imfreq' : ['long', 'matsubara_freq'],
        'imtime' : ['long', 'double'],
        'refreq' : ['long', 'double'],
        'retime' : ['long', 'double'],
        'dlr'    : ['long', 'double', 'matsubara_freq'],
        'brzone' : ['std::array<long,3>', 'std::array<double, 3>'],
        'cyclat' : ['std::array<long,3>', 'triqs::lattice::bravais_lattice::point_t']
        }
    target_and_return = [
        # rank, target, return_t
        [ 0, 'scalar_valued', 'dcomplex'],
        [ 1, 'tensor_valued<1>', 'array<dcomplex, 1>'],
        [ 2, 'matrix_valued', 'matrix<dcomplex>'],
        [ 3, 'tensor_valued<3>', 'array<dcomplex, 3>'],
        [ 4, 'tensor_valued<4>', 'array<dcomplex, 4>']
        ]

    meshes = ['imfreq', 'imtime', 'refreq', 'retime', 'dlr', 'brzone', 'cyclat']

    real_valued = lambda target: target.replace("_", "_real_")
    for rank, target, return_t in target_and_return:
        for M1 in meshes:
            xs = validargs[M1]
            nxs = len(xs)
            yield M1, [return_t]*nxs, rank, target, xs
            if M1 == 'imtime': yield M1, [return_t]*nxs, rank, real_valued(target), xs

            # Product Mesh Wrappings
            for M2 in meshes:
                if sum([M1 in ['brzone', 'cyclat'], M2 in ['brzone', 'cyclat']]) == 1:
                    ys = validargs[M2]
                    nys = len(ys)
                    arg_lst = [(x, y) for x in xs for y in ys] + [('all_t', y) for y in ys] + [(x, 'all_t') for x in xs]
                    ret_lst = [return_t]*nxs*nys + [f"gf<{M1}, {target}>"]*nys + [f"gf<{M2}, {target}>"]*nxs
                    yield 'prod<%s,%s>'%(M1,M2), ret_lst, rank, target, arg_lst
                    if 'imtime' in [M1, M2]:
                        ret_lst = [return_t]*nxs*nys + [f"gf<{M1}, {real_valued(target)}>"]*nys + [f"gf<{M2}, {real_valued(target)}>"]*nxs
                        yield 'prod<%s,%s>'%(M1,M2), ret_lst, rank, real_valued(target), arg_lst


# Fixme
C_py_transcript = {'imfreq' : 'ImFreq',
                   'refreq' : 'ReFreq',
                   'imtime' : 'ImTime',
                   'retime' : 'ReTime',
                   'dlr'    : 'DLR',
                   'brzone' : 'BrZone',
                   'cyclat' : 'CycLat',
                   'prod<brzone,imfreq>': 'BrZone_x_ImFreq',
                   'prod<brzone,imtime>': 'BrZone_x_ImTime',
                   'prod<brzone,refreq>': 'BrZone_x_ReFreq',
                   'prod<brzone,retime>': 'BrZone_x_ReTime',
                   'prod<brzone,dlr>'   : 'BrZone_x_DLR',
                   'prod<cyclat,imfreq>': 'CycLat_x_ImFreq',
                   'prod<cyclat,imtime>': 'CycLat_x_ImTime',
                   'prod<cyclat,refreq>': 'CycLat_x_ReFreq',
                   'prod<cyclat,retime>': 'CycLat_x_ReTime',
                   'prod<cyclat,dlr>'   : 'CycLat_x_DLR',
                   'prod<imfreq,brzone>': 'ImFreq_x_BrZone',
                   'prod<imtime,brzone>': 'ImTime_x_BrZone',
                   'prod<refreq,brzone>': 'ReFreq_x_BrZone',
                   'prod<retime,brzone>': 'ReTime_x_BrZone',
                   'prod<dlr,brzone>'   : 'DLR_x_BrZone',
                   'prod<imfreq,cyclat>': 'ImFreq_x_CycLat',
                   'prod<imtime,cyclat>': 'ImTime_x_CycLat',
                   'prod<refreq,cyclat>': 'ReFreq_x_CycLat',
                   'prod<retime,cyclat>': 'ReTime_x_CycLat',
                   'prod<dlr,cyclat>'   : 'DLR_x_CycLat',
                   }

m.add_preamble("""
namespace triqs {
 namespace gfs {
  template<typename Gv>
  struct gf_proxy {
   Gv gv;
   gf_proxy(Gv gv) : gv(gv){}
   template<typename ReturnType, typename ... U> ReturnType call(U&& ... x) { return ReturnType{gv(std::forward<U>(x)...)};}
  };
 }
}
""")

for mesh, return_t, R, target_t, point_t in all_calls():
    c_type = "gf_proxy<gf_view<%s,%s>>"%(mesh, target_t)
    c_py_trans = C_py_transcript[mesh]
    RE = "_R" if "_real_" in target_t else ""
    # print " Proxy : ", c_type, " : Py : ", "CallProxy%s_%s"%(c_py_trans,R,RE)
    c = class_(
            py_type = "CallProxy%s_%s%s"%(c_py_trans,R,RE),
            c_type = c_type,
            c_type_absolute = "triqs::gfs::" + c_type,
            export = False
            )
    c.add_constructor("(gf_view<%s,%s> g)"%(mesh, target_t), doc = "")
    for P, Ret in zip(point_t, return_t) :
        if not isinstance(P, tuple) :
          c.add_call(signature = "%s call(%s x)"%(Ret, P), calling_pattern = "auto result = self_c.template call<%s>(x)"%(Ret), doc = "")
        else:
            xs = ['%s x_%s'%(t,n) for (n,t) in enumerate(P)]
            xs2 = ['x_%s'%(n) for (n,t) in enumerate(P)]
            sig =  "%s call<%s>(%s)"%(Ret, Ret, ','.join(xs))
            c.add_call(signature =sig,  calling_pattern = "auto result = self_c.template call<%s>(%s)"%(Ret,  ','.join(xs2)), doc = "")
    m.add_class (c)

# #------------------------------------------------------------

# matrix valued target
for TY in ['double', 'dcomplex'] :
    for R in ['3'] : # To be generalized in C++
        m.add_function("void set_from_gf_data_mul_LR (array_view<{TY}, {R}> a, matrix<{TY}> l, array_view<{TY}, {R}> b,  matrix<{TY}> r)".format(TY=TY, R=R),
                        calling_pattern = "set_from_gf_data_mul_LR(a,l,b,r)")

# invert auxiliary tool
m.add_function("void _gf_invert_data_in_place(array_view <dcomplex, 3> a)", doc = "Aux function for inversion")

# For legacy Python code : authorize g + Matrix
for M in ['imfreq', 'refreq'] :
    m.add_function("void _iadd_g_matrix_scalar (gf_view<%s, matrix_valued> x, matrix<std::complex<double>> y)"%M, calling_pattern = "x += y")
    m.add_function("void _iadd_g_matrix_scalar (gf_view<%s, matrix_valued> x, std::complex<double> y)"%M, calling_pattern = "x += y")

    m.add_function("void _isub_g_matrix_scalar (gf_view<%s, matrix_valued> x, matrix<std::complex<double>> y)"%M, calling_pattern = "x -= y")
    m.add_function("void _isub_g_matrix_scalar (gf_view<%s, matrix_valued> x, std::complex<double> y)"%M, calling_pattern = "x -= y")
    # is it useful ?
    #m.add_function("gf<imfreq> _imul_R_g_matrix (gf_view<{M}, matrix_valued> x, matrix<std::complex<double>> y)", calling_pattern = "x = x * y")
    #m.add_function("gf<imfreq> _imul_L_g_matrix (matrix<std::complex<double>> y, gf_view<{M}, matrix_valued> x)", calling_pattern = "x = y * x")

# invert auxiliary tool
m.add_function("gf<imfreq, matrix_valued> _make_gf_from_real_gf(gf_view<imfreq, matrix_valued> g)",
        calling_pattern = " auto result = make_gf_from_real_gf(make_const_view(g));", doc = "Backward Compat. Internal")

########################
##   Code generation
########################

m.generate_code()
