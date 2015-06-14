# DO NOT EDIT
# Generated automatically using the command : 
# ${shell_command}

%for c in classes :
<%
  def doc_format(member_list) : 
   h= ['Parameter Name', 'Type', 'Default', 'Documentation']
   n_lmax = max(len(h[0]), max(len(m.name) for m in member_list))
   type_lmax = max(len(h[1]), max(len(m.ctype) for m in member_list))
   opt_lmax = max(len(h[2]), max(len(m.initializer) for m in member_list if m.initializer))
   doc_lmax = max(len(h[3]), max(len(m.doc) for m in member_list))
   form =  "| {:<%s} | {:<%s} | {:<%s} | {:<%s}|"%(n_lmax, type_lmax, opt_lmax, doc_lmax)
   header = form.format(*h)
   lines = [form.format(m.name, m.ctype, m.initializer if m.initializer else '--', m.doc) for m in member_list]
   r = '\n'.join(lines)
   sep = '-'* len(lines[0]) + '\n'
   return sep + header +'\n' + sep + r + '\n' + sep
%>

${doc_format(c.members)}

%endfor
