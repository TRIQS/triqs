.. highlight:: c
 
clef::function class 
==========================

The class triqs::clef::function is similar to boost::function (std::function),
except that :

* it can be constructed from an expression and an ordered list of placeholders.
* it is `lazy-callable` and `lazy-assignable`

.. note::
  It stores the expression polymorphically, by erasing its type. (You can put various expressions into it, cf below).
  This might lead to some performance penalty in some case, even though tests do not show that at present...

Example of usage is given in :ref:`lazy_function_ex`. 
