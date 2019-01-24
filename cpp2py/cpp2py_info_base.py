import re
class Cpp2pyInfoBase:
    """
       Derive to implement table_converters, table_imports
    """

    def __init__(self):
        pass

    def find_in_table(self, x, table):
        r = set()
        for a,b in table.items() :
          if re.compile(a).search(x): r.add(b)
        return r

    def get_converters(self, can) : 
        return self.find_in_table(can, self.table_converters)
    
    def get_imports(self, can) : 
        return self.find_in_table(can, self.table_imports)


class Cpp2pyInfoStd(Cpp2pyInfoBase):

    # No Python module to import for std:: object
    table_imports = {}

    # Which converters to import : syntax is :
    # regex to match the canonical C++ type : full name of the converters
    _table_converters = {
        'std::.*complex' : 'complex',
        'std::.*map' : 'map',
        'std::.*set' : 'set',
        'std::.*vector' : 'vector',
        'std::.*string' : 'string',
        'std::.*function' : 'function',
        'std::.*pair' : 'pair',
        'std::.*tuple' : 'tuple',
        'std::.*optional' : 'optional',
        'std::.*variant' : 'variant',
        }

    table_converters = dict ( (k, "cpp2py/converters/%s.hpp"%v) for (k,v) in _table_converters.items())
 
