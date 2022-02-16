.. highlight:: c

.. _gf_call:

Operator()
==================================

**Synopsis** ::

  value_type const & operator()(mesh_t::domain_t::point_t) const  (1)

  view_type          operator()() const                           (2)

  ClefExpression  operator()(ClefExpression) const                (3)


The operator () is in charge of evaluating the Green function on its domain,
by any means (e.g. interpolation) that the specialization define.

For example, g(t) for a Green function in time would return some interpolation of
the value of g between the closest mesh point.
Hence, this operator is only const: it would make no sense to modify g(t).
(to modify the Green function, cf operator []).

However, the Clef automatic assignment method (cf...) *is* written
with the () operator, in order have a better syntax for multivariable function.
It would be more logical to use [], but a strange, old and extremely inconvenient limitation of C++
forbids to overload [] for multiple variables...


NB: This is valid for both the container (e.g. gf), and the view (e.g. gf_view).


.. _gf_element_access:

(1) Evaluation
---------------------------------

Evaluation at one point of the domain.

NB: Specialization may provide other overloads.

.. _gf_making_view:

(2) Building a view
---------------------------------


The special case (2) (no argument) returns a complete view of the object
(equivalent to view_type(* this)).


.. _gf_lazy:

(3) Interaction with clef expressions
-------------------------------------------------

* gf and gf_view can be used with the nda::clef library





