.. _python_virtualenv:
.. highlight:: bash

Python Virtual Environments
===========================

On some machines, a few of the necessary Python dependencies of TRIQS may be missing or outdated
(as e.g. on old distributions and computing clusters) or you might not be able to use the system's
Python installation.

In those cases, a Python virtual environment set up in your home directory is a clean and simple solution.

.. note:: This works as long as the other dependencies of the packages on e.g. C++ libraries are satisfied.

Usage
-----

We present below a few basic instructions for using Python's ``venv`` package.
For more information please refer to the official `documentation. <https://docs.python.org/3/library/venv.html>`_

* As of verison 3.3, Python comes with a built-in virtual environment package, called ``venv``. Although it should
  be included in the basic Python installation, on some systems you might need to install it separately. For example
  on Ubuntu, run::

    sudo apt-get install python3-venv

* As a first step, create a directory for your virtual environments in your home path, e.g.::

    mkdir $HOME/.venv

* To make a new isolated Python virtual environment called ``my_python``, do::

    python3 -m venv $HOME/.venv/my_python

* You can then use it by loading it into your shell :ref:`environment <environment_vars>` via::

    source $HOME/.venv/my_python/bin/activate

* Confirm that your own Python virtual environment is activated by running the command::
  
    which python

  which should return::

    path_to_your_home/.venv/my_python/bin/python

* To install or upgrade any Python package for this local Python environment, run::

    pip install --upgrade package_name

* If you want to make this your default Python setup, just add the line::

    source $HOME/.venv/my_python/bin/activate

  to your ``$HOME/.bash_profile``

Compiling a package from source
-------------------------------

Some packages, such as mpi4py should be compiled against the particular library that they depend on.
In those cases you can instruct ``pip`` to build the package from source by using the ``--no-binary`` option.
For example::

  pip install --upgrade --no-binary=mpi4py mpi4py  

would install or upgrade the mpi4py package to the latest version, by building the packages from source. On a cluster, be sure to load the proper library dependencies (e.g. openmpi) into your environment before this step.
