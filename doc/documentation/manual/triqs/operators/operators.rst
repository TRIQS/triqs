
.. _many_body_operator:

Second-quantization operators
=============================

.. highlight:: c

``many_body_operator_generic`` is a templated class, which implements the algebra of fermionic operators.
An object of this class represents a general fermionic operator and supports all standard algebraic operations (sums, products, multiplication by a scalar).
It allows to write readable and clean C++ code involving various operators, such as Hamiltonians and observables of many-body systems.

.. note::

    The internal storage of the ``many_body_operator_generic`` object is not based on a matrix representation.
    Instead of that, the object stores a list of normally-ordered monomials in basis elements
    (creation and annihilation operators), accompanied by scalar coefficients. This approach allows
    to minimize the required storage space.

``many_body_operator_generic`` is declared as ::

    namespace triqs {
    namespace operators {
        template<typename ScalarType> class many_body_operator_generic;
    }}

Template parameters
-------------------

    * **ScalarType** determines the scalar type to construct the algebra.

``ScalarType`` will be ``double``, ``std::complex<double>``, or ``triqs::utility::real_or_complex`` most of the time.
``triqs::utility::real_or_complex`` is a variant numeric type that can carry either a real or a complex value.
Type of the result of an arithmetic expression involving ``real_or_complex`` objects is dynamically deduced at runtime.

One may, however, use an arbitrary user-defined type, as long as it meets two conditions:

* Objects of the type form a field, i.e. they support operations ``+``, ``-``, ``*``, ``=``
  and compound assignments ``+=``, ``-=``, ``*=``, ``/=``.
* There is a free function ``bool is_zero(ScalarType [const&] x)``, declared in namespace ``triqs::utility``,
  which detects if an object of the type ``ScalarType`` is zero.

One more requirement must be met to make free function ``dagger()``
(Hermitian conjugate) available:

* There is a free function ``ScalarType conj(ScalarType [const&] x)``, declared in namespace ``triqs::utility``,
  which returns a complex conjugate of ``x``.

For the sake of convenience, three type aliases are declared: ::

    // Operator with real matrix elements
    using many_body_operator_real = many_body_operator_generic<double>;
    // Operator with complex matrix elements
    using many_body_operator_complex = many_body_operator_generic<std::complex<double>>;
    // Operator with polymorphic matrix elements
    using many_body_operator = many_body_operator_generic<real_or_complex>;

Construction/factories
----------------------

``many_body_operator_generic`` provides a minimal set of constructors: ::

    // Default constructor; constructs a zero operator
    many_body_operator_generic();
    // Construct a constant operator
    many_body_operator_generic(ScalarType const& x);
    // Copy-constructor
    many_body_operator_generic(many_body_operator_generic const&);
    // Move-constructor
    many_body_operator_generic(many_body_operator_generic&&) = default;

Three factory functions can be used to construct nontrivial operators: ::

    // Annihilation
    template<typename ScalarType = real_or_complex, typename... IndexTypes> many_body_operator_generic<ScalarType> c(IndexTypes... ind);
    // Creation
    template<typename ScalarType = real_or_complex, typename... IndexTypes> many_body_operator_generic<ScalarType> c_dag(IndexTypes... ind);
    // Number of particles
    template<typename ScalarType = real_or_complex, typename... IndexTypes> many_body_operator_generic<ScalarType> n(IndexTypes... ind);

``IndexTypes`` is an arbitrarily long sequence of index types, each being ``int`` or ``std::string``.

Creation and annihilation operators obey the canonical anticommutation relation

.. math::
    \hat c^\dagger_{\mathrm{ind}_1} \hat c_{\mathrm{ind}_2} +
    \hat c_{\mathrm{ind}_2} \hat c^\dagger_{\mathrm{ind}_1} = \delta_{{\mathrm{ind}_1},{\mathrm{ind}_2}},

and the number of particle is defined as

.. math::
    \hat n_\mathrm{ind} = \hat c^\dagger_\mathrm{ind} \hat c_\mathrm{ind}.

There is no need to preregister valid values of ``ind`` before they are used to create an elementary operator.
This means, that an algebra can be extended with new basis elements on-the-fly, after some expressions have been created.

Overloaded operations
---------------------

``many_body_operator_generic`` class defines a number of arithmetic operations with objects of the class and constants of type ``ScalarType``.
If ``A`` and ``B`` are objects of class ``many_body_operator_generic`` (instantiated with *the same scalar and index types*) and ``x`` is an
instance of ``ScalarType``, then the following expressions are valid: ::

    // Addition
    A + B; A + x; x + A;
    A += B; A += x;

    // Subtraction
    A - B; A - x; x - A;
    A -= B; A -= x; -A;

    // Multiplication
    A*B; x*A; A*x;
    A *= B; A *= x;

    // Division by scalar
    A / x;
    A /= x;

The result of any of the defined operations is guaranteed to preserve its normally ordered form.

``many_body_operator_generic`` can be copy-constructed and assigned from another ``many_body_operator_generic`` instantiation
with a compatible scalar type. For example, it is possible to copy-construct ``many_body_operator_complex`` from
``many_body_operator_real``, but not vice versa.

An instance of ``many_body_operator_generic`` can be inserted into an output stream, provided ``ScalarType`` supports insertion into the stream. ::

    many_body_operator_generic<double> x = c(0);
    many_body_operator_generic<double> y = c_dag(1);

    std::cout << (x + y)*(x - y) << std::endl; // prints "2*C^+(1)C(0)"

Member types
------------
::

    using scalar_t = ScalarType;
Accessor to the ``ScalarType``.

Methods
-------
::

    bool is_zero() const;
