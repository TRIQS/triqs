import os, re
import cpp2py.clang_parser as CL
from synopsis import make_synopsis_list, make_synopsis_template_decl
from processed_doc import replace_latex, clean_doc_string

# common tools for both rendering functions

rst_start = """
..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief

"""

# The hidden toc for RST reference
toctree_hidden ="""
.. toctree::
    :hidden:
      
"""

def make_header(s):
    """ Make a rst header from the string s """
    return '\n\n' + s + '\n' + '-'*len(s)  + '\n\n'

def escape_lg(s):
    """Escape the > and < in the string, which are special in rst"""
    return s.replace('>','\>').replace('<','\<')

def render_list(item_list, header):
    """ 
       Make rst code for a list of items with  a header
       It splits the first word in a separate column
    """
    if not item_list: return ''
    head = make_header(header) if header else ''
    l = [ (x+'   ').split(' ',1) for x in item_list]
    return head + '\n'.join(" * **%s**: %s\n"%(k.strip(),v) for (k,v) in l)

def render_note(s) :
    """ Make rst code for a note. Nothing if empty  """
    return """
.. note::
    %s"""%(s) if s else ''

def render_warning(s) :
    """ Make rst code for a warning. Nothing if empty  """
    return """
.. warning::
    %s"""%(s) if s else ''
   
#-------------------------------------

def render_table(list_of_list):
    """ Make rst code for a table, from a list of list
        Compute the lengths of fields, etc...
    """
    lcols = [len(x) for x in list_of_list[0]]
    for li in list_of_list : # compute the max length of the columns
        lcols = [ max(len(x), y) for x,y in zip(li, lcols)]
    form =  '| ' + " | ".join("{:<%s}"%x for x in lcols).strip() + ' |'
    sep = '+' + '+'.join((x+2) *'-' for x in lcols) + '+'
    r = [sep]
    for li in list_of_list: r += [form.format(*li), sep] 
    return '\n'.join(r) + '\n'

#-------------------------------------

def render_fig(figs): 
    fig = figs.split(":", 1)
    return """
.. figure:: {fig[0]}
   :alt: {fig[1]}
   :align: center
        """.format(fig = fig) + fig[1].lstrip(' \t\n\r') + '\n'

#-------------------------------------

def render_example(filename):
    """From the filename, prepare the doc1, doc2, before and after the code
       and compute the lineno of the code for inclusion"""
    decal = 4
    # Hard error ?
    if not os.path.exists(filename):
        #print "example file %s (in %s) does not exist"%(filename,os.getcwd())
        return ''
    ls = open(filename).read().strip().split('\n')
    # What is this ??
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
   
    if not code : return ''
    # TODO : Change triqs example ? Include ? code block ...
    return """
Example
---------

{d1}

.. triqs_example::

    linenos:{s},{e}

{code}

{d2}
        """.format(d1 = doc1, d2 = doc2,s = s, e = e, code = code)
 
#------------------------------------

def render_fnt(parent_class, f_name, f_overloads):
    """
       Generate the rst page for a function
        
        * f_name : name of the function
        * overload_docs : ??
        * parent_class : None or the parent class if it is a method
        * f_overloads : a list of WHAT ?
    """
    # Start of the page
    R = rst_start
    
    # tag and name of the class
    parent_cls_name_fully_qualified = (CL.fully_qualified_name(parent_class) + "::") if parent_class else ""
    parent_cls_spelling = (parent_class.spelling + '_') if parent_class else '' 
    f_name_full = parent_cls_name_fully_qualified + f_name
    
    R += """
.. _{class_rst_ref}:

{f_name_full}
""".format(f_name_full = f_name_full, class_rst_ref = parent_cls_spelling + f_name)

    R += '=' * (len(f_name_full)+0) + '\n' + """

"""
    # Synopsis
    R += make_synopsis_list(f_overloads) + '\n\n'
    
    # HOW DO WE GROUP THE OVERLOAD
    # Enumerate all overloads
    for n, f in enumerate(f_overloads):
        doc = f.processed_doc
        doc_elem = doc.elements

        num = '%s.'%(n+1) if len(f_overloads)>1 else ''
        R += '\n' + num + ' ' + doc.brief_doc + '\n'
        R += doc.doc

        # TODO which order ?
        if 'note' in doc_elem :     R += render_note(doc_elem.pop('note'))
        if 'warning' in doc_elem:   R += render_warning(doc_elem.pop('warning'))
        if 'figure' in doc_elem:    R += render_fig(doc_elem.pop('figure'))
        if 'tparam' in doc_elem:    R += render_list(doc_elem.pop('tparam'), 'Template parameters')
        if 'param' in doc_elem:     R += render_list(doc_elem.pop('param'), 'Parameters')
        if 'return' in doc_elem:    R += make_header('Return value')        + doc_elem.pop('return')

    # any example from the overloads
    # Should we TAKE ONLY ONE ????? Error ??
    example_file_name = reduce(lambda x,y : x or y, [ f.processed_doc.elements.pop('example', '') for f in f_overloads], '')  
    R += render_example(example_file_name)

    return R

