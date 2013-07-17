.. highlight:: c

Functional constructs :  map, fold, ...
========================================================


map
--------
* **Purpose** :

  map promotes any function into an `array function`, acting term by term.

* **Syntax** :

  If `f` is a function, or a function object (SEE : ref to function object lib)::
   
   f : ValueType1 --> ValueType2

  Then map(f) is a function::
   
    ReturnType map(f) ( ArgType const & A)
   
  where ArgType  models the :ref:`HasImmutableArrayInterface` concept

   * with value_type == ValueType1

  and ReturnType models the :ref:`HasImmutableArrayInterface` concept

   * with the same domain as ArgType
   * with value_type == ValueType2

* Examples::

   double f(int i) { return i*10;}
   int main() { 
    auto F = map(f);
    array<int,2> A(2,2); 
    array<double,2> B; 
    A() =2;
    B = F(A);
    // works also with expressions of course
    C = F( 2*A );
    C = C + 3* F(2*A); // ok that is just an example...
    }

* Some cases require explicit cast, e.g. for the standard abs function (defined in XXXX), or the compiler does not know which std::abs you are talking about ::

   auto Abs = map( static_cast< double (*)(double)> (std::abs) );
   
fold
-------------------------------------------------

* **Purpose** :
  fold implement the folding (or reduction) on the array.

* **Syntax** :

  If A is a type which models the :ref:`HasImmutableArrayInterface` concept
  (e.g. an array<ValueType1,R, Opt> , a matrix, a vector, an expression,  ...)

  and `f` is a function, or a function object (SEE : ref to function object lib)::
   
   ValueType2 f (A::value_type , ValueType2)

  Then fold (f , A, ValueType2 init = ValueType2() ) = f( f( f( ... f( a(0,1), a(0,0))))) 
   
.. warning::

  The order of traversal is the same as foreach.

* **Example** : sum is implemented like this ::

   template <class A>
   typename A::value_type sum(A const & a) { return fold ( std::plus<typename A::value_type>())  (a); }




