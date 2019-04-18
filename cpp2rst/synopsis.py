import re, os
import cpp2py.clang_parser as CL

def decay(s) :
    for tok in ['const ', 'const&', '&&', '&'] :
        s = re.sub(tok,'',s)
    return s.strip()

def make_synopsis_template_decl(node):
    """
        Given a node (class, function, method),
        extract its template parameters
        and return the string template<...>
    """
    tparams = CL.get_template_params(node)
    if not tparams: return ''
    targs = ', '.join("%s %s"%(pp[0],pp[1]) + (" = %s"%pp[2] if (len(pp)==3 and pp[2]) else '') for pp in tparams)
    return "template<%s>"%targs

shift = 4
maxlen = 90

# def reindent(s, shift):
    # if shift >= 0 : return '\n'.join(shift*' ' + x for x in s.split('\n'))
    # return '\n'.join(x[shift:] for x in s.split('\n'))


def process_param_type(t_name, remove):
    t_name = re.sub(remove, '', t_name)
    # replace const A & by A const &
    t_name = re.sub(r'const ([^&]*) &', r'\1 const &', t_name)
    d = decay(t_name)
    print "class_list_name", t_name, class_list_name
    if d in class_list_name: # has a link
       return t_name.replace(d,":ref:`%s <%s>`"%(d,d))
    else:
       return t_name

def process_rtype(t_name, remove):
    t_name = re.sub(remove, '', t_name)
    tname =  re.sub(r"\s*typename\s+std\d*::enable_if<(.*),(.*)>::type", r"requires(\1)\n \2 ", t_name)
    return tname

def make_synopsis_one_function(f, number):
    """
        Given the AST node for a function f, returns the synopsis
    """
    # If @synopsis was given manually
    #syn = f.processed_doc.elements.pop('synopsis', '')
    #if syn : return [syn]
 
    ns = CL.get_namespace(f) + '::' # to remove the myclass:: from the types of arg and rtype
    is_not_constructor = not getattr(f, 'is_constructor', False)
    
    template = make_synopsis_template_decl(f)
    
    result_type = (process_rtype(f.result_type.spelling, remove = ns) + ' ') if is_not_constructor else ''
    name = " %s "%f.spelling.strip() if is_not_constructor else f.spelling.split('<',1)[0] # eliminate the <> in the constructor name
    qualif = CL.get_method_qualification(f) + (' noexcept' if getattr(f,'noexcept',False) else '')
  
    #for p in CL.get_params(f):
    #    print p.type.get_canonical().spelling

    params1 = [(p.type.spelling, p.spelling, CL.get_param_default_value(p)) for p in CL.get_params(f)]
    params = ["%s %s"%(process_param_type(t, remove = ns), ":param:`%s`"%n if n else '') + (" = %s"%d if d else "") for t,n,d in params1]
  
    # first attempt : one line
    nspace = 8 if number>=10 else 7
    sep = nspace*' ' +  '| '
    sep2 = ',\n'  + sep + '  '
    res1 = sep + result_type + ":red:`%s` "%name.strip() + '('
    res  = res1 +  ', '.join(x for x in params)
    if len(res) > maxlen: # not good, need to split
        res = res1 + sep2.join(x for x in params)
    
    brief = f.processed_doc.brief_doc
    r = ('%s:cppbrief:`%s`\n'%(sep,brief) if brief else '') + ('%s:green:`%s`\n'%(sep,template) if template else '')  + res + ') ' + qualif
    return r.strip()


def make_synopsis_list(f_list):
    #if len(f_list) == 1 :
    #     return ' **Synopsis**\n\n .. rst-class:: cppsynopsis\n\n    ' + '\n\n    %s'%make_synopsis_one_function(f_list[0], 0)
    # overloads
    return ' **Synopsis**\n\n .. rst-class:: cppsynopsis\n\n    ' + '\n\n    '.join("%s. %s"%(n+1,make_synopsis_one_function(f, n+1)) for n, f in enumerate(f_list))


# re.findall(r'([^><\s]+)',s)

