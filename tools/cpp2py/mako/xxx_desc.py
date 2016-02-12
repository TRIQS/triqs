<%
    import re
    from util import replace_latex, make_table, replace_cpp_keywords_by_py_keywords
    def deduce_normalized_python_class_name(s) :
        return ''.join([x.capitalize() for x in s.split('_')])

    def decay(s) :
        for tok in ['const ', 'const&', '&&', '&'] :
            s = re.sub(tok,'',s)
        s = s.replace('const_view', 'view') # DISCUSS THIS
        return s.strip()

   # compute used_module_list
    recognized_namespace_for_using = {
        'triqs::gfs::' : 'gf',
        'triqs::operators::many_body_operator' : 'operators',
        'triqs::lattice' : 'lattice_tools'
        }

    using_needed_for_modules = {
        'gf' : 'namespace triqs::gfs',
        'operators' : 'triqs::operators::many_body_operator',
        'lattice_tools' : 'namespace triqs::lattice'
        }

    converters_to_include = {
        'std::map' : 'map',
        'std::set' : 'set',
        'std::vector' : 'vector',
        'std::string' : 'string',
        'std::function' : 'function',
        'std::pair' : 'pair',
        'std::tuple' : 'tuple',
        'triqs::utility::variant' : 'variant'
        }

    used_module_list, converters_list = [], set()
    def analyse(t) :
        if t is None :return
        #global used_module_list
        for ns, mod in recognized_namespace_for_using.items() :
          if decay(t.canonical_name).startswith(ns) :
            used_module_list.append(mod)

        for ns, mod in converters_to_include.items() :
          tname = t.canonical_name.replace('std::__1::','std::') # on OS X, strange __1 name ?
          if ns in decay(tname):
            converters_list.add(mod)

    for c in classes :
        for m in c.constructors :
            for t,n,d in m.params : analyse(t)
        for m in c.methods :
            for t,n,d in m.params : analyse(t)
            analyse(m.rtype)
        if hasattr(c,'proplist'):
            for p in c.proplist :
                analyse(p.getter.rtype)

    for c in classes_of_parameters :
        for m in c.members :
            analyse(m.type)

    for f in functions :
        for t,n,d in f.params : analyse(t)
        analyse(f.rtype)

    used_module_list = set(used_module_list) # makes unique
    converters_list = set(converters_list)
    using_list = [using_needed_for_modules[m] for m in used_module_list]

    def cls(t) :
        tname = decay(t.name)
        if 'gf' in used_module_list: tname = re.sub('triqs::gfs::','',tname)
        if 'parameters' in used_module_list: tname = re.sub('triqs::params::','',tname)
        tname = tname.replace(' ','')
        return tname

    def make_signature(m) :
        assert not m.tparams, "template functions can not be wrapped to Python (%s)"%m.name
        s = "({args})"
        if not m.is_constructor :
          s = cls(m.rtype) + " {name} " + s
        args = ', '.join( ["%s %s"%(cls(t),n) + (" = %s"%d.replace('"','\\"') if d else "") for t,n,d in m.params]) if m.parameter_arg == None else '**%s'%cls(m.params[0][0])
        s = s.format(args = args, **m.__dict__)
        return s.strip()
%>
# Generated automatically using the command :
# ${shell_command}
from wrap_generator import *

# The module
module = module_(full_name = "${modulename}", doc = "${replace_latex(moduledoc)}", app_name = "${appname}")

# All the triqs C++/Python modules
%for mod in used_module_list :
module.use_module('${mod}', 'triqs')
%endfor
##
## All the using
##%for ns in using_list :
##module.add_using('${ns}')
##%endfor

# Add here all includes beyond what is automatically included by the triqs modules
module.add_include("${args.filename.replace("../c++/",'')}")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
%for conv in converters_list :
#include <triqs/python_tools/converters/${conv}.hpp>
%endfor
%for ns in using_list :
using ${ns};
%endfor
%for c in classes :
%for ns in c.ns :
using namespace ${ns};
%endfor
%endfor
%if classes_of_parameters :
#include "./converters.hxx"
%endif
""")
##
%for c in [c for c in classes if not 'ignore_in_python' in c.annotations]:
<%
  def doc_format(member_list):
   h = ['Parameter Name','Type','Default', 'Documentation']
   l = [(m.name, m.ctype, m.initializer if m.initializer else '', replace_latex(m.doc)) for m in member_list]
   return make_table(h, l)
%>
# The class ${c.name}
c = class_(
        py_type = "${deduce_normalized_python_class_name(c.name)}",  # name of the python class
        c_type = "${c.name}",   # name of the C++ class
        doc = r"${replace_latex(c.doc, True)}",   # doc of the C++ class
%if 0 :
        #
        #Hereafter several options to be selected by hand. Cf doc
        #has_iterator = True,
        #boost_serializable= True,
        #is_printable= True,
        #arithmetic = ("algebra","double")
%endif
)

%for m in c.members :
c.add_member(c_name = "${m.name}",
             c_type = "${m.ctype}",
             read_only= False,
             doc = """${replace_cpp_keywords_by_py_keywords(replace_latex(m.doc, True))} """)

%endfor
##
%for m in [m for m in c.constructors if not m.is_template and not 'ignore_in_python' in m.annotations]:
c.add_constructor("""${make_signature(m)}""",
                  doc = """${replace_cpp_keywords_by_py_keywords(replace_latex(m.doc, True)) if m.parameter_arg==None else doc_format(m.parameter_arg.members) } """)

%endfor
##
%for m in [m for m in c.methods if not 'ignore_in_python' in m.annotations]:
c.add_method("""${make_signature(m)}""",
             %if m.is_static :
             is_static = True,
             %endif
             doc = """${replace_cpp_keywords_by_py_keywords(replace_latex(m.doc,True)) if m.parameter_arg==None else doc_format(m.parameter_arg.members) } """)

%endfor
##
%if hasattr(c,'proplist'):
    %for p in [p for p in c.proplist]:
c.add_property(name = "${p.name}",
               getter = cfunction("${make_signature(p.getter)}"),
               %if p.setter :
               setter = cfunction("${make_signature(p.setter)}"),
               %endif
               doc = """${replace_latex(p.doc, True)} """)

    %endfor
%endif
##
module.add_class(c)

%endfor
##
%for f in [f for f in functions if not 'ignore_in_python' in f.annotations]:
module.add_function ("${make_signature(f)}", doc = """${replace_cpp_keywords_by_py_keywords(replace_latex(f.doc, True))}""")

%endfor
##
module.generate_code()

