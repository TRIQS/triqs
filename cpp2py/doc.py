# This module contains functions to process the documentation
# of classes, functions, from C++ to Python
# make_doc(x) is the general function, where x is a node.

import re
import cpp2py.clang_parser as CL
import util
from processed_doc import ProcessedDoc, replace_latex, clean_doc_string

def make_table(*list_of_list):
    """
    :param list_of_list: list of list of strings
    :returns: a valid rst table
    """
    lcols = [len(x) for x in list_of_list[0]]
    for li in list_of_list : # compute the max length of the columns
        lcols = [ max(len(x), y) for x,y in zip(li, lcols)]
    form =  '| ' + " | ".join("{:<%s}"%x for x in lcols).strip() + ' |'
    sep = '+' + '+'.join((x+2) *'-' for x in lcols) + '+'
    r = [sep]
    for li in list_of_list: r += [form.format(*li), sep] 
    return '\n'.join(r) + '\n'

def treat_member_list(member_list) : 
    class _m:
      def __init__(self, m):
          self.spelling, self.type = m.spelling, m.type
    
    member_list2 = []
    for m in member_list:
      mm = _m(m)
      mm.doc, doc_lines = "", [l.lstrip() for l in clean_doc_string(m.raw_comment).splitlines()]

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

# FIXME : Obsolete. This is used for generate an rst file which is redundant.
def doc_param_dict_format(member_list) :
    """
       member_list : list of the node of the members of a struct

       returns the rst table
    """
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
    lines = [form.format(m.spelling, m.ctype, m.initializer.replace("res.","") if m.initializer else '--', m.doc) + '\n' + sep2 for m in member_list2]
    #for x in lines : 
    #    print x
    r = '\n'.join(lines)
    return sep2 + '\n' + header +'\n' + sep1 + '\n' + r 

def decal(s, shift = 5):
    sep = shift * ' '
    return '\n'.join( (sep + x.strip()) for x in s.split('\n'))

def make_doc_function(node):
    """ Makes the doc of the node node"""
    pdoc = ProcessedDoc(node) # general treatment and analysis, common with cpp2rst

    # first case : it is a dict function
    # FIXME : to be improved, what do we want ?
    if util.use_parameter_class(node):
        member_list = CL.get_members(util.get_decl_param_class(node), True)
        table = doc_param_dict_format(member_list)
        return "%s\n\n%s\n\n%s\n"%( pdoc.brief_doc, pdoc.doc, table) 

        # member_list2 = treat_member_list(member_list)
        # h = ['Parameter Name','Type','Default', 'Documentation']
        # l = [(m.spelling,m.ctype, m.initializer.replace("res.",""), m.doc) for m in member_list2]
        # table =  make_table(h, *l)
        # doc = clean_doc_string(node.raw_comment) + '\n' + table
        # return doc
    
    # General case

    doc = "\n%s\n%s"%(pdoc.brief_doc, pdoc.doc)
    doc = doc.strip() + "\n"
   
    params = pdoc.elements.pop('param', None) # parameters of the function
    if params:
       doc += "\nParameters\n----------\n"
       for p in params:
           name, comment = (p + '  ').split(' ',1)
           doc += "%s \n%s\n\n"%(name, decal(comment))

    ret = pdoc.elements.pop('return', None)
    if ret:
        _type = '' # FIXME : deduce the type ?
        doc += "Returns\n-------\nout  %s\n%s\n\n"%(_type, decal(ret))
   
    return doc.strip()

def make_doc_class(node):

    # FIXME
    return make_doc_function(node)


def make_doc(node) : 
    return clean_doc_string(node.raw_comment)
