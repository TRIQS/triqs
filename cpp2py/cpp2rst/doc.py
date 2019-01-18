from cpp2py.doc import replace_latex, process_doc
import re
from collections import OrderedDict

class ProcessDoc: 
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

 * @manual_methods  Comma separated list of methods which are referenced in the table,
                    but the page of doc is not generated. It will be written manually.
 * @manual_friends  Comma separated list of non member functions which are referenced in the table,
                    but the page of doc is not generated. It will be written manually.

The @ MUST BE THE LAST FIELDS
    """
    def __init__(self, node, var_unique, var_multi) : 
        def cls(s) : 
            return re.sub(r'\\n',r'\n', s).strip()
        
        doc = node.raw_comment if node.raw_comment else "\n\n"
        #print "COMMENT", doc
        for p in [r"/\*",r"\*/",r"^\s*\*", r"///", r"//", r"\\brief"] : 
            doc = re.sub(p,"",doc, flags = re.MULTILINE)
        spl = doc.strip().split('\n',1) 
        self.brief_doc, doc = spl[0], spl[1] if len(spl)>1 else '' 

        spl = doc.split('@')

        d = {}
        for x in var_multi:
            if x not in d: d[x] = []
        for x in spl[1:]:
            item, rest= (x + ' ').split(' ',1)
            if item in var_unique:
                if item not in d:
                    d[item] = rest
                else : 
                    print "@%s is present more than once !"%item 
            elif item in var_multi:
                value, comment = rest.split(' ',1)
                d[item].append((value, comment))
            else:
                print "@%s not recognized"%item
        for x in var_unique:
            if x not in d: d[x] = ''
        
        self.processed_doc = spl[0]
        self.elements = d
