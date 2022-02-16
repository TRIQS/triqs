Convertion  vs wrapping
=======================

Wrapping classes from C++ to Python
-----------------------------------

The *wrapping*  of a C++ function or class consist in
creating a *new* function or class in Python which exposes its data and functionnality.
For example, by wrapping a C++ Green function class, one produces a
new Python module, written in C/C++, with new Python objects.

This notion is to be distinguished from the more general notion of *convertion*.

Convertion
----------

Each time a function in C++ is called from python:

* Each argument has to be converted from Python to C++
* The function is then called in C++
* Its result is then converted back to Python (to None for a function returning void).

Indeed the types are different in Python and C++. A convertion is necessary.
E.g. a Python integer is *not* simply a C++ int.

The convertion is determined by the C++ type.

The wrapped classes are automatically convertible, as well as a list of
C++/Python types.
The t able of possible convertions is (here by convertion T\ :sub:`p` is the Python convertion of C++ T):

+------------------------------------------------------+-------------------------------------------------------------------+
| C++                                                  | Python                                                            |
+======================================================+===================================================================+
| int                                                  | int                                                               |
+------------------------------------------------------+-------------------------------------------------------------------+
| double                                               | float                                                             |
+------------------------------------------------------+-------------------------------------------------------------------+
| nda::array                                 | numpy.ndarray                                                     |
+------------------------------------------------------+-------------------------------------------------------------------+
| std::string                                          | string                                                            |
+------------------------------------------------------+-------------------------------------------------------------------+
| std::vector<T>                                       | list of T\ :sub:`p`                                               |
+------------------------------------------------------+-------------------------------------------------------------------+
| std::tuple<T\ :sub:`1`,T\ :sub:`2`,T\ :sub:`3`, ...> | tuple of (T\ :sub:`p1`, T\ :sub:`p2`, T\ :sub:`p3`, ...)          |
+------------------------------------------------------+-------------------------------------------------------------------+
| std::map<K,V>                                        | dict with keys of type K\ :sub:`p` and values of type V\ :sub:`p` |
+------------------------------------------------------+-------------------------------------------------------------------+
| class/struct my_struct                               | wrapped class MyStruct                                            |
+------------------------------------------------------+-------------------------------------------------------------------+










