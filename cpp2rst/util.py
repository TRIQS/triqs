# This module contains a few utilities
import re, os

def clean_end_and_while_char(s):
    return re.sub(re.compile(r"[ \t\r\f\v]+$",re.MULTILINE),'', s.strip())# clean end and white char 

def decay(s) :
    for tok in ['const ', 'const&', '&&', '&'] :
        s = re.sub(tok,'',s)
    s = s.replace('const_view', 'view') # DISCUSS THIS
    return s.strip()

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


