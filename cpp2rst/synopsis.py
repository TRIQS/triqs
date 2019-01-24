import re, os
import cpp2py.clang_parser as CL

def decay(s) :
    for tok in ['const ', 'const&', '&&', '&'] :
        s = re.sub(tok,'',s)
    return s.strip()

def add_linebreaks(s, num_char=80):
    """ add linebreaks every num_char characters in string s (if possible i.e if whitespace)"""
    char_count=0
    final_s=''
    for w in s.split(' '):
     char_count += len(w)
     if char_count < num_char: final_s += w+' '
     else: final_s += '\n   '+w+' '; char_count=len(w)
    return final_s

def process_param_type(t_name):
    if t_name in class_list_name: # has a link
       d = decay(t_name)
       return t_name.replace(d,":ref:`%s <%s>`"%(d,d))
    else:
       return t_name

def process_rtype(t_name):
    tname =  re.sub(r"\s*typename\s+std\d*::enable_if<(.*),(.*)>::type", r"requires(\1)\n \2 ", t_name)
    return tname

def make_synopsis_template_decl(node) :
    tparams = CL.get_template_params(node)
    if not tparams: return ''
    targs = ', '.join("%s %s"%(pp[0],pp[1]) + (" = %s"%pp[2] if (len(pp)==3 and pp[2]) else '') for pp in tparams)
    return "template<%s> "%targs

def make_synopsis(m, pdoc, decal):
    #assert not m.tparams, "template functions "
    #try :
    syn = pdoc.elements['synopsis']
    if syn : return [syn]
    s = " {name} ({args}) {qualif}"
    if hasattr(m, 'result_type') :
        s = process_rtype(m.result_type.spelling) + s
    #if not CL.is_constructor(m) :
    #  s = process_rtype(getattr(m, 'result_type', None)) + s
    s = make_synopsis_template_decl(m) + "\n" + decal*' ' +  s
    # filter to remove enable_if dummies from the API
    def no_dummy (t,n) :
        return not ( 'enable_if' in t and 'dummy' in n)
    params = [(p.type.spelling, p.spelling, CL.get_param_default_value(p)) for p in CL.get_params(m)]
    args = ', '.join( ["%s %s"%(process_param_type(t),n) + (" = %s"%d if d else "") for t,n,d in params if no_dummy(t,n)])
    s = s.format(args = args, name = m.spelling.strip(), qualif = CL.get_method_qualification(m))
    if getattr(m,'noexcept',False): s += ' noexcept'
    r = [x.strip() for x in s.split('\n')]
    L= [x for x in r if x]
    L_lb = [add_linebreaks(x) for x in L]
    return L_lb
    #except:
    #    return ["Error in building synopsis"]

def make_synopsis_list(m_list, doc_methods):
    if not m_list: return ''
    decal = 4
    signs = [ make_synopsis(x, d, decal) for x,d in zip(m_list,doc_methods)]
    m = max( max (len(l) for l in s) for s in signs)
    tab = ("\n" + decal*' ')
    form =  '{:<%s}    {:<%s}'%(m, 3)
    lines =  []
    for n,s in enumerate(signs) :
       for p,l in enumerate(s):
          if p==0 :
            lines += [ '',  form.format(l, "(%s)"%(n+1) if len(m_list)>1 else '' )]
          else :
            lines.append(form.format(l,''))
    return  decal*' ' + tab.join(lines[1:])


