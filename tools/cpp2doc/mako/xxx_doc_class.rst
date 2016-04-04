<%
 import cpp2doc_tools as tools
 from cpp2doc_tools import make_table
 from util import replace_latex
 tools.class_list = class_list
 incl = c.doc_elements['include']
 if c.doc_elements['figure']:  fig=c.doc_elements['figure'].split(":")
%>
..
   Generated automatically using the command:
   ${shell_command}

.. highlight:: c

.. _${c.name}:

%if incl:
.. code-block:: c

    #include <${incl}>
%endif

${c.name}
${'=' * (len(c.name)+2)}

**Synopsis**:

.. code-block:: c

     ${tools.make_synopsis_template_decl(c.tparams)} class ${c.name};

${c.processed_doc}

%if c.doc_elements['figure']:
.. figure:: ${fig[0]}
   :alt: ${fig[1]}
   :align: center

   ${fig[1].lstrip(' \t\n\r')}
%endif

%if c.doc_elements['note']:
.. note::
     ${c.doc_elements['note']}
%endif

%if c.doc_elements['warning']:
.. note::
     ${c.doc_elements['warning']}
%endif

%if len(c.doc_elements['tparam']) > 0 :

Template parameters
----------------------

%for p,doc in c.doc_elements['tparam'].items():
   * **${p}**: ${doc}
%endfor
%endif

%if len(c.members) > 0:
Public members
-----------------

${make_table(['Member','Type','Comment'], [(t.name,t.ctype, t.doc) for t in c.members])}
%endif

%if len(c.type_alias) > 0:
Member types
-----------------

${make_table(['Member type','Comment'], [(t.name, t.doc) for t in c.type_alias])}
%endif

%if len(c.all_m) > 0:
Member functions
-----------------

${make_table(['Member function','Comment'], [(":ref:`%s <%s_%s>`"%(name,c.name, name), replace_latex(m[0].brief_doc)) for name, m in c.all_m.items()])}

.. toctree::
  :hidden:

%for m_name in c.all_m:
   ${c.name}/${m_name}
%endfor
%endif

%if len(c.all_friend_functions) > 0:
Non Member functions
-----------------------

${make_table(['Non member function','Comment'],
           [(":ref:`%s <%s_%s>`"%(name,c.name, name), m[0].brief_doc) for name, m in c.all_friend_functions.items()]) }

.. toctree::
  :hidden:

%for m_name in c.all_friend_functions:
   ${c.name}/${m_name}
%endfor
%endif

<%
  code,d1,d2, s,e = tools.prepare_example(c.name, 4)
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

