# Generated automatically using the command :
# ${W.shell_command}
from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "${W.modulename}", doc = r"${doc.replace_latex(W.moduledoc)}", app_name = "${W.appname}")

# Imports
%if import_list:
module.add_imports(*${import_list})
%endif

# Add here all includes 
module.add_include("${W.filename.split('c++/')[-1]}")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
%for conv in converters_list :
#include <${conv}>
%endfor

%for ns in using_list :
using namespace ${ns};
%endfor
""")
##

%for e in W.all_enums:
module.add_enum("${e.spelling}", ${["%s::%s"%(e.spelling, x.spelling) for x in e.get_children()]}, "${CL.get_namespace(e)}", doc = r"""${doc.make_doc(e)}""")
%endfor

##
%for c in W.all_classes:
# The class ${c.spelling}
c = class_(
        py_type = "${util.deduce_normalized_python_class_name(c.spelling)}",  # name of the python class
        c_type = "${c.type.get_canonical().spelling}",   # name of the C++ class
        doc = r"""${doc.make_doc_class(c)}""",   # doc of the C++ class
        hdf5 = ${'True' if W.has_hdf5_scheme(c) else 'False'},
)
<%
  methods, proplist = W.separate_method_and_properties(c)
%>
%for m in CL.get_members(c, False, CL.is_public) :
c.add_member(c_name = "${m.spelling}",
             c_type = "${m.type.spelling}",
             read_only= ${W.members_read_only},
             doc = r"""${doc.make_doc(m)}""")

%endfor
##
%for m in CL.get_constructors(c, W.keep_fnt):
c.add_constructor("""${W.make_signature_for_desc(m, True)}""", doc = r"""${doc.make_doc_function(m)}""")

%endfor
##
%for m in methods:
c.add_method("""${W.make_signature_for_desc(m)}""",
              %if m.spelling == "operator()" : 
               name = "__call__",
              %endif
             %if m.is_static_method() :
             is_static = True,
             %endif
             doc = r"""${doc.make_doc_function(m)}""")

%endfor
##
%for p in proplist:
c.add_property(name = "${p.name}",
               getter = cfunction("${W.make_signature_for_desc(p.getter)}"),
               %if p.setter :
               setter = cfunction("${W.make_signature_for_desc(p.setter)}"),
               %endif
               doc = r"""${doc.clean_doc_string(p.doc)}""")

%endfor
##
module.add_class(c)

%endfor
##
%for f in W.all_functions:
module.add_function ("${W.make_signature_for_desc(f, is_free_function = True)}", doc = r"""${doc.make_doc_function(f)}""")

%endfor

## converters
%for c in W.param_cls_list:
# Converter for ${c.spelling}
c = converter_(    
        c_type = "${c.type.spelling}", 
        doc = r"""${doc.make_doc(c)}""",  
)
%for m in CL.get_members(c, True):
c.add_member(c_name = "${m.spelling}",
             c_type = "${m.type.spelling}",
             initializer = """ ${CL.get_member_initializer(m)} """,
             doc = r"""${doc.make_doc(m)}""")

%endfor
module.add_converter(c)

%endfor

##
module.generate_code()

