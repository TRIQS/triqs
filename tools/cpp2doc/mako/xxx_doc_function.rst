<%
 import cpp2doc_tools as tools
 tools.class_list = class_list
 f = f_overloads[0]
 incl = f_overloads[0].doc_elements['include'] 
%>
..
   Generated automatically using the command :
   ${shell_command}
   ${f_overloads[0].file_location}

.. highlight:: c

%if c is not None :
.. _${c.name}_${f_name}:
%endif
%if c is None:
.. code-block:: c

    #include <${f_overloads[0].file_location}>
%endif

${f_name}
${'=' * (len(f_name)+2)}

**Synopsis**:

.. code-block:: c

${tools.make_synopsis_list(f_overloads)}

%if len(f_overloads) >1 :
%for n,m in enumerate(f_overloads) :
(${n+1})  ${m.processed_doc}
%endfor
%else:
${f_overloads[0].processed_doc}
%endif

Parameters
-------------

%for p,doc in f.doc_elements['param'].items():
* **${p}**: ${doc}
%endfor

Return value
--------------

${f.doc_elements['return']}

<% 
  code,d1,d2, s,e = tools.prepare_example(f_name, 4)
%>

%if f.doc_elements['note']:
.. note::
     ${f.doc_elements['note']}
%endif
%if f.doc_elements['warning']:
.. warning::
     ${f.doc_elements['warning']}
%endif


%if code is not None:

Example
---------

${d1}

.. triqs_example::

    linenos:${s},${e}

${code}

${d2}    

%endif

