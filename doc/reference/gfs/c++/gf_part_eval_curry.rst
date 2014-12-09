.. highlight:: c

.. _gf_eval_curry: 

Partial evaluation and Currying 
================================

.. warning::

   This part of the library is still in alpha, in development.
 
Multivariable Green function are be both partially evalued, and "curried".

Partial evaluation 
--------------------------

**Purpose** :

  Having a function (x,y) -> f(x,y), fix x = x0 in order to obtain a function of y only.

**Synopsis** ::

  template<int ... pos, typename Opt, typename Target, bool B, bool IsConst, typename IT, typename ... Ms>
  gf_view< sliced_domain_t<cartesian_product<Ms...>>  ,Target, Opt, IsConst>
   partial_eval(gf_impl< cartesian_product<Ms...>, Target,Opt,B,IsConst> const & g, IT index) { 
 
Probably an example would help here ... :

.. triqs_example:: ./gf_part_eval_curry_0.cpp
Currying
-------------

**Purpose** :

  Having a function (x,y) -> f(x,y), rewrite it as x-> y-> f(x,y)

**Synopsis** ::

  template <int... pos, typename Target, typename Opt, bool IsConst, typename... Ms>
  gf_view<cart_prod<triqs::tuple::filter_t<std::tuple<Ms...>, pos...>>,
                lambda_valued<curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target, Opt,IsConst>, pos...>>, Opt, IsConst>
  curry(gf_view<cartesian_product<Ms...>, Target, Opt, IsConst> g);
 
 
.. triqs_example:: ./gf_part_eval_curry_1.cpp
