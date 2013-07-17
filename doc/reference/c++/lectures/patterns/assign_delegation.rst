.. highlight:: c


Assignment Delegation
===============================

Problem
------------

Given a class A which defines an `operator =`, we would like to 
be able to implement various optimisation depending on the RHS (right hand side).

Normally, we can simply define various operator in class A ::

  class A { 
   // ...
   template<typename RHS> A & operator = (RHS rhs);
  }

maybe with some enabler to choose for different types.

Normally RHS are expected to model some precise concept (e.g. array, green function, etc...).
In other words, RHS are looking "like an A".
It is then easy to implement a generic version of operator = that uses just the concept.

We would like to be able to implement various optimisations depending on the RHS (right hand side).
This optimisation are decided are compile time and maybe also at run time.

Of course, we could split the definition of operator = into cases with enable_if.
But we want a non-intrusive solution, i.e. be able to add an optimisation 
for a new type of RHS, without modifying the class A.

Real life examples 
^^^^^^^^^^^^^^^^^^^^^^^^

* array, matrix, vector, with RHS a generic proto expression. For example in triqs::arrays ::

   M = inverse(M);   // (1)
   M = 2*inverse(M); // (2)

  inverse returns an inverse_lazy object. We would like that the inversion is done in place, *without 
  temporary* in this (1), but obviously not for a general expression like (2).

* Green function, for example in the case :: 

    gf = fourier (gt) ;
    
  The Fourier transformation could be done directly in the gf data, without a temporary, 
  if this becomes a bottleneck the code (?).

  The point here is also that we can first implement a generic algorithm. If we need, 
  we know that we will be able to optimise this temporary *without any change in the user code*.

Solution 
------------------

A simple solution is to delegate the assignement and simply use overload e.g. ::

 template <typename LHS, typename RHS> 
 void assign_delegation (LHS & lhs, RHS const & rhs) {
   // implement the generic algorithm
 }

 class A { 
   // ...
   template<typename RHS> A & operator = (RHS const & rhs) { assign_delegation(*this, rhs);}
  }

And then later when defining MyRHS_with_optimisation ::

   class MyRHS_with_optimisation { 
    // .... implement the object ...

    template <typename LHS> 
    friend void assign_delegation (LHS & lhs, MyRHS_with_optimisation const & rhs) {
      // implement the special algorithm
    }
   };

**NB** : Adding the function as a friend in the class has 2 advantages over defining it outside: 
  
   * It is friend, hence it can access the data of rhs.
   * It will be injected in the namespace where the class MyRHS_with_optimisation, and therefore it will be found by ADL.

Variant 
^^^^^^^^^^   

With the same technique, one can also delegate the compound assignments :  +=, -=, *=, /=. 

Let us label these operators by a char :A, S, M, D. We can then differentiate between the various
case by adding an additionnal dummy parameter of mpl::char_<X> type::

 template <typename LHS, typename RHS, char OP> 
  void compound_assign_delegation (LHS & lhs, RHS const & rhs, mpl::char_<OP>) {
    // implement the generic algorithm
  }

 class A { 
   
   template<typename RHS> A & operator += (RHS const & rhs) { 
   compound_assign_delegation(*this, rhs, mpl::char_<'A'>() );
   }
   
   // and so on...
  }

And then later when defining MyRHS_with_optimisation ::

   class MyRHS_with_optimisation { 
    // .... implement the object ...

    template <typename LHS> 
    friend void compound_assign_delegation (LHS & lhs, MyRHS_with_optimisation const & rhs,mpl::char_<'E'>) {
      // implement the special algorithm
    }
   };

**NB** :

* One can then overload some or all of the operations for a given class.

* mpl::char_<'A'> is a different **type** than mpl::char_<'S'>, so the standard overload mechanism of C++ will 
  choose the right function at compile time (the `mpl::char_` object is tiny, and will be optimized away).
  It better than passing a simple char and make a switch at run time (because there is no compile time choice in this case).

* One could a priori also add a char OP in the template parameter list, but then in the MyRHS_with_optimisation class, we would need 
  to specialize a function, which results in less readeable code, IMHO.

Examples
-----------

Used in triqs::arrays.

