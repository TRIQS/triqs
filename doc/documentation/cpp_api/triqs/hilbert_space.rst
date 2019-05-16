triqs::hilbert_space
####################



Type aliases
------------

+--------------+-------------------------------------------------+------------------------------------------------------------------+
| indices_t    | std::vector<std::variant<int, std::string> >    |  The index type of an operator                                   |
+--------------+-------------------------------------------------+------------------------------------------------------------------+
| gf_struct_t  | std::vector<std::pair<std::string, indices_t> > |  Type type describing the structure of a Block Green's function  |
+--------------+-------------------------------------------------+------------------------------------------------------------------+
| fock_state_t | uint64_t                                        |  The coding of the fermionic Fock state: 64 bits word in binary. |
+--------------+-------------------------------------------------+------------------------------------------------------------------+


Classes
-------

.. table::
   :width: 50% 50%

+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`fundamental_operator_set <triqs__hilbert_space__fundamental_operator_set>`                             |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`hilbert_space <triqs__hilbert_space__hilbert_space>`                                                   |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`sub_hilbert_space <triqs__hilbert_space__sub_hilbert_space>`                                           |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`state <triqs__hilbert_space__state>`                                                                   |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`state\<HilbertSpace,ScalarType,true\> <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT>`   |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`state\<HilbertSpace,ScalarType,false\> <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_falseGT>` |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`imperative_operator <triqs__hilbert_space__imperative_operator>`                                       |  |
+--------------------------------------------------------------------------------------------------------------+--+
| :ref:`space_partition <triqs__hilbert_space__space_partition>`                                               |  |
+--------------------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    hilbert_space/fundamental_operator_set
    hilbert_space/hilbert_space
    hilbert_space/sub_hilbert_space
    hilbert_space/state
    hilbert_space/stateLTHilbertSpace,ScalarType,trueGT
    hilbert_space/stateLTHilbertSpace,ScalarType,falseGT
    hilbert_space/imperative_operator
    hilbert_space/space_partition


Functions
---------

+----------------------------------------------------------------+--+
| :ref:`make_zero_state <triqs__hilbert_space__make_zero_state>` |  |
+----------------------------------------------------------------+--+
| :ref:`project <triqs__hilbert_space__project>`                 |  |
+----------------------------------------------------------------+--+

.. toctree::
    :hidden:

    hilbert_space/make_zero_state
    hilbert_space/project