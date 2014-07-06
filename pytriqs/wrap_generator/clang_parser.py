# This module defines the function parse that 
# call libclang to parse a C++ file, and retrieve
# from the clang AST the classes, functions, methods, members (including
# template).
# This module is use e..g by the wrapper desc generator.
import sys,re,os
import clang.cindex
import itertools
from mako.template import Template
import textwrap

def get_annotations(node):
    return [c.displayname for c in node.get_children()
            if c.kind == clang.cindex.CursorKind.ANNOTATE_ATTR]

def process_doc (doc) :
    if not doc : return ""
    for p in ["/\*","\*/","^\s*\*", "///", "//", r"\\brief"] : doc = re.sub(p,"",doc,flags = re.MULTILINE)
    return doc.strip()

file_locations = set(())

class member_(object):
    def __init__(self, cursor,ns=()):
        loc = cursor.location.file.name
        if loc : file_locations.add(loc)
        self.doc = process_doc(cursor.raw_comment)
        self.ns = ns
        self.name = cursor.spelling
        self.access = cursor.access_specifier
        self.ctype = cursor.type.spelling
    
    def namespace(self) : 
        return "::".join(self.ns)

class Function(object):
    def __init__(self, cursor, is_constructor = False, ns=() ): #, template_list  =()):
        loc = cursor.location.file.name
        if loc : file_locations.add(loc)
        self.doc = process_doc(cursor.raw_comment)
        self.brief_doc = self.doc.split('\n')[0].strip() # improve ...
        self.ns = ns
        self.name = cursor.spelling
        self.annotations = get_annotations(cursor)
        self.access = cursor.access_specifier
        self.params = []
        self.params_decay = []
        self.template_list = []  #template_list
        self.is_constructor = is_constructor
        self.is_static = cursor.is_static_method()

        def decay(s) : 
            s = re.sub('const','',s)
            s = re.sub('&&','',s)
            s = re.sub('&','',s)
            return s.strip()
        for c in cursor.get_children():
            if c.kind == clang.cindex.CursorKind.TEMPLATE_TYPE_PARAMETER : 
                 self.template_list.append(c.spelling)
            elif (c.kind == clang.cindex.CursorKind.PARM_DECL) :
                self.params.append ( (c.type.spelling, c.spelling))
                self.params_decay.append ( (decay(c.type.spelling), c.spelling))
            #else : 
            #    print " node in fun ", c.kind
        # self.rtype = cursor.result_type.get_canonical().spelling
        self.rtype = cursor.result_type.spelling if not is_constructor else None
        #print 'params for ', self.name,  self.params_decay

    def namespace(self) : 
        return "::".join(self.ns)

    def signature_cpp(self) :
        s = "{name} ({args})" if not self.is_constructor else "{rtype} {name} ({args})"
        s = s.format(args = ', '.join( ["%s %s"%t_n for t_n in self.params]), **self.__dict__) 
        if self.template_list : 
            s = "template<" + ', '.join(['typename ' + x for x in self.template_list]) + ">  " + s
        if self.is_static : s = "static " + s
        return s.strip()

    @property
    def is_template(self) : return len(self.template_list)>0

    def __str__(self) :
        return "%s\n%s\n"%(self.signature_cpp(),self.doc)

class Class(object):
    def __init__(self, cursor,ns):
        loc = cursor.location.file.name
        if loc : file_locations.add(loc)
        self.doc = process_doc(cursor.raw_comment)
        self.brief_doc = self.doc.split('\n')[0].strip() # improve ...
        self.ns = ns
        self.name = cursor.spelling
        self.functions = []
        self.constructors = []
        self.methods = []
        self.members = []
        self.proplist = []
        self.annotations = get_annotations(cursor)
        self.file = cursor.location.file.name

        # MISSING : constructors template not recognized 
        for c in cursor.get_children():
            # Only public nodes
            if c.access_specifier != clang.cindex.AccessSpecifier.PUBLIC : continue

            if (c.kind == clang.cindex.CursorKind.FIELD_DECL):
                m = member_(c)
                self.members.append(m)

            elif (c.kind == clang.cindex.CursorKind.CXX_METHOD):
                f = Function(c)
                self.methods.append(f)

            elif (c.kind == clang.cindex.CursorKind.CONSTRUCTOR):
                f = Function(c, is_constructor = True)
                self.constructors.append(f)

            elif (c.kind == clang.cindex.CursorKind.FUNCTION_DECL):
                f = Function(c)
                self.functions.append(f)
            
            elif (c.kind == clang.cindex.CursorKind.FUNCTION_TEMPLATE):
                f = Function(c)
                self.methods.append(f)

    def namespace(self) : 
        return "::".join(self.ns)
  
    def __str__(self) :
        s,s2 = "class {name}:\n  {doc}\n\n".format(**self.__dict__),[]
        for m in self.members :
            s2 += ["%s %s"%(m.ctype,m.name)]
        for m in self.methods : 
           s2 += str(m).split('\n')
        for m in self.functions : 
           s2 += ("friend " + str(m)).split('\n')
        s2 = '\n'.join( [ "   " + l.strip() + '\n' for l in s2 if l.strip()])
        return s + s2

def build_functions_and_classes(cursor, namespaces=[]):
    classes,functions = [],[]
    for c in cursor.get_children():
        if (c.kind == clang.cindex.CursorKind.FUNCTION_DECL
            and c.location.file.name == sys.argv[1]):
            functions.append( Function(c,namespaces))
        elif (c.kind in [clang.cindex.CursorKind.CLASS_DECL, clang.cindex.CursorKind.STRUCT_DECL]
            and c.location.file.name == sys.argv[1]):
            classes.append( Class(c,namespaces))
        elif c.kind == clang.cindex.CursorKind.NAMESPACE:
            child_fnt, child_classes = build_functions_and_classes(c, namespaces +[c.spelling])
            functions.extend(child_fnt)
            classes.extend(child_classes)

    return functions,classes

def parse(filename, debug, compiler_options, where_is_libclang): 

  compiler_options =  [ '-std=c++11', '-stdlib=libc++', '-D__CODE_GENERATOR__'] + compiler_options

  clang.cindex.Config.set_library_file(where_is_libclang)
  index = clang.cindex.Index.create()
  translation_unit = index.parse(filename, ['-x', 'c++'] + compiler_options)
  print "Parsing done. \nExtracting ..."
 
  # If clang encounters errors, we report and stop
  errors = [d for d in translation_unit.diagnostics if d.severity >= 3]
  if errors : 
      s =  "Clang reports the following errors in parsing\n"
      for err in errors :
        loc = err.location
        s += '\n'.join(["file %s line %s col %s"%(loc.file, loc.line, loc.column), err.spelling])
      raise RuntimeError, s + "\n... Your code must compile before making the wrapper !" 

  # Analyze the AST to extract classes and functions
  functions, classes = build_functions_and_classes(translation_unit.cursor)
  print "... done"

  global file_locations
  if len(file_locations) != 1 : 
      print file_locations
      raise RuntimeError, "Multiple file location not implemented"
  file_locations = list(file_locations)

  if debug : 
      print "functions"
      for f in functions : 
          print f
      
      print "classes"
      for c in classes : 
          print c

  return functions, classes

