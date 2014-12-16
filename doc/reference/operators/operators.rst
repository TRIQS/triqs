
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

The aim of ``many_body_operator`` is to have a class, which allows to encode different operator expressions in C++ in the form closest to the mathematical notation.
But at the same time, one would like to explicitly extract the structure of a defined operator (to calculate its matrix elements, for example).
For this purpose ``many_body_operator`` exposes the following part of its interface:

- ``struct canonical_ops_t``
    This structure represents an elementary operator (basis element of the algebra).
    ::

        struct canonical_ops_t { 
            bool dagger;    // true = creation, false = annihilation
            std::tuple<IndexTypes...> indices; // values of indices
        };

- ``typedef ... monomial_t;``
    An ordered sequence of elementary operators (monomial).

- ``typedef ... const_iterator;``
    A bidirectional constant iterator to the list of monomials.
    It can be dereferenced into a special proxy object, which carries two data members: ``coef`` and ``monomial``.

- ``begin()``/``cbegin()``
    Returns ``const_iterator`` pointing at the first monomial.

- ``end()``/``cend()``
    Returns ``const_iterator`` pointing past the end.

Here is an example of use: ::

    typedef many_body_operator<double> Op;
    Op H = -0.5*(n(0) + n(1)) + n(0)*n(1);

    for(Op::const_iterator it = H.begin(); it != H.end(); ++it){
        double coef = it->coef;
        Op::monomial_t monomial = it->monomial;

        std::cout << "Coefficient: " << coef << std::endl;
        std::cout << "Monomial: " << std::endl;
        for(auto const& o : monomial){
            std::cout << "dagger: " << o.dagger << " index: " << std::get<0>(o.indices) << " "; // only 1 index per elementary operator 
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

``many_body_operator`` is a fully HDF-compliant class.

Python
------

.. highlight:: python

This class can be cimported from Cython: ::

    from many_body_operator cimport many_body_operator

It corresponds to a specialized version of ``many_body_operator``: ``double`` as the scalar type and two indices of type ``std::string``.
There are also an extension type ``Operator`` and three factory function to be imported from Python: ::

    from many_body_operator import Operator, C, C_dag, N

All arithmetic operations implemented in C++ are also available in Python as well as special methods ``__repr__()`` and ``__str__()``.
The factory functions accept two arguments of any types and convert them into strings using Python operator ``str()``.
