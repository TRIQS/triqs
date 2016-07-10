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

from clang.cindex import CursorKind

def get_annotations(node):
    return [c.displayname for c in node.get_children()
            if c.kind == CursorKind.ANNOTATE_ATTR]

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
        self.type = type_(cursor.type)
        self.ctype = cursor.type.spelling
        self.annotations = get_annotations(cursor)
        # the declaration split in small tokens
        tokens = [t.spelling for t in cursor.get_tokens()]
        self.initializer = None
        if '=' in tokens:
            self.initializer = ''.join(tokens[tokens.index('=') + 1:tokens.index(';')])

    def namespace(self) :
        return "::".join(self.ns)

class type_(object):
    def __init__(self, cursor):
        self.name, self.canonical_name = cursor.spelling, cursor.get_canonical().spelling

    def __repr__(self) :
        return self.name
        #return "type : %s %s\n"%(self.name, self.canonical_name)

class type2_(type_):
    def __init__(self, name):
        self.name, self.canonical_name = name, name

class type_alias_(object):
    def __init__(self, cursor):
        self.doc = process_doc(cursor.raw_comment)
        self.name = cursor.spelling
        self.value = list(cursor.get_tokens())[3].spelling

    def __repr__(self) :
        return "using %s = %s"%(self.name,self.value)
        #return "type : %s %s\n"%(self.name, self.canonical_name)

def extract_bracketed(tokens):
    r = []
    bracket_count = 0
    for t in tokens:
        if t == '<': bracket_count += 1
        else: bracket_count -= len(t) - len(t.lstrip('>'))
        r.append(t)
        if bracket_count == 0: return r

def make_signature_template_params(tparams):
    return "template<" + ', '.join(["%s %s = %s"%x if x[2] else "%s %s"%x[:2] for x in tparams]) + ">  " if tparams else ''

