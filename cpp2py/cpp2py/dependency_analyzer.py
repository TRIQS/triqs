import util, importlib
from cpp2py_info_base import Cpp2pyInfoStd
  
class DependencyAnalyzer:
    """
    A parametrized function that takes a list of AST node of types
    and deduces the list of python modules to import and converters to include
    """
  
    basic_types = ["void", "bool", "int", "long", "long long", "unsigned int", "unsigned long", "unsigned long long", "double"]
  
    def __init__(self, modules_with_converter_list):
        """
        Parameters
        ----------
        modules_with_converter_list : list of string
                                      list of module/package containing a Cpp2pyInfo class
        """
        self.get_imp_conv= [Cpp2pyInfoStd] 
        for f in modules_with_converter_list:
            try : 
                C = importlib.import_module(f)
                self.get_imp_conv.append(C.Cpp2pyInfo)
            except AttributeError:
                raise RuntimeError, "%s is not a proper converter.\nThis Python module does not exist or does not have the Cpp2pyInfo class"%f

    def __call__(self, type_node_list, types_being_wrapped_or_converted): 
        """
         Parameters
         ------------

         type_node_list : list/gen of AST node of type
                          a list of types
         
         Return
         -------

         two sets : python modules to import, converters to include
        """
        ignored = [util.decay(x.type.get_canonical().spelling) for x in types_being_wrapped_or_converted]
        m,c = set(), set()
        unknown_types = {}
        for x in type_node_list : 
            m1,c1 = set(), set()
            can = util.decay(x.get_canonical().spelling)
            for info_cls in self.get_imp_conv:
                c1 |= set(info_cls().get_converters(can))
                m1 |= set(info_cls().get_imports(can))
            if not (can in self.basic_types or can in ignored or c1 or m1) : 
                unknown_types[util.decay(x.get_canonical().spelling)] = util.decay(x.spelling)
            c |= c1
            m |= m1
        if unknown_types:
            print 20*'=' + "\nError : The following types can not be converted: \n"
            for can, x in unknown_types.items() : 
                print "%s (%s)"%(x, can)
            raise TypeError, ""
        return sorted(list(m)), sorted(list(c))

