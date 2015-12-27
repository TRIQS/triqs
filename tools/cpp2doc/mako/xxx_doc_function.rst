<%
 import cpp2doc_tools as tools
 tools.class_list = class_list
 f = f_overloads[0]
 incl = f_overloads[0].doc_elements['include'] 
 if f.doc_elements['figure']:  fig=f.doc_elements['figure'].split(":")
%>
..
   Generated automatically using the command :
   ${shell_command}
   ${f_overloads[0].file_location}

.. highlight:: c

%if c is None:
.. code-block:: c

    #include <${f_overloads[0].file_location}>
%endif

%if c is not None :
.. _${c.name}_${f_name}:
%else:
.. _${f_name}:
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

%if f.doc_elements['figure']:
.. figure:: ${fig[0]}
   :alt: ${fig[1]}
   :align: center

   ${fig[1].lstrip(' \t\n\r')}
%endif

%if sum([len(m.doc_elements['tparam']) for n,m in enumerate(f_overloads)]) > 0 :

Template parameters
-----------------------

%if len(f_overloads) >1 :
%for n,m in enumerate(f_overloads) :

 (${n+1}) 

% if len(m.doc_elements['tparam'])>1:
% for p,doc in m.doc_elements['tparam'].items():

   * **${p}**: ${doc}
%endfor
% elif len(m.doc_elements['tparam'])==1:
% for p,doc in m.doc_elements['tparam'].items():
   **${p}**: ${doc}
%endfor
%else :
   not documented
%endif

%endfor
%else:
%for p,doc in f.doc_elements['tparam'].items():
* **${p}**: ${doc}
%endfor
%endif
%endif
Parameters
-------------

%if len(f_overloads) >1 :
%for n,m in enumerate(f_overloads) :

 (${n+1}) 

% for p,doc in m.doc_elements['param'].items():

   * **${p}**: ${doc}


%endfor
%endfor
%else:
%for p,doc in f.doc_elements['param'].items():
* **${p}**: ${doc}
%endfor
%endif

Return value
--------------

%if f.doc_elements['return']:
${f.doc_elements['return']}
%endif

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

