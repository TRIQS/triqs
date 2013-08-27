.. highlight:: c

.. _arr_map_fold:

Functional constructs : map & fold
###########################################

Two standard functional constructs are provided : 

* *map* that promotes a function of the array element to a function of the array,  
  element by element.

* *fold* is the reduction of a function on the array. 

.. _map:

map
========================================================
* **Purpose** :

  map promotes any function into an `array function`, acting term by term.

* **Synopsis** ::

    template<class F> auto map (F f);

  If `f` is a function, or a function object :: 
   
    ValueType2 f(ValueType1)

  Then map(f) is a function::
  
     template<ImmutableCuboidArray A> auto map(f) (A const &)
   
  with : 
     * A::value_type == ValueType1
     * The returned type of map(f) models the :ref:`ImmutableCuboidArray` concept

       * with the same domain as A
       * with value_type == ValueType2

* N.B. : Some cases require explicit cast, e.g. for the standard abs function (already defined in arrays/mapped_function.hpp) , 
  or the compiler does not know which std::abs you are talking about ::

   auto Abs = map( std::function<double(double)>(static_cast< double (*)(double)> (std::abs)) );
    
* TO DO : clarify the F f or F const & : check code and put an example with std::ref.

* **Example** : 

.. compileblock::

   #include <triqs/arrays.hpp>
   using triqs::arrays::matrix; using triqs::arrays::make_matrix; using triqs::clef::placeholder;
   int main() { 
    // declare and init a matrix
    placeholder<0> i_; placeholder<1> j_;
    matrix<int> A (2,2); A(i_,j_) <<  i_ + j_ ; 
    
    // the mapped function
    auto F = triqs::arrays::map([](int i) { return i*2.5;});

    std::cout<< "A = "        << A                        << std::endl; 
    std::cout<< "F(A) = "     << F(A)                     << std::endl; // oops no computation done
    std::cout<< "F(A) = "     << make_matrix(F(A))     << std::endl;
    std::cout<< "3*F(2*A) = " << make_matrix(3*F(2*A)) << std::endl;
   }


fold
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
  
   * A is a type which models the :ref:`ImmutableCuboidArray` concept
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

  The function :ref:`arr_fnt_sum` which returns the sum of all the elements of the array is implemented as ::
   template <class A>
   typename A::value_type sum(A const & a) { return fold ( std::plus<typename A::value_type>())  (a); }



  Note in this example : 
   
   * the simplicity of the code
   * the genericity : it is valid for any dimension of array.
   * internally, the library will rewrite it as a series of for loop, ordered in the TraversalOrder of the array
     and inline the plus operator.








