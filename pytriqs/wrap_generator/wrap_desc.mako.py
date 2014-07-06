from wrap_generator import *

# The module
module = module_(full_name = "${modulename}", 
                 doc = " ")

#
# Need to add here the necessary include for compilation
#module.add_include("<iostream>") 
#module.add_include("<triqs/xxxxx.hpp>") 

# Some include, using, etc... Cf doc.
module.add_preamble("""
""")

<% 
  def deduce_normalized_python_class_name(s) :                         
    return ''.join([x.capitalize() for x in s.split('_')])

  def make_signature(m) :
    assert not m.template_list, "template functions can not be wrapped to Python"
    s = "{rtype} {name} ({args})" if not m.is_constructor else "({args})"
    s = s.format(args = ', '.join( ["%s %s"%t_n for t_n in m.params_decay]), **m.__dict__) 
    return s.strip()

%>

%for c in classes :  
g = class_(
        py_type = "${deduce_normalized_python_class_name(c.name)}",  # name of the python class
        c_type = "${c.name}",   # name of the C++ class
        #
        # Hereafter several options to be selected by hand. Cf doc 
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

