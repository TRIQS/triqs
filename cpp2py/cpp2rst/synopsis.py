import re, os
import cpp2py.clang_parser as CL
import global_vars

def escape_lg(s):
    """Escape the > and < in the string, which are special in rst"""
    return s.replace('>','\>').replace('<','\<')


def replace_ltgt(name):
    """Make the changes in names suitable for using as label in RST : remove < > ..."""
    r = re.sub('<','LT', name)
    r = re.sub('>','GT', r)
    return r

def make_label(name):
    """Make the changes in names suitable for using as label in RST : remove < > ..."""
    r = re.sub('<','LT', name)
    r = re.sub('>','GT', r)
    r = re.sub('[\.]','_',r)
    r = re.sub('::','__', r)
    r = re.sub(',','_', r)
    return r

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

def process_type_name(type_node):
   
    decl = CL.jump_to_declaration(type_node)

    def decay(s) :
        for tok in ['const ', 'const&', '&&', '&'] :
            s = re.sub(tok,'',s)
        return s.strip()

    def clean_ns(s):
        for x in global_vars.process_type_name_ns_to_clean:
            s = re.sub(x, '', s)
        return s
            
    # Remove the namespace
    # replace const A & by A const & 
    t_name = clean_ns(type_node.spelling)
    t_name = re.sub(r'const ([^&]*) &', r'\1 const &', t_name)
 
    # try to see if decl is in the known list 
    class_list = global_vars.synopsis_class_list
    #d = CL.fully_qualified_name(decl)
    #l = [ cls for cls in class_list if cls.fully_qualified_name_no_template == d]
    #print "DECL",[cls.fully_qualified_name_no_template for cls in l]
    #print "DECL",  d, [l.fully_qualified_name_no_template]decl.spelling, CL.fully_qualified_name(decl) #, [CL.fully_qualified_name(x) for x in class_list]
    if decl in class_list:
    #if l:
        cls_idx = class_list.index(decl)
        cls = class_list[cls_idx]
        part_to_labelize = clean_ns(decay(t_name))
        label = cls.name_for_label 
        # First version is just the full name, with namespace
        # t_name =  t_name.replace(part_to_labelize,":ref:`%s`"%(label))
        t_name =  t_name.replace(part_to_labelize,":ref:`%s <%s>`"%(escape_lg(part_to_labelize),label)) 

    return t_name


def make_synopsis_one_function(f, number):
    """
        Given the AST node for a function f, returns the synopsis
        number : the number of the function (in a list of overloads)
    """
    ns = CL.get_namespace(f) + '::' # to remove the myclass:: from the types of arg and rtype 
    is_not_constructor = not getattr(f, 'is_constructor', False)
    
    template = make_synopsis_template_decl(f)
    
    result_type = (process_type_name(f.result_type) + ' ') if is_not_constructor else ''
    name = " %s "%f.spelling.strip() if is_not_constructor else f.spelling.split('<',1)[0] # eliminate the <> in the constructor name
    qualif = CL.get_method_qualification(f) + (' noexcept' if getattr(f,'noexcept',False) else '')
  
    params1 = [(p.type, p.spelling, CL.get_param_default_value(p)) for p in CL.get_params(f)]
    params = ["%s %s"%(process_type_name(t), ":param:`%s`"%n if n else '') + (" = %s"%d if d else "") for t,n,d in params1]
    # same with no rst decoration 
    params_no_role = ["%s %s"%(t.spelling, n) + (" = %s"%d if d else "") for t,n,d in params1]
 
    # first attempt : one line, else multiple line
    nspace = 8 if number>=10 else 7
    sep = nspace*' ' +  '| '
    sep2 = ',\n'  + sep + ' '*len(name) 
    res1 = sep + result_type + ":red:`%s` "%name.strip() + '(' 
    
    # First compute the result without any rst decoroation to compute the lengths
    res_no_role = res1 +  ', '.join(x for x in params_no_role)
    if len(res_no_role) > global_vars.synopsis_maxlen_function: # not good, need to split
        res = res1 + sep2.join(x for x in params) # DEBUG + "%s %s"%(len(res_no_role) , global_vars.synopsis_maxlen_function)
    else: 
        res  = res1 +  ', '.join(x for x in params)

    # brief = f.processed_doc.brief_doc
    #r = ('%s:cppbrief:`%s`\n'%(sep,brief) if brief else '') + ('%s:green:`%s`\n'%(sep,template) if template else '')  + res + ') ' + qualif
    r =  ('%s:green:`%s`\n'%(sep,template) if template else '')  + res + ') ' + qualif
    
    return r.strip()


def make_synopsis_list(f_list):
    if len(f_list) == 1 : # use same instruction as !=1 but simplify
        return '**Synopsis**\n\n .. rst-class:: cppsynopsis\n\n    ' + '\n\n    '.join("   %s"%(make_synopsis_one_function(f, n+1)) for n, f in enumerate(f_list))
    else :
        return '**Synopsis**\n\n .. rst-class:: cppsynopsis\n\n    ' + '\n\n    '.join("%s. %s"%(n+1,make_synopsis_one_function(f, n+1)) for n, f in enumerate(f_list))


