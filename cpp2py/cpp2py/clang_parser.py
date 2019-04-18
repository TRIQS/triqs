# This module defines the function parse that
# call libclang to parse a C++ file, and retrieve
# A few helper functions for libclang
import sys,re,os, itertools, platform
import clang.cindex
from mako.template import Template
from clang.cindex import CursorKind, LibclangError
import libclang_config

def pretty_print(x, keep= None, s='...') : 
   print x
   for y in x.get_children():
        if keep and not keep(y) : continue 
        print s, y.spelling
        print s, y.kind
        print s, [z.spelling for z in y.get_tokens()]
        pretty_print(y, keep, s + '...')

def decay(s) :
    for tok in ['const ', 'const&', '&&', '&'] :
        s = re.sub(tok,'',s)
    return s.strip()

def get_annotations(node):
    return [c.displayname for c in node.get_children() if c.kind == CursorKind.ANNOTATE_ATTR]

def get_tokens(node): 
    return [t.spelling for t in node.get_tokens()]

def get_type_alias_value(node) : 
    """ node is a using, get the rhs"""
    return list(node.get_tokens())[3].spelling

def extract_bracketed(tokens):
    r = []
    bracket_count = 0
    for t in tokens:
        if t == '<': bracket_count += 1
        else: bracket_count -= len(t) - len(t.lstrip('>'))
        r.append(t)
        if bracket_count == 0: return r

def is_deprecated(node):
    """Check if the node is deprecated"""
    tokens = get_tokens(node)
    return len(tokens)>3 and tokens[0] =='__attribute__' and tokens[3] =='deprecated'

def is_public(node): 
    return node.access_specifier == clang.cindex.AccessSpecifier.PUBLIC 

def jump_to_declaration(node):
    """
    Precondition : node is a parameter of a function/method, or a base class
    Return : a cursor on its declaration
    """
    tt = node.type.get_declaration()  # guess it is not a ref
    if not tt.location.file : tt = node.type.get_pointee().get_declaration() # it is a T &
    return tt

def keep_all(x) : return is_public(x) #True

#--------------------  function related ---------------------------------------------------------

def is_explicit(node) : # for a constructor
    return 'explicit' in get_tokens(node)
   
def is_noexcept(node): 
    return 'noexcept' in get_tokens(node)[-2:]

def get_method_qualification(node): 
    """
    Detects from the tokens the trailing const, const &, &, &&, etc ...
    It is just after a ) if it exists (a type can not end with a ) )
    """
    s = ' '.join(get_tokens(node))
    for pat in ["const\s*&*","&+", "noexcept"] :
        m = re.search(r"\) (%s)"%pat, s)
        if m: return m.group(1).strip()
    return ''

def get_template_params(node):
    """
    Precondition : node is a function/method
    returns a list of (typename, name, default) or (Type, name)
    returns [] is not a template
    """
    tparams = []
    def get_default(c) : 
        tokens = get_tokens(c)
        return ''.join(tokens[tokens.index('=') + 1:-1]) if '=' in tokens else None
          
    for c in node.get_children():
        if c.kind == CursorKind.TEMPLATE_TYPE_PARAMETER :
            tparams.append(("typename", c.spelling, get_default(c)))
        elif c.kind == CursorKind.TEMPLATE_NON_TYPE_PARAMETER:
            l = list(c.get_tokens())
            if l: 
               tparams.append((l[0].spelling, c.spelling, get_default(c)))
    return tparams

def is_template(node) : 
    return len(get_template_params(node))>0 # optimize ?

def get_params(node):
    """
    Precondition : node is a function/method
    Yields the node of the parameters of the function
    """
    for c in node.get_children():
        if c.kind == CursorKind.PARM_DECL : 
            yield c

def get_param_default_value(node):
    """
    Precondition : node is a parameter of a function/method
    returns the default value if present or None
    Defined at the rhs of an = sign.
    """
    #print [x.spelling for x in node.get_tokens()]
    sp = ''.join(x.spelling for x in node.get_tokens()).split('=')
    assert len(sp) == 1 or len(sp) ==2
    #print "    ---> default =",   None if len(sp)==1 else sp[1]
    return None if len(sp)==1 else sp[1]

#--------------------  class components ---------------------------------------------------------

def get_name_with_template_specialization(node):
    """
    node is a class
    returns the name, possibly added with the <..> of the specialisation
    """
    if not node.kind in (CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION) : return None
    tokens = get_tokens(node)
    name = node.spelling
    if tokens and tokens[0] == 'template':
       t = tokens[len(extract_bracketed(tokens[1:]))+3:]
       if t and t[0] == '<': name = name + ''.join(extract_bracketed(t))
    return name

