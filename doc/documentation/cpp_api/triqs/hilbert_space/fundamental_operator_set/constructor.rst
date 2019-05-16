..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__fundamental_operator_set__constructor:

triqs::hilbert_space::fundamental_operator_set::constructor
===========================================================

*#include <triqs/hilbert_space/fundamental_operator_set.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`fundamental_operator_set` ()

    2. | :red:`fundamental_operator_set` (std::vector<int> const & :param:`v`)

    3. | :green:`template<typename IndexType>`
       | :red:`fundamental_operator_set` (std::set<IndexType> const & :param:`s`)

    4. | :red:`fundamental_operator_set` (fundamental_operator_set::data_t const & :param:`v`)

    5. | :red:`fundamental_operator_set` (gf_struct_t const & :param:`gf_struct`)

Documentation



 **1)**   Construct an empty set



 **2)**   Construct a set with each stored index being a pair of integers `(i,j)`



 **3)**   Construct from a set of generic index sequences



 **4)**   Construct from a vector of index sequences



 **5)**   Construct fundamental_operator_set on a GF structure





Parameters
^^^^^^^^^^

 * :param:`v` `i` runs from 0 to `v.size()-1`; `j` runs from 0 to `v[i].size()-1` for each `i`

 * :param:`s` Set of indices

 * :param:`gf_struct` GF structure object