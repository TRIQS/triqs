C++11/14 & notations
=====================

C++11/C++14
---------------

TRIQS is a C++11 library. It *requires* a last generation C++ compiler (Cf :ref:`require_cxx_compilers`).

Indeed, the development of C++ is very dynamic these years.
The language and its usage is changing very profoundly with the introduction of several 
notions (e.g. move semantics, type deduction, lambda, variadic templates  ...).
Moreover, C++11 compliant compilers are now widely available, with gcc and clang.

A major consequence of this evolution is that writing libraries
has become much more accessible, at a *much* lower cost in development time,
with clearer, shorter and more readable code, hence maintainable.
Efficient techniques which were considered before as complex and reserved to professional C++ experts
are now becoming simple to implement, like e.g. expression templates.

The implementation of most of the TRIQS library (e.g. clef, arrays) would be either impossible or at least 
much more complex and time consuming (with a lot of abstruse boost-like construction)
in previous versions of C++.

Besides, this evolution is not finished.
The new coming standard, C++14, expected to be adopted and implemented very soon, 
will still make it a lot better. In particular, the concept support (template constraints)
will hopefully solve the most problematic issue with metaprogramming techniques, i.e. the lack of concept
check at compile time, resulting in long and obscur error messages from the compiler when *using* the library.
Hence, TRIQS will move to C++14 as soon as compilers are available.

Notation
----------

In the documentation, we make use in particular of the C++14 concept notation for template.
For example ::

  template<ImmutableArray A> void f(A const & a);

means that f is a template ::
  
  template<typename A> void f(A const & a);
 
which is enabled or valid only for types A which models the `ImmutableArray` concept.