class Function(object):
    def __init__(self, cursor, is_constructor = False, ns=(), parent_class = None):
        loc = cursor.location.file.name
        if loc : file_locations.add(loc)
        self.file_location = loc
        self.doc = process_doc(cursor.raw_comment)
        self.brief_doc = self.doc.split('\n')[0].strip() # improve ...
        self.ns = ns
        self.name = cursor.spelling
        self.annotations = get_annotations(cursor)
        self.access = cursor.access_specifier
        self.params = [] # a list of tuple (type, name, default_value or None).
        self.tparams = []  #template_list
        self.is_constructor = is_constructor
        self.is_static = cursor.is_static_method()
        self.parameter_arg = None # If exists, it is the parameter class

        tokens = [t.spelling if t else '' for t in cursor.get_tokens()]

        # detect from the tokens if a constructor is explicit
        self.is_explicit =  'explicit' in tokens

        # detects no except
        self.is_noexcept = 'noexcept' in tokens[-2:]

        def rindex(l, x) :
            """Get the right index of x in the list l"""
            return len(l) - l[::-1].index(x) -1

        # detect from the tokens the trailing const, const &, &, &&, etc ...
        # it is just after a ) if it exists (a type can not end with a ) )
        def get_qualif(syn):
            for pat in ["const &*","&+", "noexcept"] :
                m = re.search(r"\) (%s)"%pat, syn)
                if m: return m.group(1).strip()
        self.const = get_qualif(' '.join(tokens)) or ''

        if parent_class and parent_class.name == self.name.split('<')[0] :
            self.is_constructor = True

        if self.is_constructor : self.name = parent_class.name

        for c in cursor.get_children():

            if c.kind == CursorKind.TEMPLATE_TYPE_PARAMETER :
                tokens = [t.spelling if t else '' for t in c.get_tokens()]
                d = ''.join(tokens[tokens.index('=') + 1:-1]) if '=' in tokens else None
                self.tparams.append(("typename", c.spelling, d))

            elif c.kind == CursorKind.TEMPLATE_NON_TYPE_PARAMETER:
                self.tparams.append((list(c.get_tokens())[0].spelling, c.spelling))

            elif (c.kind == CursorKind.PARM_DECL) :
                default_value = None
                for ch in c.get_children() :
                    # TODO : string literal do not work.. needs to use location ? useful ?
                    if ch.kind in [CursorKind.INTEGER_LITERAL, CursorKind.FLOATING_LITERAL,
                                   CursorKind.CHARACTER_LITERAL, CursorKind.STRING_LITERAL,
                                   CursorKind.UNARY_OPERATOR, CursorKind.UNEXPOSED_EXPR,
                                   CursorKind.CXX_BOOL_LITERAL_EXPR, CursorKind.CALL_EXPR ] :
                        #default_value =  ch.get_tokens().next().spelling
                        default_value =  ''.join([x.spelling for x in ch.get_tokens()][:-1])
                        # if the default value is a = A{} or = {}, need to clean
                        # the = in front of it (why ??).
                        default_value = default_value.strip(' =')
                t = type_(c.type)

                # We look if this argument is a parameter class...
                if 'use_parameter_class' in self.annotations :
                    tt = c.type.get_declaration()  # guess it is not a ref
                    if not tt.location.file : tt = c.type.get_pointee().get_declaration() # it is a T &
                    #if tt.raw_comment and 'triqs:is_parameter' in tt.raw_comment:
                    self.parameter_arg = Class(tt, ns)

                self.params.append ( (t, c.spelling, default_value ))
            #else:
            #    print "unknown kind ", c.kind, c.kind.is_attribute(), dir(c.kind)
 
        if self.parameter_arg : assert len(self.params) == 1, "When using a parameter class, it must have exactly one argument"
        self.rtype = type_(cursor.result_type) if not is_constructor else None
        #print self.rtype, parent_class.name if parent_class else ''
        #if parent_class and self.rtype and self.rtype.name.replace('&','').replace('const','') == parent_class.name :
        #    self.rtype = self.rtype.split('<')[0]

    def namespace(self) :
        return "::".join(self.ns)

    def signature_cpp2(self) :
        s = "{name} ({args})" if self.is_constructor else "{rtype} {name} ({args})"
        s = s.format(args = ', '.join( ["%s %s"%(t.name,n) + ("=%s"%d if d else "") for t,n,d in self.params]), **self.__dict__)
        s= make_signature_template_params(self.tparams) + s
        if self.is_static : s = "static " + s
        return s.strip()

    @property
    def is_template(self) : return len(self.tparams)>0

    def __str__(self) :
        return "%s\n%s\n"%(self.signature_cpp2(),self.doc)


class FriendFunction(Function):
    """WORKAROUND for defect of libclang, which does not contain the friend node"""
    def __init__(self, cursor,  ns=(), parent_class = None):
        loc = cursor.location.file.name
        if loc : file_locations.add(loc)
        self.file_location = loc
        self.doc = process_doc(cursor.raw_comment)
        self.brief_doc = self.doc.split('\n')[0].strip() # improve ...
        self.ns = ns
        self.const = ''
        self.name = cursor.spelling
        self.annotations = get_annotations(cursor)
        self.access = cursor.access_specifier
        self.params = [] # a list of tuple (type, name, default_value or None).
        self.tparams = []  #template_list
        self.is_constructor = False
        self.is_static = False
        self.parameter_arg = None

        tokens = [t.spelling if t else '' for t in cursor.get_tokens()]
        # If the friend function is a template, must analyse the template part
        if tokens[0] == "template":
            template_part = extract_bracketed(tokens[1:])
            tokens = tokens[len(template_part)+1:]
            # Parse template params
            self.tparams = [ (x.strip().split(' ') + [None]) for x in " ".join(template_part[1:-1]).split(',')]

        open_par, close_par = tokens.index('('),tokens.index(')')
        l = tokens[1:open_par]
        self.rtype = type2_(' '.join(l[:-1]))
        self.name = l[-1]
        args_list = tokens[open_par+1:close_par]
        # separate the arguments
        def f(tmp) :
            if '=' in tmp :
               ty, var, defaut = tmp[:-3], tmp[-3], tmp[-1]
            else:
               ty, var, defaut = tmp[:-1], tmp[-1], None
            self.params.append((type2_(''.join([(' ' if x in ['const','&'] else '') + x for x in ty])),var,defaut))

        tmp = []
        for x in args_list :
            if x == ',' and tmp.count('<') == tmp.count('>'):
                f(tmp)
                tmp = []
            else :
                tmp.append(x)
        f(tmp)

