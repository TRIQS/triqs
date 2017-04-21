
#include "../wrapper_tools.hpp"
#include <triqs/gfs.hpp>

#include "./gf.hpp"

//
namespace triqs {
 namespace py_tools {

  // Converter for Block gf
  template <typename... T> struct py_converter<triqs::gfs::block_gf_view<T...>> {

   using gf_type      = triqs::gfs::gf<T...>;
   using gf_view_type = triqs::gfs::gf_view<T...>;
   using c_type       = triqs::gfs::block_gf_view<T...>;

   static PyObject* c2py(c_type g) {
    // rm the view_proxy
    std::vector<gf_view_type> vg;
    vg.reserve(g.data().size());
    for (auto const& x : g.data())
     vg.push_back(x);
    pyref v_gf    = convert_to_python(vg);
    pyref v_names = convert_to_python(g.block_names());
    if (v_gf.is_null() or v_names.is_null()) return NULL;
    pyref cls     = pyref::module("pytriqs.gf").attr("BlockGf");
    if (cls.is_null()) return NULL;
    pyref kw = PyDict_New();
    PyDict_SetItemString(kw, "name_list", v_names);
    PyDict_SetItemString(kw, "block_list", v_gf);
    pyref empty_tuple = PyTuple_New(0);
    return PyObject_Call(cls, empty_tuple, kw);
   }

   static bool is_convertible(PyObject* ob, bool raise_exception) {
    pyref cls = pyref::module("pytriqs.gf").attr("BlockGf");
    if (cls.is_null()) TRIQS_RUNTIME_ERROR << "Cannot find the pytriqs.gf.BlockGf";
    int i = PyObject_IsInstance(ob, cls);
    if (i == -1) { // an error has occurred
     i = 0;
     if (!raise_exception) PyErr_Clear();
    }
    if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a BlockGf");

    pyref x   = borrowed(ob);
    pyref gfs = x.attr("_BlockGf__GFlist");
    return (i && py_converter<std::vector<gf_view_type>>::is_convertible(gfs, raise_exception));
   }

   static c_type py2c(PyObject* ob) {
    pyref x     = borrowed(ob);
    pyref names = x.attr("_BlockGf__indices");
    pyref gfs   = x.attr("_BlockGf__GFlist");
    return make_block_gf_view(convert_from_python<std::vector<std::string>>(names),
                              convert_from_python<std::vector<gf_view_type>>(gfs));
   }
  };

  // Treat the block_gf as view
  template <typename... T> struct py_converter<triqs::gfs::block_gf<T...>> : py_converter<triqs::gfs::block_gf_view<T...>> {};
 }
}
