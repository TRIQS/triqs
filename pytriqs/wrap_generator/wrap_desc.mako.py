<%
    import re
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
        'triqs::params::' : 'parameters',
        'triqs::utility::many_body_operator' : 'operators2',
        }

    using_needed_for_modules = {
        'gf' : 'namespace triqs::gfs',
        'parameters' : 'namespace triqs::params',
        'operators2' : 'triqs::utility::many_body_operator',
        }

    used_module_list = []
    def analyse(t) :
        if t is None :return
        #global used_module_list
        for ns, mod in recognized_namespace_for_using.items() :
          if decay(t.canonical_name).startswith(ns) :
            used_module_list.append(mod)

    for c in classes :
        for m in c.constructors :
            for t,n,d in m.params : analyse(t)
        for m in c.methods :
            for t,n,d in m.params : analyse(t)
            analyse(m.rtype)
        for p in c.proplist :
            analyse(p.getter.rtype)

    for c in classes_of_parameters :
        for m in c.members : 
            analyse(m.type)

    for f in functions :
        for t,n,d in f.params : analyse(t)
        analyse(f.rtype)

    used_module_list = set(used_module_list) # makes unique
    using_list = [using_needed_for_modules[m] for m in used_module_list]

    def cls(t) :
        tname = decay(t.name)
        if 'gf' in used_module_list: tname = re.sub('triqs::gfs::','',tname)
        if 'parameters' in used_module_list: tname = re.sub('triqs::params::','',tname)
        tname = tname.replace(' ','')
        return tname

    def make_signature(m) :
        assert not m.template_list, "template functions can not be wrapped to Python"
        s = "({args})"
        if not m.is_constructor :
          s = cls(m.rtype) + " {name} " + s
        args = ', '.join( ["%s %s"%(cls(t),n) + (" = %s"%d if d else "") for t,n,d in m.params]) if m.parameter_arg == None else '**%s'%cls(m.params[0][0])
        s = s.format(args = args, **m.__dict__)
        return s.strip()
%>
# Generated automatically using the command : 
# ${shell_command}
from wrap_generator import *

# The module
module = module_(full_name = "${modulename}", doc = "${moduledoc}")

# All the triqs C++/Python modules
%for mod in used_module_list :
module.use_module('${mod}')
%endfor
##
## All the using 
##%for ns in using_list :
##module.add_using('${ns}')
##%endfor

# Add here all includes beyond what is automatically included by the triqs modules
module.add_include("${args.filename}")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
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
%for c in classes :
<%
  def doc_format(member_list) : 
   h= ['Parameter Name', 'Type', 'Default', 'Documentation']
   n_lmax = max(len(h[0]), max(len(m.name) for m in member_list))
   type_lmax = max(len(h[1]), max(len(m.ctype) for m in member_list))
   opt_lmax = max(len(h[2]), max(len(m.initializer) for m in member_list if m.initializer))
   doc_lmax = max(len(h[3]), max(len(m.doc) for m in member_list))
   form =  "  {:<%s} {:<%s} {:<%s} {:<%s}"%(n_lmax, type_lmax, opt_lmax, doc_lmax)
   header = form.format(*h)
   r = '\n'.join( form.format(m.name, m.ctype, m.initializer if m.initializer else '--', m.doc) for m in member_list)
   return header + '\n\n' + r
%>
# The class ${c.name}
c = class_(
        py_type = "${deduce_normalized_python_class_name(c.name)}",  # name of the python class
        c_type = "${c.name}",   # name of the C++ class
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
             doc = """${m.doc} """)

%endfor
##
%for m in [m for m in c.constructors if not m.is_template]:
c.add_constructor("""${make_signature(m)}""", 
                  doc = """${m.doc} """)

%endfor
##
%for m in c.methods:
c.add_method("""${make_signature(m)}""", 
             %if m.is_static :
             is_static = True,
             %endif
             doc = """${m.doc if m.parameter_arg==None else doc_format(m.parameter_arg.members) } """)

%endfor
##
%for p in [p for p in c.proplist]: 
c.add_property(name = "${p.name}", 
               getter = cfunction("${make_signature(p.getter)}"),
               %if p.setter : 
               setter = cfunction("${make_signature(p.setter)}"),
               %endif
               doc = """${p.doc} """)

%endfor
##
module.add_class(c)

%endfor
##
%for f in functions :  
module.add_function ("${make_signature(f)}", doc = "${f.doc}")

%endfor
##
module.generate_code()

