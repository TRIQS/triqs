
Second-quantization operators
=============================

.. highlight:: c

``many_body_operator`` is a template class, which implements the algebra of fermionic operators.
An object of this class represents a general fermionic operator and supports all standard algebraic operations (sums, products, multiplication by a scalar).
It allows to write readable and clean C++ code involving various operators, such as Hamiltonians and observables of many-body systems.

.. note::

    The internal storage of a ``many_body_operator`` object is not based on a matrix representation.
    Instead of that the object stores a list of normally-ordered monomials in basis elements
    (creation and annihilation operators), accompanied by scalar coefficients. This approach allows
    to minimize the required storage space.

``many_body_operator`` is declared as ::

    template<typename scalar_t> class many_body_operator;

Template parameters
-------------------

    * **scalar_t** determines the type of a scalar to construct the algebra.

``scalar_t`` will be ``double`` or ``std::complex<double>`` most of the time. One may, however, use
an arbitrary user-defined type, as long as it meets two conditions:

* Objects of the type form a field, i.e. they support operations ``+``,``-``,``*``,``=``
  and their assignment versions ``+=``, ``-=``, ``*=``, ``/=``.
* There is a free function ``bool is_zero(scalar_t [const&] x)``, declared in namespace ``triqs::utility``,
  which detects an object of the type ``scalar_t`` being zero.

One more requirement must be met to make free function ``dagger()`` available:

* There is a free function ``scalar_t _conj(scalar_t [const&] x)``, declared in namespace ``triqs::utility``,
  which returns a complex conjugate of ``x``.

Construction/factories
----------------------

``many_body_operator`` provides a minimal set of constructors: default, copy and move constructor. The default constructor
creates a zero operator.

Three factory functions can be used to construct nontrivial operators: ::

    // Annihilation
    template<typename scalar_t = double, typename... IndexTypes> many_body_operator<scalar_t> c(IndexTypes... ind);
    // Creation
    template<typename scalar_t = double, typename... IndexTypes> many_body_operator<scalar_t> c_dag(IndexTypes... ind);
    // Number of particles
    template<typename scalar_t = double, typename... IndexTypes> many_body_operator<scalar_t> n(IndexTypes... ind);

``IndexTypes`` is a sequence of index types with any number of elements, each being ``int`` or ``std::string``.

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

``many_body_operator`` class defines a number of arithmetic operations with objects of the class and constants of type ``scalar_t``.
If ``A`` and ``B`` are objects of class ``many_body_operator`` (instantiated with *the same scalar and index types*) and ``x`` is an
instance of ``scalar_t``, then the following expressions are valid: ::

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

Within the current implementation, ``many_body_operator`` provides no type conversions between objects with
different scalar types or index types.
For example, one cannot mix operators with ``scalar_t = double`` and ``scalar_t = std::complex<double>`` in a single expression.
Nevertheless, one operator can be assigned from another if their scalar types are compatible.

An instance of ``many_body_operator`` can be inserted into an output stream, provided ``scalar_t`` supports insertion into the stream. ::

    many_body_operator<double> x = c(0);
    many_body_operator<double> y = c_dag(1);

    std::cout << (x + y)*(x - y) << std::endl; // prints "2*C^+(1)C(0)"

Methods
-------
::

    bool is_zero() const
Returns ``true`` if the object is a precise zero.

Free functions
--------------
::

    many_body_operator<scalar_t> dagger(many_body_operator<scalar_t> const& op)
Returns the Hermitian conjugate of ``op``.

Iteration over monomials
------------------------

The aim of ``many_body_operator`` is to have a class allowing to encode different operator expressions in C++ in the form closest to the mathematical notation.
At the same time, one would like to explicitly extract the structure of a defined operator (to calculate its matrix elements, for instance).
For this purpose ``many_body_operator`` exposes the following part of its interface:

- ``using indices_t = std::vector<boost::variant<int, std::string>>;``
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
    It can be dereferenced into a special proxy object, which carries two data members: ``scalar_t coef`` and ``monomial_t const& monomial``.

- ``begin()``/``cbegin()``
    Returns ``const_iterator`` pointing at the first monomial.

- ``end()``/``cend()``
    Returns ``const_iterator`` pointing past the end.

Here is an example of use: ::

    using Op = many_body_operator<double>;
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

Objects of ``many_body_operator`` are ready to be serialized/deserialized with Boost.Serialization.
It also allows to transparently send/receive them through Boost.MPI calls.

.. warning::

    Storing ``many_body_operator`` in HDF5 is not yet implemented.

Python
------

.. highlight:: python

Python wrapper for ``many_body_operator`` class is called ``Operator``. It is found in module ``pytriqs.operators.operators`` : ::

    from pytriqs.operators.operators import Operator, c, c_dag, n

It corresponds to a specialized version of ``many_body_operator``: ``double`` as the scalar type and two indices.
All arithmetic operations implemented in C++ are also available in Python as well as special methods ``__repr__()`` and ``__str__()``.

.. runblock:: python

    from pytriqs.operators.operators import *
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