def is_deprecated(c):
    """Check if the node is deprecated"""
    tokens = [t.spelling if t else '' for t in c.get_tokens()]
    return len(tokens)>3 and tokens[0] =='__attribute__' and tokens[3] =='deprecated'

class Class(object):
    def __init__(self, cursor,ns):
        loc = cursor.location.file.name
        if loc : file_locations.add(loc)
        self.file_location = loc
        self.doc = process_doc(cursor.raw_comment)
        self.brief_doc = self.doc.split('\n')[0].strip() # improve ...
        self.ns = ns
        self.tparams = []
        self.public_base = []
        self.name = cursor.spelling
        self.functions = []
        self.constructors = []
        self.methods = []
        self.friend_functions = []
        self.members = []
        #self.proplist = []
        self.type_alias = []
        self.annotations = get_annotations(cursor)
        self.file = cursor.location.file.name

        tokens = [t.spelling if t else '' for t in cursor.get_tokens()]

        # If the class is a specialization
        if cursor.kind in (CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION):
            if tokens and tokens[0] == 'template':
               t = tokens[len(extract_bracketed(tokens[1:]))+3:]
               if t and t[0] == '<': self.name = self.name + ''.join(extract_bracketed(t))

        print "Analysing", self.name

        for c in cursor.get_children():

            # Need to analyse this FIRST, their access is not public strangely
            # so they do not pass the next tests.
            if c.kind == CursorKind.TEMPLATE_TYPE_PARAMETER:
                tokens = [t.spelling if t else '' for t in c.get_tokens()]
                d = ''.join(tokens[tokens.index('=') + 1:-1]) if '=' in tokens else None
                self.tparams.append(("typename", c.spelling, d))

            if c.kind == CursorKind.TEMPLATE_NON_TYPE_PARAMETER:
                tokens = [t.spelling if t else '' for t in c.get_tokens()]
                d = ''.join(tokens[tokens.index('=') + 1:-1]) if '=' in tokens else None
                self.tparams.append((tokens[0], c.spelling, d))

            # Skip private, undocumented or deprecated nodes
            if c.access_specifier != clang.cindex.AccessSpecifier.PUBLIC : continue
            if not c.raw_comment : continue
            if is_deprecated(c) : continue

            if (c.kind == CursorKind.CXX_BASE_SPECIFIER):
               tokens = [t.spelling if t else '' for t in c.get_tokens()]
               tt = c.type.get_declaration()  # guess it is not a ref
               if not tt.location.file : tt = c.type.get_pointee().get_declaration() # it is a T &
               self.public_base.append( Class(tt, ns))

            elif (c.kind == CursorKind.FIELD_DECL):
                m = member_(c)
                self.members.append(m)

            elif (c.kind == CursorKind.CXX_METHOD):
                f = Function(c, parent_class = self)
                self.methods.append(f)

            elif (c.kind == CursorKind.CONSTRUCTOR):
                f = Function(c, is_constructor = True, parent_class  = self)
                self.constructors.append(f)

            elif (c.kind == CursorKind.FUNCTION_DECL):
                f = Function(c)
                self.functions.append(f)

            elif (c.kind == CursorKind.FUNCTION_TEMPLATE):
                f = Function(c, parent_class = self)
                if f.is_constructor :
                    self.constructors.append(f)
                else :
                    self.methods.append(f)

            elif c.kind == CursorKind.TYPE_ALIAS_DECL:
                self.type_alias.append(type_alias_(c))
                # do not capture properly the rhs, but is it useful ?
                #print [(x.spelling, x.kind) for x in c.get_children()]
                #print [t.spelling if t else '' for t in c.get_tokens()]
                #print type_alias_(c)

            #elif c.kind == CursorKind.TYPEDEF_DECL:
            #    print [x.spelling for x in c.get_children()]
            #    self.type_alias.append(type_alias_(c))

            elif c.kind == CursorKind.UNEXPOSED_DECL:
                tokens =  [t.spelling if t else '' for t in c.get_tokens()]
                if tokens[0] == "template": tokens = tokens[len(extract_bracketed(tokens[1:]))+1:]
                if tokens[0] == "friend" and tokens[1] not in ['struct', 'class'] :
                    self.friend_functions.append(FriendFunction(c, parent_class = self))

            elif c.kind == CursorKind.CXX_ACCESS_SPEC_DECL:
                pass

            else :
                print "unknown in class ", c.spelling, repr(c.kind)

    @property
    def is_template(self) : return len(self.tparams)>0

    def namespace(self) :
        return "::".join(self.ns)

    def canonical_name(self) : return self.namespace() + '::' + self.name

    def __str__(self) :
        s,s2 = "class {name}:\n  {doc}\n\n".format(**self.__dict__),[]
        for m in self.members :
            s2 += ["%s %s"%(m.ctype,m.name)]
        for m in self.methods :
           s2 += str(m).split('\n')
        for m in self.functions :
           s2 += ("friend " + str(m)).split('\n')
        s2 = '\n'.join( [ "   " + l.strip() + '\n' for l in s2 if l.strip()])
        s= make_signature_template_params(self.tparams) + s
        return s + s2

    def __repr__(self) :
        return "Class %s"%self.name

