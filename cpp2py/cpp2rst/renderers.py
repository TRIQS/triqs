import os, re, itertools
import cpp2py.clang_parser as CL
from synopsis import make_synopsis_list, make_synopsis_template_decl, make_label, replace_ltgt, escape_lg
from processed_doc import replace_latex, clean_doc_string
from collections import OrderedDict
import global_vars

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

def make_header(s, char = '-'):
    """ Make a rst header from the string s """
    return '\n\n' + s + '\n' + char*len(s)  + '\n\n'

#------------------------------------

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

def render_list(item_list, header, char):
    """ 
       Make rst code for a list of items with  a header
       It splits the first word in a separate column
    """
    if not item_list: return ''
    head = make_header(header, char) if header else ''
    l = [ (x+'   ').split(' ',1) for x in item_list]
    return head + '\n'.join(" * **%s**: %s\n"%(k.strip(),v) for (k,v) in l)
   
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
    """Read the example file and prepare the rst"""
    if not filename : return ''
    filename = os.path.join(global_vars.examples_root, filename) 
    if not os.path.exists(filename):
        return """
.. error:: File not found 

    Example file %s not found"""%(filename)
    # Here we could eliminate the headers
    code = open(filename).read()
    sep = 4*' '
    code = '\n'.join(sep + x for x in code.strip().split('\n'))
    return """
Example
---------

.. code-block:: cpp

{code}

        """.format(code = code)
 
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
    parent_cls_name = (parent_class.name + '_') if parent_class else '' 
    f_name_full = parent_cls_name_fully_qualified + f_name
    
    R += """
.. _{class_rst_ref}:

{f_name_full}
""".format(f_name_full = f_name_full, class_rst_ref = make_label(parent_cls_name + f_name))

    R += '=' * (len(f_name_full)+0) + '\n' + """

"""
    # Synopsis
    R += make_synopsis_list(f_overloads) + '\n\n'
     
    # We regroup the overload as in cppreference
   
    def make_unique(topic) :
        rets = set(f.processed_doc.elements.pop(topic, '').strip() for f in f_overloads)
        rets = list(x for x in rets if x)
        if len(rets)> 1: print "Warning : Multiple documentation of %s across overloads. Picking first one"%topic
        return rets[0] if rets else ''
   
    def make_unique_list(topic) : 
        D = OrderedDict()
        for n, f in enumerate(f_overloads):
           plist = f.processed_doc.elements.pop(topic) # p is a string "NAME REST"
           for p in plist : 
               name, desc = (p + '   ').split(' ',1)
               if name not in D:
                   D[name] = desc.strip()
               else:
                   if D[name] != desc.strip() : 
                       print "Warning : multiple definition of parameter %s at overload %s"%(name, n)
        return D

    def render_dict(d, header, char):
        """ 
           Make rst code for a list of items with  a header
           It splits the first word in a separate column
        """
        if not d: return ''
        head = make_header(header, char) if header else ''
        return head + '\n'.join(" * **%s**: %s\n"%(k.strip(),v) for (k,v) in d.items())

    has_overload = len(f_overloads)> 1

    # Head doc 
    head = make_unique('head') or ("Documentation" if has_overload else '')
    R += '%s\n\n'%head 

    # The piece of doc which is overload dependent
    for num, f in enumerate(f_overloads):
        pd =  f.processed_doc
        num_s =  '**%s)**  '%(num+1) if has_overload else '' 
        if pd.doc : R +=  '\n\n %s %s\n '%(num_s,  pd.doc)+ '\n'

    # Tail doc 
    R += '%s\n\n' %make_unique('tail') 

    # Figure
    figure = make_unique('figure')
    if figure : R += render_fig(figure)

    # tparam and param
    tparam_dict = make_unique_list('tparam')
    param_dict = make_unique_list('param')
    
    R += render_dict(tparam_dict, 'Template parameters', '^')
    R += render_dict(param_dict, 'Parameters','^')
 
    # Returns 
    rets = make_unique("return")
    if rets : R += make_header('Returns', '^') + " * %s"%rets

    # Examples 
    example_file_name = make_unique("example")
    R += render_example(example_file_name.strip())

    # Warning 
    w = make_unique("warning")
    R += render_warning(w)

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
    fully_qualified_name = CL.fully_qualified_name(cls)
    R +="""
.. _{cls.name_for_label}:

{cls.fully_qualified_name}
{separator}

Defined in header <*{incl}*>

.. code-block:: c

    {templ_synop} class {cls.name}

{cls_doc.brief_doc}

{cls_doc.doc}
    """.format(cls = cls, incl = incl.strip(), separator = '=' * (len(cls.fully_qualified_name)), templ_synop = make_synopsis_template_decl(cls), cls_doc = cls_doc)

    # 
    R += cls_doc.doc
    if 'tparam' in doc_elem:    R += render_list(doc_elem.pop('tparam'), 'Template parameters', '-')
    if 'note' in doc_elem :     R += render_note(doc_elem.pop('note'))
    if 'warning' in doc_elem:   R += render_warning(doc_elem.pop('warning'))
    if 'figure' in doc_elem:    R += render_fig(doc_elem.pop('figure'))

    # Members
    if len(cls.members) > 0:
        R += make_header('Public members') 
        R += render_table([(t.spelling,t.type.spelling, replace_latex(clean_doc_string(t.raw_comment)) if t.raw_comment else '') for t in cls.members])

    # Usings 
    if len(cls.usings) > 0:
        R += make_header('Member types') 
        R += render_table([(t.spelling, re.sub(cls.namespace + '::','',t.underlying_typedef_type.spelling), 
                            replace_latex(clean_doc_string(t.raw_comment)) if t.raw_comment else '') for t in cls.usings])

    # A table for all member functions and all friend functions
    def group_of_overload(f_list): 
        s = set(f.processed_doc.elements['group'] for f in f_list if 'group' in f.processed_doc.elements)
        assert len(s) < 2, "Can not have different group for various overload"
        return s.pop if s else ''
   
    def make_func_list(all_f, header_name):
        if not all_f : return ''
        R = make_header(header_name)
        
        # Regroup the function by sub category
        D = OrderedDict()
        for name, flist in all_f.items():
            cat =flist[0].processed_doc.elements.get('category', None) 
            D.setdefault(cat, list()).append((":ref:`%s <%s>`"%(escape_lg(name),make_label(cls.name + '_' + name)), flist[0].processed_doc.brief_doc))
        
        # Make the sub lists
        for cat, list_table_args in D.items() : 
            if cat : R += make_header(cat, '~')
            R += render_table(list_table_args)
        
        # the hidden toctree is not regrouped
        R += toctree_hidden
        for f_name in all_f:
           R += "    {cls_name}/{f_name}\n".format(cls_name = replace_ltgt(cls.name), f_name = f_name)
        return R

    R += make_func_list(all_methods, 'Member functions')
    R += make_func_list(all_friend_functions, 'Non Member functions')

    # Example
    if 'example' in doc_elem: R += render_example(doc_elem.pop('example'))
    
    return R