#------------------------------------

# Why is all_methods, functoni passed but not members ???
# Why not do the work here ?

def render_cls(cls, all_methods, all_friend_functions):
    """
       Generate the rst page for a class
        
        * cls : node for the classf_name : name of the function
        * doc_methods : ??
    """
   
    cls_doc = cls.processed_doc
    doc_elem = cls_doc.elements
    R= rst_start
    
    # include
    incl = doc_elem.pop('include', '')

    # class header
    cls_name_fully_qualified = CL.fully_qualified_name(cls)
    R +="""
.. _{cls.spelling}:

{cls_name_fully_qualified}
{separator}

Defined in header <*{incl}*>

.. code-block:: c

    {templ_synop}class {cls.spelling};

{cls_doc.brief_doc}

{cls_doc.doc}
    """.format(cls = cls, incl = incl.strip(), separator = '=' * (len(cls_name_fully_qualified)), templ_synop = make_synopsis_template_decl(cls), cls_doc = cls_doc, cls_name_fully_qualified = cls_name_fully_qualified)

    # 
    R += cls_doc.doc
    if 'tparam' in doc_elem:    R += render_list(doc_elem.pop('tparam'), 'Template parameters')
    if 'note' in doc_elem :     R += render_note(doc_elem.pop('note'))
    if 'warning' in doc_elem:   R += render_warning(doc_elem.pop('warning'))
    if 'figure' in doc_elem:    R += render_fig(doc_elem.pop('figure'))

    # Members
    c_members = list(CL.get_members(cls, True)) 
    if len(c_members) > 0:
        R += make_header('Public members') 
        R += render_table([(t.spelling,t.type.spelling, replace_latex(clean_doc_string(t.raw_comment)) if t.raw_comment else '') for t in c_members])

    # Using : TODO : KEEP THIS ?
    c_usings = list(CL.get_usings(cls)) 
    for t in c_usings: 
        t.my_tag = t.spelling

    if len(c_usings) > 0:
        R += make_header('Member types') 
        R += render_table([(t.spelling, replace_latex(clean_doc_string(t.raw_comment)) if t.raw_comment else '') for t in c_usings])

    # A table for all member functions and all friend functions
    def group_of_overload(f_list): 
        s = set(f.processed_doc.elements['group'] for f in f_list if 'group' in f.processed_doc.elements)
        assert len(s) < 2, "Can not have different group for various overload"
        return s.pop if s else ''
   
    def make_func_list(all_f, header_name):
        R = ''
        if len(all_f) > 0:
            R += make_header(header_name)
            R += render_table([(":ref:`%s <%s_%s>`"%(name,escape_lg(cls.spelling), escape_lg(name)), f_list[0].processed_doc.brief_doc) for (name, f_list) in all_f.items() ])
            R += toctree_hidden
            for f_name in all_f:
               R += "    {cls.spelling}/{f_name}\n".format(cls = cls, f_name = f_name)
        return R

    R += make_func_list(all_methods, 'Member functions')
    R += make_func_list(all_friend_functions, 'Non Member functions')

    # Example
    if 'example' in doc_elem: R += render_example(doc_elem.pop('example'))
    
    return R