def get_base_classes(node, keep = keep_all): 
    """
    node is a class
    yields the nodes to the public base class.
    """
    for c in node.get_children():
        if c.kind == CursorKind.CXX_BASE_SPECIFIER and keep(c): 
                yield jump_to_declaration(c)

def get_members(node, with_inherited, keep = keep_all): 
    """
    node is a class
    yields the nodes to the public members
    if with_inherited, yields the nodes to the members AND the members of the public base classes
    """
    for b in get_base_classes(node):
        for m in get_members(b, with_inherited, keep):
            yield m
    
    for c in node.get_children():
       if c.kind == CursorKind.FIELD_DECL and keep(c): 
            yield c

def get_member_initializer(node):
    """node is a field_decl, i.e. a member in a class. Gets the rhs of the = or None"""
    tokens = get_tokens(node)
    if '=' in tokens:
        end_idx = tokens.index(';') if ';' in tokens else len(tokens)
        initializer = ''.join(tokens[tokens.index('=') + 1:end_idx]).strip()
        # Other members can be used in initialization with 'this->member_name'
        return initializer.replace("this->", "res.")
    else:
        return ''

def is_constructor(cls, m):
    """ is m a constructor of cls"""
    return m.kind == CursorKind.CONSTRUCTOR or \
          (m.kind == CursorKind.FUNCTION_TEMPLATE and cls.spelling == m.spelling.split('<')[0])

def is_copy_or_move_constructor(cls, m) :
    """Assuming m is a constructor of cls, is it a copy or move constructor of cls"""
    p = list(get_params(m)) # all the parameters
    return len(p) == 1 and decay(p[0].type.get_canonical().spelling) ==  cls.type.get_canonical().spelling

def get_methods(node, with_inherited = True, keep = keep_all): 
    """
    node is a class
    yields the nodes to the members
    if with_inherited : also the inherited methods
    """
    for b in get_base_classes(node):
        for m in get_methods(b, with_inherited, keep):
            yield m

    for c in node.get_children():
        ok = c.kind == CursorKind.CXX_METHOD or (c.kind == CursorKind.FUNCTION_TEMPLATE and not is_constructor(node, c))
        if ok and keep(c):
           yield c

def get_constructors(cls, keep = keep_all, with_copy_and_move_constructors = False): 
    """
    cls is a class
    yields the clss to the public constructors
    """
    for c in cls.get_children():
        if not is_public(c) : continue
        if keep(c) and c.kind == CursorKind.CONSTRUCTOR or \
                (c.kind == CursorKind.FUNCTION_TEMPLATE and is_constructor(cls, c)):
            if with_copy_and_move_constructors or not(is_copy_or_move_constructor(cls, c)):
                    yield c

def get_usings(node, keep = keep_all): 
    """
    node is a class
    yields the nodes to the usings 
    """
    for c in node.get_children():
        if c.kind == CursorKind.TYPE_ALIAS_DECL and keep(c): 
            yield c

def get_friend_functions(node, keep = keep_all): 
    """
    node is a class
    yields the nodes to the friend functions 
    """
    for c in node.get_children():
        if c.kind == CursorKind.FRIEND_DECL and keep(c): 
            yield c.get_children().next()

#--------------------  print -----------------------------------

def make_signature_template_params(tparams):
    return "template<" + ', '.join(["%s %s = %s"%x if x[2] else "%s %s"%x[:2] for x in tparams]) + ">  " if tparams else ''

def print_fnt(f):
    s = "{name} ({args})" if is_constructor(f) else "{rtype} {name} ({args})"
    s = s.format(args = ', '.join( ["%s %s"%(t.name,n) + ("=%s"%d if d else "") for t,n,d in get_params(f)]), **self.__dict__)
    s= make_signature_template_params(get_template_params(f)) + s
    if f.is_static_method() : s = "static " + s
    return "%s\n%s\n"%(s.strip(), f.raw_comment)

def print_cls(c):
    s,s2 = "class {name}:\n  {doc}\n\n"%c.spelling,[]
    for m in get_members(c):
        s2 += ["%s %s"%(m.ctype,m.name)]
    for m in get_methods(c) :
       s2 += print_fnt(m).split('\n')
    for m in get_friend_functions(c):
       s2 += ("friend " + print_fnt(m)).split('\n')
    s2 = '\n'.join( [ "   " + l.strip() + '\n' for l in s2 if l.strip()])
    s= make_signature_template_params(get_template_params(c)) + s
    return s + s2

