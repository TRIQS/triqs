
.. highlight:: bash

Getting started
===============

TRIQS provides a set of python libraries that allows to easily write scripts
for the study of quantum interaction systems. The executable ``ipytriqs`` which
is installed in the :file:`path_to_install_directory/bin` directory is a normal
ipython interpreter (see below for details about ipython) which has access to
the TRIQS modules. You can either start an interactive session with::

    $ ipytriqs

or run scripts with::

    $ ipytriqs my_script.py

There is also an executable which allows to start an ipython notebook
in your browser, just type::

   $ ipytriqs_notebook

Now you can try to run some scripts to get familiar with TRIQS. Why don't you
:ref:`go to our python tour <tour>` and run some of the examples there?


Learn more about python, ipython and the notebook
-------------------------------------------------

The ``ipytriqs`` executable calls an `ipython interpreter
<http://ipython.org>`_.  This is basically a more user-friendly version of the
standard python interpreter with an enhanced interactive shell that makes it
easy to visualize data.  It also provides the ipython notebook, a browser-based
notebook with support for text, mathematical expressions, inline plots and
inline python scripts. We really think it is a very powerful tool and recommend
that you spend some time learning ipython and the notebook. Here are some
useful links to learn python, ipython, scipy.

* To learn the Python language itself the recommended starting point is the
  standard `python tutorial <http://docs.python.org/tutorial>`_.

* A good set of lectures is the `Scipy lecture notes
  <http://scipy-lectures.github.com/>`_.   

* A good starting point to learn about scientific computing with Python and
  related ideas is `Software carpentry <http://software-carpentry.org>`_, which
  provides nice video/slides `lectures on Python
  <http://software-carpentry.org/4_0/python>`_

* Python has a large number of libraries, which can be used in combination with
  TRIQS. For example:

  * The Python `standard library <http://docs.python.org/library>`_ is already
    very rich.

  * `Numpy <http://docs.scipy.org/doc/numpy/user>`_  allows to manipulate
    multidimensionnal arrays (cf also the `tutorial
    <http://www.scipy.org/Tentative_NumPy_Tutorial>`_).

  * `Scipy <http://www.scipy.org>`_ includes many packages for scientific
    computing.

  * `Matplotlib <http://matplotlib.sourceforge.net>`_ offers very nice plotting
    possibilities.

  * `SymPy <http://sympy.org/>`_ provides some formal computing capabilities.

  * `Cython <http://cython.org/>`_ is a quick way to build simple `C` modules in
    computationally intense case.
    
