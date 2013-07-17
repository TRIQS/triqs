#include <Python.h>
#include <boost/python.hpp>

namespace triqs { namespace python_tools {

// A simple wrapper
class boost_object {

  boost::python::handle<> hdl;

  public:

  boost_object(PyObject * py_ob) : hdl(boost::python::borrowed(py_ob)) {}

  operator boost::python::object () { return boost::python::object(hdl); }
  operator boost::python::list () { return boost::python::list(hdl); }
  operator boost::python::dict () { return boost::python::dict(hdl); }

};

}}
