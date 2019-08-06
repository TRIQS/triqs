import re, itertools, os
from collections import OrderedDict
import cpp2py.clang_parser as CL
import global_vars

 
"""
Meaning of the @option in the doc:

 @tparam  T   Documentation of a template parameter of a function or a class
 @param   X   Documentation of parameter X for a function
 @return      Doc of the return of a function

 @warning    Warning
 @note       Note
 @figure     An illustration
 
 @include   For a class, a function, the top level file to include [default: the definition file]
 @example filename  File name of the example. Path should start from doc/documentation/examples

 @head  head and tail are the two parts in common for all overload.
 @tail   Cf  
 @brief Short description used in tables

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
    if '\t'  in s:
        print "WARNING : TABS ", s.replace('\t','TAB')

    for p in [r"^\s*/\*\*?",r"\*/",r"^\s*\*",  r'^\s*\*/\s*$',r"^\s*///", r"^s*//"]:
        s = re.sub(p,"",s, flags = re.MULTILINE)
    return s

# ------------------------------------------------------------------------

def strip_left_spaces(s):
    """Remove the left spaces so that the minimal space for at least one line is 0"""
    lines = s.rstrip().split('\n')
    l2 = [l for l in lines if l.strip()] # non empty lines
    min_space = min ( sum( 1 for _ in itertools.takewhile(str.isspace,a) ) for a in l2) if l2 else 0
    return '\n'.join(l[min_space:] for l in lines)

# ------------------------------------------------------------------------

class ProcessedDoc: 
    
    fields_allowed_in_docs = ['include', 'return', 'warning','figure', 'note', 'brief', 'example', 'param', 'tparam', 'group', 'head', 'tail', 'category'] 
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
        doc = clean_doc_string(raw_doc).rstrip() # do NOT remove leading space
        
        # split : the first line is brief, and the rest
        doc = replace_latex(doc)
        if '$' in doc : 
            print "FAILED to process the latex for node %s"%CL.fully_qualified(node)
            print doc
        doc2 = doc.split('@',1)[0] # Get rid of everything after the first @
        self.doc = doc2

        assert '@' not in self.doc, "Internal Error in doc processing !"

        # Process the main part of the doc
        self.doc = strip_left_spaces(self.doc)

        # Extract the @XXXX elements with a regex @XXXX YYYY (YYYY can be multiline).
        d = dict( (key, []) for key in self.fields_with_multiple_entry)
        regex = r'@(\w+)\s(\s*[^@]*)'
        for m in re.finditer(regex, doc, re.DOTALL):
            key, val = m.group(1), replace_latex(m.group(2)).rstrip()
            if key not in self.fields_allowed_in_docs:
                print "Field %s is not recognized"%key
            if key in self.fields_with_multiple_entry:
                d[key].append(val)
            else:
                d[key] = val
        self.elements = d
        if 'brief' not in d : d['brief']=''

        # if 'return' in d : print d['return']

        # print doc
        # print raw_doc
        # print d['param']

        # Final adjustement
        if 'brief' not in d : d['brief']=''

        if 'example' not in d : # take the default
            filename=  "%s.cpp"%(CL.fully_qualified_name(node).replace("::",'/'))
            filename = os.path.join(global_vars.examples_root, filename)
            if os.path.exists(filename):
                d['example'] =  filename

        if 'include' not in d :
           ns = CL.get_namespace_list(node)
           d['include'] = '/'.join(ns) + '.hpp'
