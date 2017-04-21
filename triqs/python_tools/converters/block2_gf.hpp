#include "../wrapper_tools.hpp"
#include <triqs/gfs.hpp>
#include "./gf.hpp"

//
namespace triqs {
 namespace py_tools {

  // Converter for Block gf
  template <typename... T> struct py_converter<triqs::gfs::block2_gf_view<T...>> {

   // using gf_type = triqs::gfs::gf<T...>;
   using gf_view_type = triqs::gfs::gf_view<T...>;
   using c_type       = triqs::gfs::block2_gf_view<T...>;

   static PyObject* c2py(c_type g) {
    std::vector<std::vector<gf_view_type>> vvg;
    vvg.reserve(g.data().size());
    for (auto const& x : g.data()) {
     std::vector<gf_view_type> vg;
     vg.reserve(x.size());
     for (auto const& y : x)
      vg.push_back(y);
     vvg.push_back(vg);
    }
    pyref v_gf     = convert_to_python(vvg);
    pyref v_names1 = convert_to_python(g.block_names()[0]);
    pyref v_names2 = convert_to_python(g.block_names()[1]);
    if (v_gf.is_null() or v_names1.is_null() or v_names2.is_null()) return NULL;
    pyref cls      = pyref::module("pytriqs.gf").attr("Block2Gf");
    if (cls.is_null()) return NULL;
    pyref args = PyTuple_Pack(3, (PyObject*)v_names1, (PyObject*)v_names2, (PyObject*)v_gf);
    return PyObject_Call(cls, args, NULL);
   }

   static bool is_convertible(PyObject* ob, bool raise_exception) {
    pyref cls = pyref::module("pytriqs.gf").attr("Block2Gf");
    if (cls.is_null()) TRIQS_RUNTIME_ERROR << "Cannot find the pytriqs.gf.Block2Gf";
    int i = PyObject_IsInstance(ob, cls);
    if (i == -1) { // an error has occurred
     i = 0;
     if (!raise_exception) PyErr_Clear();
    }
    if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a Block2Gf");

    pyref x   = borrowed(ob);
    pyref gfs = x.attr("_Block2Gf__GFlist");
    return (i && py_converter<std::vector<std::vector<gf_view_type>>>::is_convertible(gfs, raise_exception));
   }

   static c_type py2c(PyObject* ob) {
    pyref x      = borrowed(ob);
    pyref names1 = x.attr("_Block2Gf__indices1");
    pyref names2 = x.attr("_Block2Gf__indices2");
    pyref gfs    = x.attr("_Block2Gf__GFlist");
    return make_block2_gf_view(convert_from_python<std::vector<std::string>>(names1),
                               convert_from_python<std::vector<std::string>>(names2),
                               convert_from_python<std::vector<std::vector<gf_view_type>>>(gfs));
   }
  };

  // Treat the block2_gf as view
  template <typename... T> struct py_converter<triqs::gfs::block2_gf<T...>> : py_converter<triqs::gfs::block2_gf_view<T...>> {};
 }
}