#--------------------  namespace components : class, function ---------------------------------------------------------

def get_enums(node, keep = keep_all, traverse_namespaces = False, keep_ns = keep_all):
    """
    node is a namespace or root.
    keep(m) : predicate
    keep_ns : predicate
    traverse_namespaces : traverse the namespaces, with keep_ns
    yields the classes/struct in node
    """
    for c in node.get_children():
        if traverse_namespaces and c.kind is CursorKind.NAMESPACE and keep_ns(c):
            for x in get_enums(c, keep, traverse_namespaces, keep_ns) :
                yield x
        else :
          if c and c.kind == CursorKind.ENUM_DECL and keep(c):
            yield c

_class_types = [CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION, CursorKind.CLASS_TEMPLATE]

def get_classes(node, keep = keep_all, traverse_namespaces = False, keep_ns = keep_all):
    """
    node is a namespace or root.
    keep(m) : predicate
    keep_ns : predicate
    traverse_namespaces : traverse the namespaces, with keep_ns
    yields the classes/struct in node
    """
    for c in node.get_children():
        if traverse_namespaces and c.kind is CursorKind.NAMESPACE and keep_ns(c):
            for x in get_classes(c, keep, traverse_namespaces, keep_ns) :
                yield x
        else :
          if c and c.kind in _class_types and keep(c):
            yield c

_fnt_types = [CursorKind.FUNCTION_DECL, CursorKind.FUNCTION_TEMPLATE]

def get_functions(node, keep = keep_all, traverse_namespaces = False, keep_ns = keep_all):
    """
    node is a namespace or root.
    yields the functions
    keep_ns : predicate
    traverse_namespaces : traverse the namespaces, with keep_ns
     """
    for c in node.get_children():
        if traverse_namespaces and c.kind is CursorKind.NAMESPACE and keep_ns(c):
           for x in get_functions(c, keep, traverse_namespaces, keep_ns) :
              yield x
        else :
          if c and c.kind in _fnt_types and keep(c):
            yield c

def fully_qualified(c):
    if c is None:
        return ''
    elif c.kind == CursorKind.TRANSLATION_UNIT:
        return ''
    else:
        res = fully_qualified(c.semantic_parent)
        if res != '':
            return res + '::' + c.spelling
    return c.spelling

def fully_qualified_name(c) :
    return fully_qualified(c.referenced)

def get_namespace_list(node):
    return fully_qualified(node.referenced).split('::')[:-1]

def get_namespace(node):
    return fully_qualified(node.referenced).rsplit('::',1)[0]
    
#--------------------  PARSE

def parse(filename, compiler_options, includes, libclang_location, parse_all_comments, skip_function_bodies = True):
    """
    filename           : name of the file to parse
    compiler_options   : options to pass to clang to compile the file 
    where_is_libclang  : location (absolute path) of libclang
    return : the root of the AST tree
    """
    # compiler options
    compiler_options = (compiler_options or []) 
    if platform.system() == 'Darwin': compiler_options.append("-stdlib=libc++") 
    if parse_all_comments : compiler_options.append("-fparse-all-comments")
    includes = set(includes)
    if "/usr/include" in includes: includes.remove("/usr/include") # /usr/include is implicitly included
    compiler_options += ['-isystem%s'%x for x in includes] + libclang_config.LIBCLANG_CXX_FLAGS

    # print compiler_options
    # Initialising libclang
    clang.cindex.Config.set_library_file(libclang_location or libclang_config.LIBCLANG_LOCATION)

    try:
        index = clang.cindex.Index.create()
    except LibclangError as err:
        print "ERROR creating libclang parser! Be sure to install libclang before installing c++2py."
        raise err
    
    # Parse the file
    assert os.path.exists(filename), " File %s does not exist "%filename
    print "Parsing the C++ file (may take a few seconds) ..."
    if skip_function_bodies:
        translation_unit = clang.cindex.TranslationUnit.from_source(filename, args =  ['-x', 'c++'] + compiler_options,
                                                                    options = clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
    else:
        translation_unit = index.parse(filename, ['-x', 'c++'] + compiler_options)

    # If clang encounters errors, we report and stop
    errors = [d for d in translation_unit.diagnostics if d.severity >= 3]
    if errors :
      s =  "Clang reports the following errors in parsing\n"
      for err in errors :
        loc = err.location
        s += '\n'.join([" file %s line %s col %s"%(loc.file, loc.line, loc.column), err.spelling])
      raise RuntimeError, s + "\n... Your code must compile before using clang-parser !"

    print "... done. \n"
    return translation_unit.cursor 

