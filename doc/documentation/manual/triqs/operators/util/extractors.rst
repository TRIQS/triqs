
Coefficient extraction
======================

Functions in this module allow easy extraction of monomial coefficients from
``many_body_operator_generic`` objects. Each of `extract_*()` functions selects
a particular class of monimials (quadratic/quartic) and returns a dictionary,
which maps tuples of monomial indices to the values of the corresponding
coefficients.

Utility function
:ref:`dict_to_matrix() <dict_to_matrix>`
converts such dictionaries into a matrix/tensor representation (interaction
`U`-matrix, for instance) given a valid
:ref:`fundamental operator set <fundamental_operator_set>`.

C++
---

In order to make code cleaner, the following shorthand types are defined
in `triqs/operators/util/extractors.hpp`,

.. code-block:: c

    // Shorthand for many_body_operator_generic
    template<typename scalar_t>
    using op_t = operators::many_body_operator_generic<scalar_t>;

    // Mapping: index pair -> coefficient of type T
    template<typename T>
    using dict2_t = std::map<std::tuple<indices_t,indices_t>,T>;

    // Mapping: index quadruple -> coefficient of type T
    template<typename T>
    using dict4_t = std::map<std::tuple<indices_t,indices_t,indices_t,indices_t>,T>;

Here `indices_t` represents an operator index sequence
(see :ref:`fundamental_operator_set`).

**Coefficient extractors**

.. toctree::
    :maxdepth: 1

    /cpp2doc_generated/triqs/operators/util/extract_h_dict
    /cpp2doc_generated/triqs/operators/util/extract_U_dict2
    /cpp2doc_generated/triqs/operators/util/extract_U_dict4

**Dictionary to matrix/tensor conversion**

.. toctree::
    :maxdepth: 1

    /cpp2doc_generated/triqs/operators/util/dict_to_matrix

Python
------

.. automodule:: triqs.operators.util.extractors
    :noindex:
   :members:
    :member-order: bysource
