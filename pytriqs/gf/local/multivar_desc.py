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
#include <triqs/gfs/singularity/tail_zero.hpp>
""")

for TimeMesh in [[("ImFreq","imfreq"), ("ImFreq","imfreq")],[("ImTime","imtime"), ("ImTime","imtime")], [("ReFreq", "refreq"), ("ReFreq", "refreq")], [("ImFreq","imfreq"), ("ImFreq","imfreq"), ("ImFreq","imfreq")],  [("ImTime","imtime"), ("ImTime","imtime"), ("ImTime","imtime")], [("BrillouinZone","brillouin_zone"),("ImFreq","imfreq")], [("BrillouinZone","brillouin_zone"),("ReFreq","refreq")] ]:

  # The class gf_mesh<cartesian_product<imfreq, imfreq>>
  c = class_(
          py_type = "Mesh%s"%("".join([x for x,y in TimeMesh])),
          c_type = "gf_mesh<cartesian_product<%s>>"%(",".join([y for x,y in TimeMesh])), 
          c_type_absolute = "triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<%s>>"%(",".join(["triqs::gfs::"+y for x,y in TimeMesh])),
          hdf5 = True,
          serializable="tuple",
          doc="Cartesian product of meshes",
  )
  c.add_constructor("""("""+""",""".join(["""gf_mesh<%s> m%s"""%(TimeMesh[i][1],i) for i in range(len(TimeMesh))])+""")""",
                    doc = """ """)

  c.add_property(name = "size",
                 getter = cfunction("size_t size ()"),
                 doc = """size """)

  c.add_property(name = "components",
                 getter = cfunction("std::tuple<"+",".join(["gf_mesh<%s>"%y for x,y in TimeMesh])+"> components ()"),
                 doc = """components """)
  #c.add_method_copy()
  #c.add_method_copy_from()

  module.add_class(c)


  ###################
  if TimeMesh[0][0]=="BrillouinZone":
   Target_List = [('matrix_valued', '')]
  else:
   Target_List = [('scalar_valued', '_s'),('tensor_valued<3>', 'Tv3'), ('tensor_valued<4>', 'Tv4')] if len(TimeMesh)<3 else [('tensor_valued<3>', 'Tv3'), ('tensor_valued<4>', 'Tv4') ]
  for target_type, target_string in Target_List:
    if target_type == "scalar_valued":
     rank = 0 
    elif target_type == "matrix_valued":
     rank = 2  
    else:
     rank = int(re.compile("tensor_valued<(.*)>").match(target_type).groups()[0])
    c = class_(
            py_type = "Gf%s%s"%("_x_".join([x for x,y in TimeMesh]), target_string), 
            c_type = "gf_view<cartesian_product<%s>, %s>"%(",".join([y for x,y in TimeMesh]), target_type),
            c_type_absolute = "triqs::gfs::gf_view<triqs::gfs::cartesian_product<%s>, triqs::gfs::%s>"%(",".join(["triqs::gfs::"+y for x,y in TimeMesh]), target_type),
            hdf5 = True,
            serializable="tuple",
            doc="2-freq gf",
            arithmetic = ("algebra", "std::complex<double>"),
    )
    shape_or_not= """""" if target_type=="scalar_valued" else """, mini_vector<size_t, %s> shape"""%(rank)

    c.add_constructor("""(gf_mesh<cartesian_product<%s>> mesh"""%(",".join([y for x,y in TimeMesh]))+shape_or_not+""")""",
                      doc = """ """)

    c.add_member(c_name = "name", c_type  ="std::string",  doc = "Name of the Green function (used for plotting only)")

    c.add_property(name = "mesh",
                   getter = cfunction("gf_mesh<cartesian_product<%s>> mesh ()"%(",".join([y for x,y in TimeMesh]))),
                   doc = """mesh """)
    if target_type=="scalar_valued":
     tail_data_t = "std::complex<double>"
    elif target_type=="matrix_valued":
     tail_data_t = "triqs::arrays::matrix<std::complex<double>>"
    else:
     tail_data_t = "triqs::arrays::array<std::complex<double>,%s>"%rank
    c.add_property(name = "tail",
      getter = cfunction("tail_zero<%s> singularity ()"%(tail_data_t)),
                   doc = """singularity """)

    c.add_property(name = "data",
                   getter = cfunction("array_view<std::complex<double>, %s> data ()"%(rank+len(TimeMesh))),
                   doc = """data """)
    for i in range(len(TimeMesh)):
     return_descriptor = ",".join([TimeMesh[j][1] for j in range(i) if not j==i])
     ind_type = "int" if not TimeMesh[i][0]=="BrillouinZone" else "triqs::utility::mini_vector<long, 3>"
     c.add_method(
       name = "slice_at_const_w%s"%(i+1),
       calling_pattern = "auto result = make_gf_from_g_and_tail(reinterpret_scalar_valued_gf_as_matrix_valued(partial_eval<%s>(self_c, n)), tail(1,1))"%i if target_type=="scalar_valued" else  "auto result =  partial_eval<%s>(self_c, n)"%i,
       signature = "gf_view<%s, matrix_valued> (%s n)"%(return_descriptor, ind_type) if target_type=="scalar_valued" else "gf_view<%s, %s> (%s n)"%(return_descriptor, target_type, ind_type),
                 doc="""slice at const %sth argument"""%(i+1))

    if rank>0 : 
     c.add_getitem(signature = "gf_view<cartesian_product<%s>>("%(",".join([y for x,y in TimeMesh]))+", ".join(["PyObject * r%s"%i for i in range(1,rank+1)])+")",
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

     c.add_setitem(signature = "void("+", ".join(["PyObject* r%s"%i for i in range(1,rank+1)])+", PyObject* val)",
                  calling_pattern=
                  """
                   pyref __arg = Py_BuildValue("(OO)", """+""", """.join(["""r%s"""%i for i in range(1,rank+1)])+""");
                   pyref gs_py = PyObject_GetItem(self, __arg);  // gs = self[r1,r2]
                   pyref res = PyNumber_Lshift(gs_py, val);      // gs << val
                  """,
                  no_self_c = True, # avoid a warning
                  doc = "g[....] << what_ever : fills the slice of the Green function with what_ever")

    c.number_protocol['lshift'] = pyfunction(name ="__lshift__", python_precall = "pytriqs.gf.local._gf_common._lshift_", arity = 2)
    c.add_method_copy()
    c.add_method_copy_from()
    if TimeMesh[0][0] =="BrillouinZone":
     c.add_pure_python_method("pytriqs.gf.local._gf_%s_x_X.plot"%(TimeMesh[0][1]), rename = "_plot_")

    module.add_class(c)

module.generate_code()
