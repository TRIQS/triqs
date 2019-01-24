namespace cpp2py { 

template <> struct py_converter<${c.c_type}> {
 static PyObject *c2py(${c.c_type} & x) {
  PyObject * d = PyDict_New();
  %for m in c.members :
  PyDict_SetItemString( d, ${'"%s"'%m.c_name}, convert_to_python(x.${m.c_name}));
  %endfor
  return d;
 }

 static PyObject *c2py(${c.c_type} && x) {
   return c2py(x);
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

 static ${c.c_type} py2c(PyObject *dic) {
  ${c.c_type} res;
  %for m in c.members:
  %if m.initializer == '' : 
  res.${m.c_name} = convert_from_python<${m.c_type}>(PyDict_GetItemString(dic, "${m.c_name}"));
  %else:
  _get_optional(dic, "${m.c_name}", res.${m.c_name} ${',' + m.initializer if m.initializer !="{}" else ''});
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
  std::vector<std::string> ks, all_keys = {${','.join('"%s"'%m.c_name for m in c.members)}};
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
  %if m.initializer:
  _check_optional <${m.c_type}>(dic, fs, err, "${m.c_name}", "${m.c_type}");
  %else:
  _check_mandatory<${m.c_type}>(dic, fs, err, "${m.c_name}", "${m.c_type}"); 
  %endif
  %endfor
  if (err) goto _error;
  return true;
  
 _error: 
   fs2 << "\n---- There " << (err > 1 ? "are " : "is ") << err<< " error"<<(err >1 ?"s" : "")<< " in Python -> C++ transcription for the class ${c.c_type}\n" <<fs.str();
   if (raise_exception) PyErr_SetString(PyExc_TypeError, fs2.str().c_str());
  return false;
 }
};

}