#-------------------------------------

def render_ns(ns, all_functions, all_classes, all_usings): 
 
    R = make_header('Reference C++ API for %s'%ns, '#') 
    ns = ns.split('::',1)[-1]

    if len(all_usings) > 0:
        R += make_header('Type aliases')
        R += render_table([(t.spelling, re.sub(ns+'::','',t.underlying_typedef_type.spelling), 
                            replace_latex(clean_doc_string(t.raw_comment)) if t.raw_comment else '') for t in all_usings])

    if all_classes:
        R += make_header('Classes')
        R += ".. table::\n   :width: 50% 50%\n\n"
        #R += render_table([(":ref:`%s <_%s_%s>`"%(cls.spelling,escape_lg(ns), escape_lg(cls.spelling)), cls.processed_doc.brief_doc) for cls in all_classes ])
        R += render_table([(":ref:`%s <%s>`"%(escape_lg(cls.name), cls.name_for_label), cls.processed_doc.brief_doc) for cls in all_classes ])
        R += toctree_hidden
        for cls in all_classes:
            R += "    {ns}/{filename}\n".format(ns = ns, filename = replace_ltgt(cls.name))


    if all_functions:
        R += make_header('Functions')
        R += render_table([(":ref:`%s <%s>`"%(name, escape_lg(name)), f_list[0].processed_doc.brief_doc) for (name, f_list) in all_functions.items() ])
        #R += render_table([(":ref:`%s <%s_%s>`"%(name,escape_lg(ns), escape_lg(name)), f_list[0].processed_doc.brief_doc) for (name, f_list) in all_functions.items() ])
        R += toctree_hidden
        for f_name in all_functions:
           R += "    {ns}/{f_name}\n".format(ns = ns, f_name = f_name)
   
    return R