def build_functions_and_classes(cursor, analyze_filter, namespaces=[]):
    classes,functions = [],[]
    for c in cursor.get_children():
        if (c.kind in [CursorKind.FUNCTION_DECL, CursorKind.FUNCTION_TEMPLATE] and analyze_filter(c, namespaces)):
          functions.append( Function(c,is_constructor = False, ns =namespaces))
        elif (c.kind in [CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL,
              CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
              CursorKind.CLASS_TEMPLATE] and analyze_filter(c, namespaces)):
            classes.append( Class(c,namespaces))
        elif c.kind == CursorKind.NAMESPACE:
            child_fnt, child_classes = build_functions_and_classes(c, analyze_filter, namespaces +[c.spelling])
            functions.extend(child_fnt)
            classes.extend(child_classes)
        else:
            pass
            #print "unknown", c.kind, c.spelling

    return functions,classes

def parse(filename, debug, compiler_options, where_is_libclang, analyze_filter):

  compiler_options =  [ '-std=c++11', '-stdlib=libc++'] + compiler_options

  clang.cindex.Config.set_library_file(where_is_libclang)
  index = clang.cindex.Index.create()
  print "Parsing the C++ file (may take a few seconds) ..."
  #print filename, ['-x', 'c++'] + compiler_options
  translation_unit = index.parse(filename, ['-x', 'c++'] + compiler_options)
  print "... done. \nExtracting ..."

  # If clang encounters errors, we report and stop
  errors = [d for d in translation_unit.diagnostics if d.severity >= 3]
  if errors :
      s =  "Clang reports the following errors in parsing\n"
      for err in errors :
        loc = err.location
        s += '\n'.join([" file %s line %s col %s"%(loc.file, loc.line, loc.column), err.spelling])
      raise RuntimeError, s + "\n... Your code must compile before using clang-parser !"

  # Analyze the AST to extract classes and functions
  functions, classes = build_functions_and_classes(translation_unit.cursor, analyze_filter)
  print "... done"

  global file_locations
  #if len(file_locations) != 1 :
  #    print file_locations
  #    raise RuntimeError, "Multiple file location not implemented"
  file_locations = list(file_locations)

  if debug :
      print "functions"
      #for f in functions :
      #    print f

      print "classes"
      for c in classes :
          print c

  return functions, classes

