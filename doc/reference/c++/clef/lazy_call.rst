
.. highlight:: c

.. _callable_object:

Overloading functions and callable objects for clef expression arguments 
=============================================================================

Given a callable object or a function, it is possible to overload it for clef expression arguments, returning a clef expression.
In all  cases:

   *Overloads that are activated iif at least one argument is a clef expression*.

Functions
------------

std math functions
^^^^^^^^^^^^^^^^^^^^^^^^

The overload is defined by clef automatically for usual functions : 

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>
    int main() { 
      triqs::clef::placeholder<3> x_;
      std::cout << 2.0 + std::cos(2.0) << std::endl;  
      std::cout << eval( x_ + cos(x_), x_ = 2) << std::endl; // NB : note the absence of std::
    }
  
Overloading a function for clef expressions arguments
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The `TRIQS_CLEF_MAKE_FNT_LAZY` macro defines the overload for clef expressions arguments of a function. Synopsis is ::

 namespace triqs { namespace clef { 
 TRIQS_CLEF_MAKE_FNT_LAZY (function_to_make_lazy);
 }}

For example:

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>
 
    double foo(double x) { return x/2;}
    int foo(int x) { return x*2;}

    namespace triqs { namespace clef { TRIQS_CLEF_MAKE_FNT_LAZY (foo) ; }}

    int main() { 
      triqs::clef::placeholder<3> x_;
      std::cout << foo(2.0) << std::endl;  
      std::cout << eval( x_ + foo(x_), x_ = 3) << std::endl;  
      std::cout << eval( x_ + foo(x_), x_ = 3.5) << std::endl;  
    }
  
Note that : 
 
 * This overload **must** be defined in the triqs::clef namespace, since it is found by ADL.
 * The function `foo` can have many overloads.
 * The function foo can be a template, BUT then the template must be disabled for lazy expressions, as in ::

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>
       
    template<typename T> 
    typename std::enable_if<!triqs::clef::is_any_lazy<T>::value,T>::type 
    foo (T const & x) { return x+1;}
      
    namespace triqs { namespace clef { TRIQS_CLEF_MAKE_FNT_LAZY (foo) ; }}

    int main() { 
      triqs::clef::placeholder<3> x_;
      std::cout << foo(2.0) << std::endl;  
      std::cout << eval( x_ + foo(x_), x_ = 3) << std::endl;  
      std::cout << eval( x_ + foo(x_), x_ = 3.5) << std::endl;  
    }

Callable objects
--------------------

Similarly to functions, classes can define an `operator()` for lazy expressions arguments.
It is an ordinary operator() that must : 

* Be enabled only when one argument is a clef expression

* Return a clef expression.

The function make_expr_call helps.

Object stored by copy (default)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The default behaviour is to store the object by copy in the expression tree...

.. compileblock::

    #include <triqs/clef.hpp>

    struct Obj { 
    double v;                      // put something in it
    Obj(double v_): v(v_){}        // constructor

    // The "normal", non lazy call operator ....
    double operator() (double x) const { return 10*x;}

    // Call operator for const object
    // make_expr_call creates a clef expression with a "call" node at the top
    // calling this object on the arguments.
    // Obj is stored by making a copy in the expression tree.
    // NB : the make_expr_call trait enabled only if one of the arguments is lazy
    // hence if none of the Args are clef expression, the template is disabled by SFINAE.
    template< typename... Args>
    typename triqs::clef::result_of::make_expr_call<Obj,Args...>::type
    operator()( Args&&... args ) const { return make_expr_call(* this,args...);}

    // Just to print itself nicely in the expressions
    friend std::ostream & operator<<(std::ostream & out, Obj const & x) { return out<<"Obj";}
    };

    int main() { 
    Obj f(7);
    triqs::clef::placeholder<3> x_;
    triqs::clef::placeholder<4> y_;
    std::cout << "Clef expression     : "<<  f(y_) + 2*x_ << std::endl ; 
    std::cout << "Complete evaluation : "<< eval(f(x_) + 2*x_, x_=1) << std::endl ; 
    std::cout << "Partial evaluation  : "<< eval(f(y_) + 2*x_, y_=1) << std::endl ; 
    std::cout << "Complete evalution  : "<< eval(f(y_) + 2*x_, x_=3, y_=1) << std::endl ; 
    }
 
Object stored by reference 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes, one want to escape the copy of the object in the tree.
In that case, just use a std::reference_wrapper in place of the object.
You have then to guarantee that the object will live longer that the expression 
that you form.

.. compileblock::

   #include <triqs/clef.hpp>

   struct Obj { 
   double v;                       // put something in it
   Obj(double v_): v(v_){}        // constructor
   Obj(Obj const &) = delete;    // a non copyable object 

   // The "normal", non lazy call operator ....
   double operator() (double x) const { return 10*x;}

   // Same as above, but now we stored a ref to the object and not a copy of the object.
   // Reference are encapsulated in std::reference_wrapper (Cf C++ documentation).
   template< typename... Args>
   typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<const Obj>,Args...>::type
   operator()( Args&&... args ) const { return make_expr_call (std::ref(* this),args...);}

   // The non const version (which then stores a non-const reference ....)
   template< typename... Args>
   typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<Obj>,Args...>::type
   operator()( Args&&... args ) { return  make_expr_call (std::ref(* this),args...);}

   // Just to print itself nicely in the expressions
   friend std::ostream & operator<<(std::ostream & out, Obj const & x) { return out<<"Obj";}
   };

   int main() { 
   Obj f(7);
   triqs::clef::placeholder<3> x_;
   triqs::clef::placeholder<4> y_;
   std::cout << "Clef expression     : "<<  f(y_) + 2*x_ << std::endl ; 
   std::cout << "Complete evaluation : "<< eval(f(x_) + 2*x_, x_=1) << std::endl ; 
   std::cout << "Partial evaluation  : "<< eval(f(y_) + 2*x_, y_=1) << std::endl ; 
   std::cout << "Complete evalution  : "<< eval(f(y_) + 2*x_, x_=3, y_=1) << std::endl ; 
   }

.. note : one can of course replace the reference_wrapper by a view of the object, e.g....


NB: When non lazy operator() is already a template ?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Then it must be disabled for clef expression argument, using the trait ::

   clef::is_any_lazy<T...> :: value // true iif one of the T is a clef expression

Example, derived from the previous one : 

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>

    struct Obj { 
    double v;                       // put something in it
    Obj(double v_): v(v_){}        // constructor

    // The "normal", non lazy call operator is now a template 
    template <typename T>
    typename std::enable_if<!triqs::clef::is_any_lazy<T>::value, T>::type   // correct 
    // T operator() (T const & x) const { return 10*x;}  would be ambiguous !
    operator() (T const & x) const { return 10*x;}

    // lazy expression
    template< typename... Args>
    typename triqs::clef::result_of::make_expr_call<Obj,Args...>::type
    operator()( Args&&... args ) const { return make_expr_call(* this,args...);}

    // Just to print itself nicely in the expressions
    friend std::ostream & operator<<(std::ostream & out, Obj const & x) { return out<<"Obj";}
    };

    int main() { 
    Obj f(7); 
    triqs::clef::placeholder<3> x_;
    triqs::clef::placeholder<4> y_;
    std::cout << "Clef expression     : "<<  f(y_) + 2*x_ << std::endl ; 
    std::cout << "Partial evaluation  : "<< eval(f(y_) + 2*x_, y_=1) << std::endl ; 
    std::cout << "Complete evalution  : "<< eval(f(y_) + 2*x_, x_=3, y_=1) << std::endl ; 
    }


