.. highlight:: c

Overloading functions and methods for CLEF arguments
=====================================================

.. _overload_function:

Overloading functions 
----------------------------

Given a function, it is possible to overload it for CLEF expression arguments, returning a CLEF expression
using the `TRIQS_CLEF_MAKE_FNT_LAZY` macro.

**Synopsis** ::

 namespace triqs { namespace clef { 
 TRIQS_CLEF_MAKE_FNT_LAZY (function_to_make_lazy);
 }}

For example:

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>

    // a simple foo function 
    double foo(double x) { return x/2;}
    int foo(int x) { return x*2;}

    // a more complex case : bar is already a template
    // we have to disable it for CLEF expression to avoid ambiguity

    // C++14 clean syntax will be (using concepts)
    // template<NotClefExpression T>
    // T bar (T const & x) { return x+1;} 

    // C++11 workaround
    template<typename T> 
    typename std::enable_if<!triqs::clef::is_clef_expression<T>::value,T>::type 
    bar (T const & x) { return x+1;} 
 
    namespace triqs { namespace clef { TRIQS_CLEF_MAKE_FNT_LAZY (foo) ; TRIQS_CLEF_MAKE_FNT_LAZY (bar) ; }}

    int main() { 
      triqs::clef::placeholder<3> x_;
      std::cout << foo(2.0)<<" "<<eval(x_ + foo(x_), x_ = 3)<<" "<<eval(x_ + foo(x_), x_ = 3.5) << std::endl;  
      std::cout << bar(2.0)<<" "<<eval(x_ + bar(x_), x_ = 3)<<" "<<eval(x_ + bar(x_), x_ = 3.5) << std::endl;  
     }
  
Note that : 
 
* This overload **must** be defined in the triqs::clef namespace, since it is found by ADL.
* The function `foo` can have many overloads.
* The function `bar` can be a template, BUT then the template must be disabled for lazy expressions.
* The overload is already defined by clef for usual functions : 

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>
    int main() { 
      triqs::clef::placeholder<3> x_;
      std::cout << 2.0 + std::cos(2.0) << std::endl;  
      std::cout << eval( x_ + cos(x_), x_ = 2) << std::endl; // NB : note the absence of std::
    }
  
.. _callable_object:

Overloading operator() and other methods 
---------------------------------------------------


Similarly to functions, classes can define an `operator()` for CLEF expressions arguments (or any other method).
It is an ordinary operator() that must : 

* Be enabled only when one argument is a CLEF expression
* Return a CLEF expression.

Example : 

.. compileblock::
       
    #include <triqs/clef.hpp>
        
    struct Obj { 
    double v;                      // put something in it
    Obj(double v_): v(v_){}        // constructor
    Obj(Obj const &) = delete;    // a non copyable object, to illustrate that we do NOT copy...

    // The "normal", non CLEF call operator ....
    double operator() (double x) const { return 10*x;}

    // This macro implements properly an overload of the operator ()
    TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

    // a method
    double my_method(double x) const { return 2*x;}
   
    // CLEF overload
    // WARNING : the method MUST be const
    TRIQS_CLEF_IMPLEMENT_LAZY_METHOD(Obj,my_method);
     
    // Just to print itself nicely in the expressions
    friend std::ostream & operator<<(std::ostream & out, Obj const & x) { return out<<"Obj";}
    };

    int main() { 
    Obj f(7);
    triqs::clef::placeholder<1> x_; triqs::clef::placeholder<2> y_;

    std::cout << "Clef expression     : "<<  f(y_) + 2*x_ << std::endl ; 
    std::cout << "Complete evaluation : "<< eval(f(x_) + 2*x_, x_=1) << std::endl ; 
    std::cout << "Partial evaluation  : "<< eval(f(y_) + 2*x_, y_=1) << std::endl ; 
    std::cout << "Complete evalution  : "<< eval(f(y_) + 2*x_, x_=3, y_=1) << std::endl<<std::endl ; 

    std::cout << "Clef expression     : "<<  f.my_method(y_) + 2*x_ << std::endl ; 
    std::cout << "Complete evaluation : "<< eval(f.my_method(x_) + 2*x_, x_=1) << std::endl ; 
    std::cout << "Partial evaluation  : "<< eval(f.my_method(y_) + 2*x_, y_=1) << std::endl ; 
    std::cout << "Complete evalution  : "<< eval(f.my_method(y_) + 2*x_, x_=3, y_=1) << std::endl ; 
    }
 
**NB**  When the method or the non CLEF operator() is already a template, 
        it must be disabled for clef expression argument, using the trait ::

           clef::is_clef_expression<T...> // true iif one of the T is a clef expression

        as the `bar` function above.


