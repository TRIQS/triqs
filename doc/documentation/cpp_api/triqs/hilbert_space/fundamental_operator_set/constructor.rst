..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _fundamental_operator_set_constructor:

triqs::hilbert_space::fundamental_operator_set::constructor
===========================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Construct an empty set`
       | :red:`fundamental_operator_set` ()

    2. | :cppbrief:`Construct a set with each stored index being a pair of integers `(i,j)``
       | :red:`fundamental_operator_set` (std::vector<int> const & :param:`v`)

    3. | :cppbrief:`Construct from a set of generic index sequences`
       | :green:`template<typename IndexType>`
       | :red:`fundamental_operator_set` (std::set<IndexType> const & :param:`s`)

    4. | :cppbrief:`Construct from a vector of index sequences`
       | :red:`fundamental_operator_set` (fundamental_operator_set::data_t const & :param:`v`)

    5. | :cppbrief:`Construct fundamental_operator_set on a GF structure`
       | :red:`fundamental_operator_set` (gf_struct_t const & :param:`gf_struct`)

Documentation





Parameters
^^^^^^^^^^

 * **v**: `i` runs from 0 to `v.size()-1`; `j` runs from 0 to `v[i].size()-1` for each `i`

 * **s**: Set of indices

 * **gf_struct**: GF structure object