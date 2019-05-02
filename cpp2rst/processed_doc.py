import re
from collections import OrderedDict
import cpp2py.clang_parser as CL
 
"""
Meaning of the @option in the doc:

 * @param   X   Documentation of parameter X for a function
 * @return      Doc of the return of a function
 * @tparam  T   Documentation of a template parameter of a function or a class

 * @synopsis  An explicit synopsis for the automatically generated one, for a function.
 * @value     Overrules the type of a using/typedef.
 * @warning     Warning
 * @figure     An illustration
 * @note        Note
 * @include     For a class, a function, the top level file to include [default: the definition file]
 * @example filename  File name of the example. Path should start from doc/reference/c++

"""

# --------------------------------- 

def replace_latex(s, escape_slash=False):
    """replace 
       $XX X$  by :math:`XX X`
       $$X XX$$  by \n\n.. math:\n\t\tX XX\n\n..\n
       [[ XXX]]  by :ref:` XXX`
     
    """
    assert not escape_slash # useless
    
    text=re.sub('\$\$([^\$]+)\$\$', r'\n\n.. math::\n\t\t\1\n\n..\n', s, flags= re.DOTALL) # multiline math
    text=re.sub('\$([^\$]+)\$', r':math:`\1`', text)
 
    if escape_slash: text=text.encode('string_escape')
    return text

# --------------------------------- 

def clean_doc_string(s):
    for p in [r"/\*",r"\*/",r"^\s*\*", r'\*\s*\n', r'\*/\s*$',r"///", r"//", r"\\brief"] : 
        s = re.sub(p,"",s, flags = re.MULTILINE)
    return s

# ------------------------------------------------------------------------

class ProcessedDoc: 
    
    fields_allowed_in_docs = ['include', 'return', 'synopsis', 'warning','figure', 'note', 'example', 'param', 'tparam', 'group', 'head', 'tail', 'category'] 
    fields_with_multiple_entry = ['param', 'tparam']

    """
      Result of the processing of the documentation string for a function or a class
      
      self.brief_doc : first line
      self.doc       : rest of the doc
      self.elements : dict with all fields @XXX -> Value
                      params, tparams will always be a list, if present 

      Replace latex with proper rst call in all fields.
    """
    def __init__(self, node): 
        raw_doc = node.raw_comment
        if not raw_doc : raw_doc = "\n\n" # default value
        
        # Clean *, &&, /// and co.
        doc = clean_doc_string(raw_doc.strip())
        
        # split : the first line is brief, and the rest
        doc = replace_latex(doc)
        if '$' in doc : 
            print "FAILED to process the latex for node %s"%CL.fully_qualified(node)
            print doc
        doc2 = doc.strip().split('@',1)[0] # Get rid of everything after the first @
        spl = doc2.strip().split('\n',1) 
        self.brief_doc, self.doc = spl[0], (spl[1] if len(spl)>1 else '') 
        assert '@' not in self.doc, "ouch!"

        # Extract the @XXXX elements with a regex @XXXX YYYY (YYYY can be multiline).
        d = dict( (key, []) for key in self.fields_with_multiple_entry)
        regex = r'@(\w+)\s*([^@]*)'
        for m in re.finditer(regex, doc, re.DOTALL):
            key, val = m.group(1), replace_latex(m.group(2)).strip()
            if key not in self.fields_allowed_in_docs:
                print "Field %s is not recognized"%key
            if key in self.fields_with_multiple_entry:
                d[key].append(val)
            else:
                d[key] = val
        self.elements = d

