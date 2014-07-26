<%
    import re
    def deduce_normalized_python_class_name(s) :
        return ''.join([x.capitalize() for x in s.split('_')])

    def decay(s) :
        for tok in ['const', '&&', '&'] :
            s = re.sub(tok,'',s)
        return s.strip()

   # compute used_module_list
    recognized_namespace_for_using = {
        'triqs::gfs::' : 'gf',
        'triqs::params::' : 'parameters',
        'triqs::utility::many_body_operator' : 'operators2',
        }

    used_module_list = []
    def analyse(t) :
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

    for f in functions :
        for t,n,d in f.params : analyse(t)
        analyse(f.rtype)

    used_module_list = set(used_module_list) # makes unique

    def cls(t) :
        tname = decay(t.name)
        if 'gf' in used_module_list: tname = re.sub('triqs::gfs::','',tname)
        if 'parameters' in used_module_list: tname = re.sub('triqs::params::','',tname)
        return tname

    def make_signature(m) :
        assert not m.template_list, "template functions can not be wrapped to Python"
        s = "({args})"
        if not m.is_constructor :
          s = cls(m.rtype) + " {name} " + s
        s = s.format(args = ', '.join( ["%s %s"%(cls(t),n) + (" = %s"%d if d else "") for t,n,d in m.params]), **m.__dict__)
        return s.strip()
%>
##
##
# Generated automatically using libclang using the command : 
# ${shell_command}
from wrap_generator import *

# The module
module = module_(full_name = "${modulename}", doc = " ")

# All the triqs C++/Python modules
%for mod in used_module_list :
module.use_module('${mod}')
%endfor

# Add here all includes beyond what is automatically included by the triqs modules
module.add_include("${args.filename}")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
// using namespace XXX;
""")

##
%for c in classes :
# The class ${c.name}
g = class_(
        py_type = "${deduce_normalized_python_class_name(c.name)}",  # name of the python class
        c_type = "${c.name}",   # name of the C++ class
        #
        #Hereafter several options to be selected by hand. Cf doc
        #has_iterator = True,   
        #boost_serializable= True,
        #is_printable= True,
        #arithmetic = ("algebra","double")
        )

%for m in c.members : 
g.add_member(c_name = "${m.name}", 
             c_type = "${m.ctype}", 
             read_only= False, 
             doc = """${m.doc} """)

%endfor
##
%for m in [m for m in c.constructors if not m.is_template]:
g.add_constructor("${make_signature(m)}", 
                  doc = """${m.doc} """)

%endfor
##
##
%for m in [m for m in c.methods]: 
g.add_method("${make_signature(m)}", 
             %if m.is_static :
             is_static = True,
             %endif
             doc = """${m.doc} """)

%endfor
##
%for p in [p for p in c.proplist]: 
g.add_property(name = "${p.name}", 
               getter = cfunction("${make_signature(p.getter)}"),
               %if p.setter : 
               setter = cfunction("${make_signature(p.setter)}"),
               %endif
               doc = """${p.doc} """)

%endfor
##
module.add_class(g)

%endfor
##
%for f in functions :  
module.add_function ("${make_signature(f)}", doc = "${f.doc}")

%endfor
##
module.generate_code()

