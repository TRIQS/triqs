# This module contains a few utilities
import re, os
import cpp2py.clang_parser as CL
#from mako.template import Template

# Must be here
def script_path() : 
   return os.path.dirname(os.path.abspath( __file__ ))

def clean_end_and_while_char(s):
    return re.sub(re.compile(r"[ \t\r\f\v]+$",re.MULTILINE),'', s.strip())# clean end and white char 

def get_decl_param_class(f):
    """ Given a node f of a function, returns the node of declaration of the param class or None"""
    if 'use_parameter_class' not in CL.get_annotations(f) : 
        return None
    p = list(CL.get_params(f))
    assert len(p) == 1, "A function/method with PARAM technique must have exacly one parameter"
    return CL.jump_to_declaration(p[0])

def use_parameter_class(m): 
    return 'use_parameter_class' in CL.get_annotations(m) 

def decay(s) :
    for tok in ['const ', 'const&', '&&', '&'] :
        s = re.sub(tok,'',s)
    s = s.replace('const_view', 'view') # DISCUSS THIS
    return s.strip()

def deduce_normalized_python_class_name(s) :
    return ''.join([x.capitalize() for x in s.split('_')])


