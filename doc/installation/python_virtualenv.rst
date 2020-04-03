.. _python_virtualenv:
.. highlight:: bash

Python Virtual Environments
===========================

On some machines, a few of the necessary Python dependencies of TRIQS may be missing or outdated
(as e.g. on old distributions and computing clusters).

While the cleanest solution would be to update or install the corresponding packages within the 
system, this is not always possible. In those cases, a Python virtual environment set up in your
home directory is a clean an a simple solution.

This is achieved via a Python package called `virtualenv. <https://virtualenv.pypa.io/en/stable>`_

.. note:: This works as long as the other dependencies of the packages on e.g. C++ libraries are satisfied.

Usage
-----

We present a below a few basic instructions for using the Python virtualenv package.
For more information please refer to the official `user guide. <https://virtualenv.pypa.io/en/stable/userguide/>`_

* As a first step, install the virtualenv package, or ask your system manager to do so. On Ubuntu, run::

    sudo apt-get install python-virtualenv

* To create a new isolated Python virtual environment in your home directory, do e.g.::

    virtualenv $HOME/.my_python --system-site-packages

  You can then use it by loading it into your shell :ref:`environment <environment_vars>` via::

    source $HOME/.my_python/bin/activate

  To confirm that you are now using your own Python virtual environment, running the command::
  
    which python

  should return::

    path_to_your_home/.my_python/bin/python

* To make this your default Python setup, just add the line::

    source $HOME/.my_python/bin/activate

  to your ``$HOME/.bash_profile``

* To install or upgrade any Python package for this local Python environment, run::

    pip install --upgrade package_name

Compiling a package from source
-------------------------------

Some packages, such as mpi4py should be compiled against the particular library that they depend on.
In those cases you can instruct ``pip`` to build the package from source by using the ``--no-binary`` option.
For example::

  pip install --upgrade --no-binary=mpi4py mpi4py  

would install or upgrade the mpi4py package to the latest version, by building the packages from source. On a cluster, be sure to load the proper library dependencies (e.g. openmpi) into your environment before this step.
