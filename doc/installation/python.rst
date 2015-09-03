.. _python_install:

Python and the scientific python tools
====================================================

On some machines, not all the necessary scientific python tools are present
(e.g. old distribution, computing centers, etc.), or perhaps just not in the right versions.

There is a way to efficiently solve this issue without recompiling your own python, and
all the libs, which is painful, and specifically **avoiding any upgrade on your system** (which
is crucial on e.g. cluster machines).

.. _virtualenv:
.. highlight:: bash

Using virtualenv to install/upgrade some python packages locally
----------------------------------------------------------------------------

In the frequent cases where only one or two packages are missing, there is a simple solution
called virtualenv.

Principle
^^^^^^^^^

virtualenv is a python package that allow you to install in your home a simple copy (or mirror)
or python, and install and upgrade some packages for you, without touching the systems.

.. note:: This works as long as the other dependencies of the packages on e.g. C++ libraries are satisfied.

Usage
^^^^^

* First install virtualenv, or ask your system manager to do so.
  On Ubuntu, run::

    sudo apt-get install python-virtualenv

* To create a new python in your home, do e.g.::

    virtualenv ~/.my_python --system-site-packages

  This create in ~/.my_python a clone of the python of the system, with its packages.

  You can then use it with::

    ~/.my_python/bin/python

* To make this your default python, just add in your ~/.bash_aliases (or ~/.bashrc)::

    source $HOME/.my_python/bin/activate

  Then, after relaunching your terminal::

    which python

  should return::

    /home/ME/.my_python/bin/python

* You can install any python package for this local python installation easily. For example, by running::

   easy_install --upgrade ipython

  you should have the latest ipython notebook.
