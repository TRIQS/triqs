.. highlight:: c

Functional construct (II) : map
========================================================

* **Purpose** :
  fold implements the folding (or reduction) on the array.

* **Syntax** :

  If `f` is a function, or a function object of synopsis (T, R being 2 types) ::

       R f ( T, R )
  
  then  ::

    auto F = fold(f);

  is a callable object which can fold any array of value_type T.

  So, if 
  
   * A is a type which models the :ref:`HasImmutableArrayInterface` concept
     (e.g. an array , a matrix, a vector, an expression,  ...)

   * A::value_type is T

  then ::

    fold (f) ( A, R init = R() ) = f( f( f( ... f( a(0,1), f(a(0,0), init))))) 
          
  Note that : 
   
   * The order of traversal is the same as foreach.
   * The precise return type of fold is an implementation detail, depending on the precise type of f, 
     use auto to keep it.
   * The function f will be inlined if possible, leading to efficient algorithms.
   * fold is implemented using a foreach loop, hence it is efficient.

* **Example** : 
  
  Many algorithms can be written in form of map/fold.

  The function *sum* which returns the sum of all the elements of the array is implemented approximately like this 
  (this function already exists in the lib, cf ???) ::

   template <class A>
   typename A::value_type sum(A const & a) { return fold ( std::plus<typename A::value_type>())  (a); }

  Note in this example : 
   
   * the simplicity of the code
   * the genericity : it is valid for any dimension of array.
   * internally, the library will rewrite it as a series of for loop, ordered in the TraversalOrder of the array
     and inline the plus operator.


