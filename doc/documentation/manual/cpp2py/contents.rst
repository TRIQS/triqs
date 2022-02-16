.. index:: wrapper

.. module:: cpp2py.wrap_generator

.. _wrapper:

Cpp2Py: a C++/Python interfacing tool
=====================================

`Cpp2Py <https://github.com/triqs/cpp2py>`_ is a standalone tool that generates the necessary wrapping code between C++ and Python.
Wrapping C++ functions and classes in Python is a non trivial problem because the two languages are quite different
(e.g. C++ uses typically value semantics, while Python uses only counted documentation/manual/triqss, the C++ equivalent of std::shared_ptr).

This tool has the following main features:

 * it is entirely automatic in most cases, i.e. except for some advanced features that require additionnal inputs.
 * it is quite flexible.
 * it can be used with a magic cell command in the Python notebook.


.. toctree::
   :maxdepth: 2

   convert
   principle
   reference