Returns ``true`` if this operator is a precise zero.

::

    triqs::hilbert_space::fundamental_operator_set make_fundamental_operator_set() const;
Returns :ref:`fundamental_operator_set` containing all indices met within this operator.

::

    static many_body_operator_generic make_canonical(bool is_dag, indices_t indices);
Returns a canonical operator (creation, if ``is_dag = true``, annihilation otherwise) with given ``indices``.


Free functions
--------------
::

    many_body_operator_generic<ScalarType> dagger(many_body_operator_generic<ScalarType> const& op);
Returns the Hermitian conjugate of ``op``.

::

    many_body_operator_generic<ScalarType> real(many_body_operator_generic<ScalarType> const& op);
Returns a copy of ``op`` with the imaginary parts of all monomial coefficients set to zero.

::

    many_body_operator_generic<ScalarType> imag(many_body_operator_generic<ScalarType> const& op);
Returns a copy of ``op`` with the real parts of all monomial coefficients set to zero.

::

    template<typename L>
    many_body_operator_generic<ScalarType> transform(many_body_operator_generic<ScalarType> const& op, Lambda&& L);
Transforms ``op`` by applying a given functor ``L`` to each monomial.
The functor must take two arguments convertible from ``monomial_t`` (see next paragraph) and ``ScalarType``
respectively, and return a new coefficient of the monomial.


Iteration over monomials
------------------------

The aim of ``many_body_operator_generic`` is to have a class allowing to encode different operator expressions in C++
in the form closest to the mathematical notation. At the same time, one would like to explicitly extract the structure
of a given operator (to calculate its matrix elements, for instance).
For this purpose ``many_body_operator_generic`` exposes the following part of its interface:

- ``using indices_t = std::vector<triqs::utility::variant_int_string>;``
    A vector of indices. Each index is a variant type with two options: ``int`` or ``std::string``.

- ``struct canonical_ops_t``
    This structure represents an elementary operator (basis element of the algebra).
    ::

        struct canonical_ops_t {
            bool dagger;       // true = creation, false = annihilation
            indices_t indices; // values of indices
            ...
        };

- ``using monomial_t = std::vector<canonical_ops_t>;``
    A *normally ordered* sequence of elementary operators (*monomial*).

- ``using const_iterator = ...;``
    A bidirectional constant iterator to the list of monomials.
    It can be dedocumentation/manual/triqsd into a special proxy object, which contains two data members: ``scalar_t coef`` and ``monomial_t const& monomial``.

- ``begin()``/``cbegin()``
    Returns ``const_iterator`` pointing at the first monomial.

- ``end()``/``cend()``
    Returns ``const_iterator`` pointing past the end.

Here is an example of use: ::

    using Op = many_body_operator;
    Op H = -0.5*(n(0) + n(1)) + n(0)*n(1);

    for(Op::const_iterator it = H.begin(); it != H.end(); ++it){
        double coef = it->coef;
        auto monomial = it->monomial;

        std::cout << "Coefficient: " << coef << std::endl;
        std::cout << "Monomial: " << std::endl;
        for(auto const& o : monomial){
            std::cout << "dagger: " << o.dagger << " index: " << o.indices[0] << " "; // only 1 index per elementary operator
        }
        std::cout << std::endl;
    }

.. highlight:: none
The output should be ::

    Coefficient: -0.5
    Monomial:
    dagger: 1 index: 0 dagger: 0 index: 0
    Coefficient: -0.5
    Monomial:
    dagger: 1 index: 1 dagger: 0 index: 1
    Coefficient: 1
    Monomial:
    dagger: 1 index: 0 dagger: 1 index: 1 dagger: 0 index: 1 dagger: 0 index: 0

Serialization & HDF5
--------------------

Objects of ``many_body_operator_generic`` are ready to be serialized/deserialized with Boost.Serialization.
This also allows to transparently send/receive them through Boost.MPI calls.

Writing to/reading from HDF5 is supported for the polymorphic version of the operators (``many_body_operator``)
provided they contain no terms beyond quartic.

Python
------

.. highlight:: python

Python wrapper for ``many_body_operator`` class is called ``Operator``. It is found in module ``triqs.operators.operators`` : ::

    from triqs.operators.operators import Operator, c, c_dag, n

It corresponds to a specialized version of ``many_body_operator_generic``: ``real_or_complex`` as the scalar type and two indices.
All arithmetic operations implemented in C++ are also available in Python as well as special methods ``__repr__()`` and ``__str__()``.

.. code-block:: python

    from triqs.operators.operators import *
    from itertools import product

    C_list = [c(1,0),c(2,0)]
    Cd_list = [c_dag(1,0), c_dag(2,0)]

    print "Anticommutators:"
    for Cd,C in product(Cd_list,C_list):
        print "{", Cd, ",", C, "} =", Cd*C + C*Cd

    print "Commutators:"
    for Cd,C in product(Cd_list,C_list):
        print "[", Cd, ",", C, "] =", Cd*C - C*Cd

    x = c('A',0)
    y = c_dag('B',0)
    print "x =", x
    print "y =", y

    print "Algebra:"

    print "-x =", -x
    print "x + 2.0 =", x + 2.0
    print "2.0 + x =", 2.0 + x
    print "x - 2.0 =", x - 2.0
    print "2.0 - x =", 2.0 - x
    print "3.0*y =", 3.0*y
    print "y*3.0 =", y*3.0
    print "x + y =", x + y
    print "x - y =", x - y
    print "(x + y)*(x - y) =", (x + y)*(x - y)

    print "x*x is zero:", (x*x).is_zero()
    print "dagger(x) = ", dagger(x)

