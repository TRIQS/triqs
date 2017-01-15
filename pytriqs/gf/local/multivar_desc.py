from wrap_generator import *

# The module
module = module_(full_name = "pytriqs.gf.local.multivar", doc = "Local multivariable Green functions ...", app_name="triqs")

# All the triqs C++/Python modules
module.use_module('gf')
module.use_module('lattice_tools')

# Add here all includes beyond what is automatically included by the triqs modules
module.add_using("namespace triqs::gfs")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
#include <triqs/python_tools/converters/vector.hpp>
#include <triqs/python_tools/converters/tuple.hpp>
""")

#################################################
##   Wrap the C++ placeholder for partial eval
#################################################

c = class_( py_type = "VarT",
        c_type = "triqs::gfs::var_t",
        c_type_absolute = "triqs::gfs::var_t",
       )
c.add_constructor("()", doc = """ """)

module.add_class(c)

#########################
##   m_tail
########################

t = class_( py_type = "MeshBz",
        c_type = "gf_mesh<brillouin_zone>",
        c_type_absolute = "triqs::gfs::gf_mesh<triqs::lattice::brillouin_zone>",
        serializable= "tuple",
       )
#module.add_class(t) #duplicate of wrapped type in gf_desc!

t = class_( py_type = "MeshCyclic",
        c_type = "gf_mesh<cyclic_lattice>",
        c_type_absolute = "triqs::gfs::gf_mesh<triqs::gfs::cyclic_lattice>",
        serializable= "tuple",
       )
#module.add_class(t) #duplicate of wrapped type in gf_desc!

t = class_( py_type = "MTailBz",
        c_type = "gf_view<brillouin_zone, tail_valued<matrix_valued>>",
        c_type_absolute = "triqs::gfs::gf_view<triqs::lattice::brillouin_zone, triqs::gfs::tail_valued<triqs::gfs::matrix_valued>>",
        serializable= "tuple",
       )
module.add_class(t)

t = class_( py_type = "MTailCyclic",
        c_type = "gf_view<cyclic_lattice, tail_valued<matrix_valued>>",
        c_type_absolute = "triqs::gfs::gf_view<triqs::gfs::cyclic_lattice, triqs::gfs::tail_valued<triqs::gfs::matrix_valued>>",
        serializable= "tuple",
       )
module.add_class(t)

c_2py = dict((("imfreq","ImFreq"),("imtime","ImTime"), ("refreq", "ReFreq"),  ("legendre","Legendre"),("brillouin_zone","BrillouinZone"),("cyclic_lattice","CyclicLattice")))

var_tuple_list = (("imfreq", "imfreq"),
                 ("imtime", "imtime"), 
                 ("refreq", "refreq"),
                 ("imfreq", "imfreq", "imfreq"),  
                 ("imtime", "imtime", "imtime"),
                 ("legendre", "legendre"), 
                 ("imfreq", "legendre"),
                 ("imfreq", "legendre", "legendre"),
                 ("brillouin_zone","imfreq"), 
                 ("brillouin_zone","refreq"),
                 ("cyclic_lattice","imtime"))

for var_tuple in var_tuple_list : 

  ########   The mesh   ###########
  mesh_rank = len(var_tuple)

  mesh_py_type =  "Mesh%s"%("".join(c_2py[x] for x in var_tuple))
  #print "Generating wrapper for %s"%mesh_py_type

  # The class gf_mesh<cartesian_product<var_tuple...[1]>>
  c = class_(
          py_type =mesh_py_type,
          c_type = "gf_mesh<cartesian_product<%s>>"%(",".join(x for x in var_tuple)),
          c_type_absolute = "triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<%s>>"%(",".join("triqs::gfs::"+x for x in var_tuple)),
          hdf5 = True,
          serializable = "tuple",
          doc = "Cartesian product of meshes " + ",".join([c_2py[x]  for x in var_tuple])
  )
  c.add_constructor("("+",".join(["""gf_mesh<%s> m%s"""%(var_tuple[i],i) for i in range(len(var_tuple))])+")",
                    doc = """ """)

  c.add_property(name = "size",
                 getter = cfunction("size_t size ()"),
                 doc = """size """)

  c.add_len()

  c.add_property(name = "components",
                 getter = cfunction("std::tuple<"+",".join(["gf_mesh<%s>"%x for x in var_tuple])+"> components ()"),
                 doc = """components """)
  #c.add_method_copy()
  #c.add_method_copy_from()

  module.add_class(c)

  ########  The Gf ###########

  cart_prod = 'triqs::gfs::cartesian_product<%s>'%(",".join("triqs::gfs::"+y for y in var_tuple))
  gf_mesh_t = 'gf_mesh<%s>'%cart_prod
  first_var_is_k_or_x = var_tuple[0] in ["brillouin_zone", "cyclic_lattice"]
  
  target_as_type = { 0 : 'scalar_valued', 2 : 'matrix_valued', 3 : 'tensor_valued<3>', 4 : 'tensor_valued<4>'}
  target_as_str = { 0 : '_s', 2 : '', 3 : 'Tv3', 4 : 'Tv4'}
  
  if first_var_is_k_or_x : 
    target_ranks = [2]
  elif mesh_rank < 3: 
    target_ranks = [0, 3, 4] 
  else :
    target_ranks = [3, 4] 

  for rank in target_ranks : 
    target_type, target_string = target_as_type[rank], target_as_str[rank]
    gf_py_type = "Gf%s%s"%("_x_".join([c_2py[x] for x in var_tuple]), target_string)
    print "Generating wrapper for %s"%gf_py_type
    c = class_(
            py_type = gf_py_type,
            c_type = "gf_view<%s, %s>"%(cart_prod, target_type),
            c_type_absolute = "triqs::gfs::gf_view<%s, triqs::gfs::%s>"%(cart_prod, target_type),
            hdf5 = True,
            serializable = "tuple",
            doc = "GF on a product of meshes " + ",".join([c_2py[x] for x in var_tuple]),
            arithmetic = ("algebra", "std::complex<double>")
    )

    c.add_constructor("""(%s mesh, mini_vector<int, %s> shape)"""%(gf_mesh_t,rank) , doc = """ """)

    c.add_member(c_name = "name", c_type = "std::string", doc = "Name of the Green function (used for plotting only)")

    c.add_property(name = "mesh", getter = cfunction("%s mesh ()"%gf_mesh_t, doc = """mesh"""))

    c.add_property(name = "data",
                   getter = cfunction("array_view<std::complex<double>, %s> data ()"%(rank+ mesh_rank)),
                   doc = """data""")

    # Slices
    all_cases = ''
    for i in range(len(var_tuple)):
     return_descriptor = ", ".join(var_tuple[j] for j in range(len(var_tuple)) if j!=i)
     ind_type = "triqs::utility::mini_vector<long, 3>" if var_tuple[i] in ["brillouin_zone", "cyclic_lattice"] else "int" 
     
     # OLD SYNTAX : TO BE REMOVED 
     rhs = "self_c" + ''.join("[%s]"%('x' if n==i else '_') for n in range(len(var_tuple))) # build the [_][n][_]
     code = "auto _= triqs::gfs::var_t{};\n auto result = " + ("reinterpret_scalar_valued_gf_as_matrix_valued(%s)"%rhs if target_type=="scalar_valued" else rhs)
     c.add_method(
       name = "slice_at_const_w%s"%(i+1),
       calling_pattern = code, 
       signature = "gf_view<%s, %s> (%s x)"%(return_descriptor, "matrix_valued" if target_type=="scalar_valued" else target_type, ind_type),
       doc="""slice at const %sth argument"""%(i+1)
     )
     
     # accumulate the cases
     arg_list = ', '.join("%s"%(ind_type if n==i else 'triqs::gfs::var_t') for n in range(len(var_tuple))) 
     all_cases += """{
                      using Tu = std::tuple<%s>;
                      if (convertible_from_python<Tu>(X, false)) return convert_to_python( triqs::tuple::apply(f, convert_from_python<Tu>(X)));
                     }
                   """%arg_list

    # one argument only : expect a tuple. Try all possible conversion of the tuple or fail
    if mesh_rank ==2: # To be generalized
        c.add_getitem(signature = "gf_view<%s>(PyObject * X) "%(cart_prod), # FIXME : IF *X Does not work !
                  calling_pattern= """ 
                   auto f= [&self_c](auto &&... y) { return triqs::gfs::details::partial_eval(&self_c, y...);};
                   auto dispatch = [&f,X]() {
                     %s
                   TRIQS_RUNTIME_ERROR << "[(...)] argument is not understood"; 
                   }; 
                   auto result = dispatch();
                   """%all_cases,
                  doc = "Returns a sliced view of the Green function")

    if rank>0 :

     c.add_getitem(signature = "gf_view<%s>(%s) "%(cart_prod, ", ".join("PyObject * r%s"%i for i in range(rank))),
                  calling_pattern= """ 
                   auto sh = self_c.target_shape();
                   auto f = [&sh, &self_c](PyObject * r, int u) {
                       if (convertible_from_python<std::string>(r, false))
                         return self_c.indices().convert_index(convert_from_python<std::string>(r),u);
                       return range_from_slice(r,sh[u]);
                       };
                   auto result = slice_target(self_c, %s); """%(', '.join("f(r%s,%s)"%(i,i) for i in range(rank))) ,
                  doc = "Returns a sliced view of the Green function")

     c.add_setitem(signature = "void("+", ".join("PyObject* r%s"%i for i in range(rank))+", PyObject* val)",
                  calling_pattern=
                  """
                   pyref __arg = Py_BuildValue("(OO)", %s);
                   pyref gs_py = PyObject_GetItem(self, __arg);  // gs = self[r1,r2]
                   pyref res = PyNumber_Lshift(gs_py, val);      // gs << val
                  """ %(', '.join('r%s'%i for i in range(rank))),
                  no_self_c = True, # avoid a warning
                  doc = "g[....] << what_ever : fills the slice of the Green function with what_ever")

    c.number_protocol['lshift'] = pyfunction(name ="__lshift__", python_precall = "pytriqs.gf.local._gf_common._lshift_", arity = 2)
    c.add_method_copy()
    c.add_method_copy_from()
    if first_var_is_k_or_x:
     c.add_pure_python_method("pytriqs.gf.local._gf_space_x_X.plot", rename = "_plot_")
    elif mesh_rank==2:
     c.add_pure_python_method("pytriqs.gf.local._gf_X_x_X.plot", rename = "_plot_")

    module.add_class(c)

module.generate_code()
