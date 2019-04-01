..
   Generated automatically by cpp2rst

.. highlight:: c


.. _fundamental_operator_set_constructor:

constructor
===========

**Synopsis**:

.. code-block:: c

    void fundamental_operator_set ()                                                                              (1)

    void fundamental_operator_set (const std::vector<int> & v)                                                    (2)

    template<typename IndexType>                                                                                  (3)
    void fundamental_operator_set (const std::set<IndexType> & s)

    void fundamental_operator_set (const
   triqs::hilbert_space::fundamental_operator_set::reduction_t & v)     (4)

    void fundamental_operator_set (const triqs::hilbert_space::gf_struct_t & gf_struct)                           (5)

(1)Construct an empty set


(2)Construct a set with each stored index being a pair of integers `(i,j)`


Parameters
----------

 * **v**: `i` runs from 0 to `v.size()-1`; `j` runs from 0 to `v[i].size()-1` for each `i`


Documentation
-------------




(3)Construct from a set of generic index sequences


Parameters
----------

 * **s**: Set of indices


Documentation
-------------




(4)Construct from a vector of index sequences


Parameters
----------

 * **v**: Vector of indices


Documentation
-------------




(5)Construct fundamental_operator_set on a GF structure


Parameters
----------

 * **gf_struct**: GF structure object


Documentation
-------------