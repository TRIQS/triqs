<%
 import cpp2doc_tools as tools
 tools.class_list = class_list
 f = f_overloads[0]
%>
..
   Generated automatically using the command :
   ${shell_command}
   ${f_overloads[0].file_location}

.. highlight:: c


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

%for n,m in enumerate(f_overloads) :

..  ---------------------------------------

%if c is None :

.. code-block:: c

%if 'include' in m.doc_elements:
    #include <${m.doc_elements['include'] | trim}>

%else:
    #include <${m.file_location}>

%endif
..
%endif

<%
num = '(%s)'%(n+1) if len(f_overloads)>1 else ''
%>
${num} ${m.brief_doc}
%if m.doc_elements['note']:

.. note::
     ${m.doc_elements['note']}
%endif
%if m.doc_elements['warning']:

.. warning::
     ${m.doc_elements['warning']}
%endif

%if m.doc_elements['figure']:
<%
  fig=m.doc_elements['figure'].split(":")
%>
.. figure:: ${fig[0]}
   :alt: ${fig[1]}
   :align: center

   ${fig[1].lstrip(' \t\n\r')}
%endif

%if len(m.doc_elements['tparam'])  > 0 :

Template parameters
-----------------------


% if len(m.doc_elements['tparam'])>1:
% for p,doc in m.doc_elements['tparam'].items():

   * **${p}**: ${doc}
%endfor
%else :
   not documented
%endif

%endif

%if len(m.doc_elements['param'])  > 0 :
Parameters
-------------

% for p,doc in m.doc_elements['param'].items():

   * **${p}**: ${doc}


%endfor

%endif

%if m.doc_elements['return']:
Return value
--------------

${m.doc_elements['return']}
%endif

%if m.processed_doc: 
Documentation
--------------
${m.processed_doc}
%endif

%endfor

<%
  # any example from the overloads
  example_file_name = reduce(lambda x,y : x or y, [ m.doc_elements['example'] for m in f_overloads] + [f_name], '')  
  code,d1,d2, s,e = tools.prepare_example(example_file_name, 4)
%>



%if code is not None:

Example
---------

${d1}

.. triqs_example::

    linenos:${s},${e}

${code}

${d2}

%endif

