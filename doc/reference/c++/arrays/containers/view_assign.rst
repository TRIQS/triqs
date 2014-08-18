.. highlight:: c

.. _arr_view_assign:

Assignment to views
=========================

**Synopsis** : 

    template<typename RHS> array_view & operator=(RHS const & X);

*NB: The assignment operator is deleted for const_views.*

The `view classes` have a quite general assignment operator.
We will illustrate it on the `array_view` class, it is the same for `matrix` and `vector`.


* **RHS** can be : 

  * Anything that models the :ref:`ImmutableCuboidArray` concept 
  
      e.g. : array, array_view, matrix, matrix_view, 
      but also formal expression (See , e.g. A+B), or any custom object of your choice.

      *Effect* :  
        Every elements viewed by the view are replaced by the evaluation of RHS.
      
      *Precondition*
       
        The shape of the view and of RHS must match exactly or behaviour is undefined.

        If the debug macro, :ref:`TRIQS_ARRAYS_ENFORCE_BOUNDCHECK<arr_debug_macro>` is defined, 
        this condition is checked at runtime.

        NB : We could lower this condition, since we don't need a domain here, just the evaluation on the indices...

  * A scalar.

      *Effect* : 
        Every elements viewed by the view are set to this scalar, except for the matrix_view,
        where the matrix is set to the identity.
     
     
NB : no move assignment operator
---------------------------------------

Note that **there is no move assignment operators for views**.
If RHS is an rvalue reference, the regular operator= (view const &) is called, that 
makes a copy of the data of RHS into the elements viewed by the view.

This behaviour is consistent with the fact that views are *not* regular types.
For example ::

  array<double,2> A(3,3);
  A(range(0,2), range(0,2)) = any_function_returning_a_new_2x2_array(....);

In this case, the expected behaviour is that the part of A views by view at l.h.s
is filled by the result of the function. There cannot be any move semantics here.

As a result, std::swap algorithm does not work properly for view, hence it has
been explicitely *deleted*. A swap function (found by ADL) is provided instead.
