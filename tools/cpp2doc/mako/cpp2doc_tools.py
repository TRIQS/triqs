import re, os
from util import make_table

def decay(s) :
    for tok in ['const ', 'const&', '&&', '&'] :
        s = re.sub(tok,'',s)
    return s.strip()

def process_param_type(t):
    if t.name in class_list: # has a link
       d = decay(t.name)
       return t.name.replace(d,":ref:`%s <%s>`"%(d,d))
    else:
       return t.name

def process_rtype(t) :
    tname =  re.sub(r"\s*typename\s+std\d*::enable_if<(.*),(.*)>::type", r"requires(\1)\n \2 ", t.name)
    return tname

def make_synopsis_template_decl(tparams) :
    if not tparams: return ''
    targs = ', '.join("%s %s"%(pp[0],pp[1]) + (" = %s"%pp[2] if (len(pp)==3 and pp[2]) else '') for pp in tparams)
    return "template<%s>"%targs

def add_linebreaks(s, num_char=80):
    """ add linebreaks every num_char characters in string s (if possible i.e if whitespace)"""
    char_count=0
    final_s=''
    for w in s.split(' '):
     char_count += len(w)
     if char_count < num_char: final_s += w+' '
     else: final_s += '\n   '+w+' '; char_count=len(w)
    return final_s

def escape_lg(s):
    return s.replace('>','\>').replace('<','\<')

def make_synopsis(m, decal):
    #assert not m.tparams, "template functions "
    try :
        syn = m.doc_elements['synopsis']
        if syn : return [syn]
        s = " {name} ({args}) {const}"
        if not m.is_constructor :
          s = process_rtype(m.rtype) + s
        s = make_synopsis_template_decl(m.tparams) + "\n" + s
        # filter to remove enable_if dummies from the API
        def no_dummy (t,n) : 
            return not ( 'enable_if' in t.name and 'dummy' in n)
        args = ', '.join( ["%s %s"%(process_param_type(t),n) + (" = %s"%d if d else "") for t,n,d in m.params if no_dummy(t,n)])
        s = s.format(args = args, name = m.name.strip(), const = m.const)
        if getattr(m,'noexcept',False): s += ' noexcept' 
        r = [x.strip() for x in s.split('\n')]
        L= [x for x in r if x]
        L_lb = [add_linebreaks(x) for x in L]
        return L_lb
    except:
        return ["Error in building synopsis"]

def make_synopsis_list(m_list):
    if not m_list: return ''
    decal = 4
    signs = [ make_synopsis(x, decal) for x in m_list]
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


def prepare_example(filename, decal):
    """From the filename, prepare the doc1, doc2, before and after the code
       and compute the lineno of the code for inclusion"""
    filename += ".cpp"
    #if not os.path.exists(filename) : filename = '/
    if not os.path.exists(filename) :
        #print "example file %s (in %s) does not exist"%(filename,os.getcwd())
        return None, None, None, 0, 0
    ls = open(filename).read().strip().split('\n')
    r = [i for i, l in enumerate(ls) if not (re.match(r"^\s*/?\*",l) or re.match("^\s*//",l))]
    s, e = r[0],r[-1]+1
    assert r == range(s,e)
    def cls(w) :
        w = re.sub(r"^\s*/?\*\s?/?",'',w)
        w = re.sub(r"^\s*//\s?",'',w)
        return w
    doc1 = '\n'.join(cls(x) for x in ls[0:s])
    code = '\n'.join(decal*' ' + x.strip() for x in ls[s:e])
    doc2 = '\n'.join(cls(x) for x in ls[e:])
    return code, doc1, doc2, s, e
