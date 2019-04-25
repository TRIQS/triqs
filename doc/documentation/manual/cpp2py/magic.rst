Magic for Jupyter notebook
=============================

The Jupyter cell magic `%%triqs` allows a direct use of the c++2py in the notebook.

It allows to automatically wrap a piece of C++ code into Python, compile it on the fly and
load it in the notebook.

.. warning::
 
   The use of this magic requires the compilation of TRIQS with clang.
