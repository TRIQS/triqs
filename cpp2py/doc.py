# This module contains functions to process the documentation
# of classes, functions, from C++ to Python
# make_doc(x) is the general function, where x is a node.

import re
import cpp2py.clang_parser as CL
import util

def replace_latex(s, escape_slash=False):
    """replace 
       $XX X$  by :math:`XX X`
       $$X XX$$  by \n\n.. math:\n\t\tX XX\n\n..\n
       [[ XXX]]  by :ref:` XXX`
     
     """
    any_math_char = 'A-Za-z0-9{}\[\],;|\(\)=./\/+-_^\'' #any math character
    #matches all expressions starting and ending with any math char, with possibly whitespaces in between
    pattern_1 = '\$(['+any_math_char+']['+any_math_char+' ]*['+any_math_char+']+)\$'
    #matches any single math char
    pattern_2 = '\$(['+any_math_char+'])\$'
    #out of line formula
    text=re.sub('\$'+pattern_1+'\$', r'\n\n.. math::\n\t\t\1\n\n..\n', s)
    text=re.sub('\$'+pattern_2+'\$', r'\n\n.. math::\n\t\t\1\n\n..\n', text)
    #inline formula
    text=re.sub(pattern_1, r':math:`\1`', text)
    text=re.sub(pattern_2, r':math:`\1`', text)
    #to create a hyperlink
    text=re.sub('\[\[([A-Za-z0-9{}\(,\)=./\/+-_]+)\]\]', r':ref:`\1`', text)

    if escape_slash: text=text.encode('string_escape')
    return text

def make_table(head_list, list_of_list):
    """
    :param head_list: list of strings with table headers
    :param list_of_list: list of list of strings
    :returns: a valid rst table
    """
    l = len (head_list)
    lcols = [len(x) for x in head_list]
    for li in list_of_list : # compute the max length of the columns
        lcols = [ max(len(x), y) for x,y in zip(li, lcols)]
    form =  '| ' + " | ".join("{:<%s}"%x for x in lcols).strip() + ' |'
    header= form.format(*head_list)
    w = len(header)
    sep = '+' + '+'.join((x+2) *'-' for x in lcols) + '+'
    sep1 = sep.replace('-','=')
    r = [sep, header, sep1]
    for li in list_of_list: r += [form.format(*li), sep] 
    return '\n'.join(r) + '\n'

def process_doc(doc):
    if not doc : return ""
    for p in ["/\*","\*/","^\s*\*", "///", "//", r"\\brief"] : 
        doc = re.sub(p,"",doc,flags = re.MULTILINE)
    doc = doc.strip()
    doc = replace_latex(doc, True)
    return doc


def replace_cpp_keywords_by_py_keywords(s):
    """replace syntax 
       @param XXX blabla 
       by
       :param XXX: blabla
    """
    s=re.sub('@param ([A-Za-z0-9_]*) ',r':param \1: ', s)
    return s

def treat_member_list(member_list) : 
    class _m:
      def __init__(self, m):
          self.spelling, self.type = m.spelling, m.type
    
    member_list2 = []
    for m in member_list:
      mm = _m(m)
      mm.doc, doc_lines = "", [l.lstrip() for l in process_doc(m.raw_comment).splitlines()]

      #print m, doc_lines
      mm.initializer = CL.get_member_initializer(m)
      mm.ctype = m.type.spelling
      for l in doc_lines:
          if l.startswith('type:'):
             # Override 'Type' field
             mm.ctype = l[5:].lstrip()
             continue
          if l.startswith('default:'):
             # Override 'Default' field
             mm.initializer = l[8:].lstrip()
             continue
          mm.doc += l + ' '
      #print mm.spelling, mm.ctype, mm.initializer 
      member_list2.append(mm)
    return member_list2

def doc_format_param(member_list):
   member_list2 = treat_member_list(member_list)
   h = ['Parameter Name','Type','Default', 'Documentation']
   l = [(m.spelling,m.ctype, m.initializer, m.doc) for m in member_list2]
   return make_table(h, l)

def make_doc(node):
    """ process doc of node"""
    
    doc = process_doc(node.raw_comment)
    if util.use_parameter_class(node):
        doc = doc + '\n' + doc_format_param(CL.get_members(util.get_decl_param_class(node), True))
    
    return replace_latex(replace_cpp_keywords_by_py_keywords(doc))
    

def doc_param_dict_format(member_list) : 
    member_list2 = treat_member_list(member_list)
    h= ['Parameter Name', 'Type', 'Default', 'Documentation']
    n_lmax = max(len(h[0]), max(len(m.spelling) for m in member_list2))
    type_lmax = max(len(h[1]), max(len(m.ctype) for m in member_list2))
    opt_lmax = max(len(h[2]), max(len(m.initializer) for m in member_list2 if m.initializer))
    doc_lmax = max(len(h[3]), max(len(m.doc) for m in member_list2))
    form =  "| {:<%s} | {:<%s} | {:<%s} | {:<%s} |"%(n_lmax, type_lmax, opt_lmax, doc_lmax)
    header = form.format(*h)
    sep1 = '+' + '+'.join([ (x+2)*'=' for x in (n_lmax, type_lmax, opt_lmax, doc_lmax)]) + '+'
    sep2 = '+' + '+'.join([ (x+2)*'-' for x in (n_lmax, type_lmax, opt_lmax, doc_lmax)]) + '+'
    lines = [form.format(m.spelling, m.ctype, m.initializer if m.initializer else '--', m.doc) + '\n' + sep2 for m in member_list2]
    #for x in lines : 
    #    print x
    r = '\n'.join(lines)
    return sep2 + '\n' + header +'\n' + sep1 + '\n' + r 



