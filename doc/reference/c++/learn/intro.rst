
Developing a project with TRIQS
===============================

Welcome to this tutorial! The goal of these notes is to give a practical
introduction to the development of a code that uses the TRIQS headers and
libraries. Rather than being completely general this tutorial will guide you
through the development of a simple CT-INT impurity solver.

At first, we will see how to write a code that uses TRIQS but that is not meant
to become part of TRIQS. This is especially relevant when you write a pure C++
code for your personal use. You will learn how to write a Monte Carlo
simulation with a simple example.

The next step will be to *pythonize* your code. This is very convenient to
change parameters or do simple pre-simulation calculations. It is also
the way most of TRIQS applications are done.


.. toctree::
   :maxdepth: 2

   triqs_library
   first_mc
