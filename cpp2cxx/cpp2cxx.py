import os, re, sys, itertools
from mako.template import Template
import cpp2py.clang_parser as CL

class Cpp2Cxx: 
    """ """
    def __init__(self, filename, namespaces=(), compiler_options=None, includes = None, system_includes = None, libclang_location = None, parse_all_comments = False, target_file_only = False):
        """
           Parse the file at construction
           
           Parameters
           -----------

           filename : string
                      Name of the file to parse
           
           namespaces : list of string 
                      Restrict the generation to the given namespaces.
           
           includes : string, optional
                      Additional includes to add (-I xxx) for clang

           system_includes : string, optional
                      Additional System includes to add (-isystem xxx) for clang
           
           compiler_options : string, optional 
                      Additional option for clang compiler
           
           libclang_location : string, optional
                      Absolute path to libclang. By default, the detected one.

           parse_all_comments : bool
                      Grab all comments, including non doxygen like [default = False]

           target_file_only : bool
                      Neglect any included files during desc generation [default = False]
        """
        self.filename, self.namespaces, self.target_file_only = filename, namespaces, target_file_only
        self.root = CL.parse(filename, compiler_options, includes, system_includes, libclang_location, parse_all_comments, skip_function_bodies = False)

    # FIXME : pass the 3 functions to run functions ?? 
    # default : namespace, filelocation, ?
    # lambda ns : this_condition(ns) or ... easy to add...
    def keep_ns(self, n):
        if (target_file_only and n.location.file.name != self.filename): return False
        return len(self.namespaces) == 0 or n.spelling in self.namespaces
    
    def keep_cls(self, c):
        if (target_file_only and c.location.file.name != self.filename): return False
        return True
    
    def all_classes_gen(self):
        """Generates all the AST nodes of classes (mode = 'C') or functions (mode = 'F')"""
        return CL.get_classes(self.root, self.keep_cls, traverse_namespaces = True, keep_ns = self.keep_ns)

    def make_h5(self, cls):

        print CL.get_annotations(cls)
        """cls : AST node for a class. Returns the h5_read/write code"""
        # FIXME : treat template !
        h5w = '\n'.join('h5_write(gr, "%s", %s);'%(x.spelling, x.spelling) for x in CL.get_members(cls, False))
        h5 = """
void h5_write(triqs::h5::group h5group, std::string const & subgroup_name, {cls_name} const &x) {{
auto gr = h5group.create_group(subgroup_name);
{h5w}
}}

void h5_read(triqs::h5::group h5group, std::string const & subgroup_name,{cls_name} &x) {{
auto gr = h5group.open_group(subgroup_name);
{h5r}
}}
        """.format(cls_name = cls.spelling, h5w = h5w, h5r = h5w.replace('write', 'read'))
        ns = ':'.join(CL.get_namespace_list(cls))
        r = "namespace %s {{\n {H5} \n}}"%ns if ns else '' "{H5}"  
        return r.format(H5 = h5)

    def run(self, output_filename, verbose  = True):
        """ Makes the cxx file"""
        
        with open(output_filename, "w") as f:
            f.write('\n'.join(self.make_h5(cls) for cls in self.all_classes_gen()))
        os.system('clang-format -i ' + output_filename)


