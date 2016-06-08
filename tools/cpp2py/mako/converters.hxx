// DO NOT EDIT
// Generated automatically using libclang using the command : 
// ${shell_command}
%for c in classes :
<%
  def name_lmax(member_list) : 
   return max(len(m.name) for m in member_list)

  def name_format(name) : 
    f =  '{:<%s}'%(name_lmax(c.members)+2)
    return f.format(name)

  def name_format_q(name) : return name_format('"%s"'%name) 
 
  def type_format(name) : 
    f  = '{:<%s}'%(max(len(m.ctype) for m in c.members))
    return f.format(name)
%>

// --- C++ Python converter for ${c.name}
#include <triqs/python_tools/converters/vector.hpp>
#include <triqs/python_tools/converters/string.hpp>

namespace triqs { namespace py_tools {

template <> struct py_converter<${c.name}> {
 static PyObject *c2py(${c.name} const & x) {
  PyObject * d = PyDict_New(); 
  %for m in c.members :
  PyDict_SetItemString( d, ${name_format('"%s"'%m.name)}, convert_to_python(x.${m.name}));
  %endfor
  return d;
 }

 template <typename T, typename U> static void _get_optional(PyObject *dic, const char *name, T &r, U const &init_default) {
  if (PyDict_Contains(dic, pyref::string(name)))
   r = convert_from_python<T>(PyDict_GetItemString(dic, name));
  else
   r = init_default;
 }

 template <typename T> static void _get_optional(PyObject *dic, const char *name, T &r) {
  if (PyDict_Contains(dic, pyref::string(name)))
   r = convert_from_python<T>(PyDict_GetItemString(dic, name));
  else
   r = T{};
 }

 static ${c.name} py2c(PyObject *dic) {
  ${c.name} res;
  %for m in c.members :
  %if m.initializer == None : 
  res.${m.name} = convert_from_python<${m.ctype}>(PyDict_GetItemString(dic, "${m.name}"));
  %else:
  _get_optional(dic, ${name_format_q(m.name)}, res.${name_format(m.name)} ${',' + m.initializer if m.initializer.strip() !="{}" else ''});
  %endif
  %endfor
  return res;
 }

 template <typename T>
 static void _check(PyObject *dic, std::stringstream &fs, int &err, const char *name, const char *tname) {
  if (!convertible_from_python<T>(PyDict_GetItemString(dic, name), false))
   fs << "\n" << ++err << " The parameter " << name << " does not have the right type : expecting " << tname
      << " in C++, but got '" << PyDict_GetItemString(dic, name)->ob_type->tp_name << "' in Python.";
 }

 template <typename T>
 static void _check_mandatory(PyObject *dic, std::stringstream &fs, int &err, const char *name, const char *tname) {
  if (!PyDict_Contains(dic, pyref::string(name)))
   fs << "\n" << ++err << " Mandatory parameter " << name << " is missing.";
  else _check<T>(dic,fs,err,name,tname);
 }

 template <typename T>
 static void _check_optional(PyObject *dic, std::stringstream &fs, int &err, const char *name, const char *tname) {
  if (PyDict_Contains(dic, pyref::string(name))) _check<T>(dic, fs, err, name, tname);
 }

 static bool is_convertible(PyObject *dic, bool raise_exception) {
  if (dic == nullptr or !PyDict_Check(dic)) {
   if (raise_exception) { PyErr_SetString(PyExc_TypeError, "The function must be called with named arguments");}
   return false;
  }  
  std::stringstream fs, fs2; int err=0;

#ifndef TRIQS_ALLOW_UNUSED_PARAMETERS
  std::vector<std::string> ks, all_keys = {${','.join('"%s"'%m.name for m in c.members)}};
  pyref keys = PyDict_Keys(dic);
  if (!convertible_from_python<std::vector<std::string>>(keys, true)) {
   fs << "\nThe dict keys are not strings";
   goto _error;
  }
  ks = convert_from_python<std::vector<std::string>>(keys);
  for (auto & k : ks)
   if (std::find(all_keys.begin(), all_keys.end(), k) == all_keys.end())
    fs << "\n"<< ++err << " The parameter '" << k << "' is not recognized.";
#endif

  %for m in c.members :
  %if m.initializer == None : 
  _check_mandatory<${type_format(m.ctype)}>(dic, fs, err, ${name_format_q(m.name)}, "${m.ctype}"); 
  %else:
  _check_optional <${type_format(m.ctype)}>(dic, fs, err, ${name_format_q(m.name)}, "${m.ctype}");
  %endif
  %endfor
  if (err) goto _error;
  return true;
  
 _error: 
   fs2 << "\n---- There " << (err > 1 ? "are " : "is ") << err<< " error"<<(err >1 ?"s" : "")<< " in Python -> C++ transcription for the class ${c.name}\n" <<fs.str();
   if (raise_exception) PyErr_SetString(PyExc_TypeError, fs2.str().c_str());
  return false;
 }
};

}}
%endfor
