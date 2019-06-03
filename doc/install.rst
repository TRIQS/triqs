.. index:: installation

.. highlight:: bash

.. _installation:

Installation
============

TRIQS and its applications are provided *a la carte*:
after you have installed the TRIQS library, you will be able to easily install
various TRIQS-based applications: impurity solvers, tools for DFT+DMFT
calculations, etc.

This page describes the installation of the TRIQS library itself. The
installation procedure of the applications is described on their respective
sites, under 'Install'.

Philosophy
----------

The TRIQS project is in perpetual dynamic evolution such that we can get our
developments directly to users straight out of the oven.

However, we also understand that some users may not wish to constantly update
their codes, and are happy to use an older but perhaps more stable version.

To this end, we propose two options to the user:

#. You follow the master branch in the git repository of TRIQS and all applications.
   This will guarantee that you are using the latest stable release including essential
   bug-fixes. Note that new releases might occasionally include changes of the API, which
   we summarize in a set of release notes.
   We use continuous integration to ensure that the master branch always compiles and 
   passes all tests. This is checked for both the TRIQS library and several public 
   (and private) applications.

#. You use a version tag, e.g. version 2.0, for TRIQS and all applications.
   This guarantees complete reproducibility, while you might be missing out on the latest
   features and bug-fixes.

Packaged Versions of TRIQS
==========================

.. _triqs_docker:
Docker
------

The virtualization software `docker <https://docs.docker.com/install>`_  can be used to
run a triqs environment for both Jupyter Notebook and shell access on a variety of operating
systems. Once docker is installed, just pull the latest image with::

        docker pull flatironinstitute/triqs
 
and follow the commands on the `image-website <https://hub.docker.com/r/flatironinstitute/triqs>`_.

.. note:: Docker images allows for an easy set-up on cluster machines that provide the virtualization software `singularity <https://singularity.lbl.gov>`_.
          For more information please refer to the `singularity documentation <https://singularity.lbl.gov/docs-docker>`_.

Binder
------

`Binder <https://mybinder.org>`_ is a web-service running Jupyter notebooks for direct access.
A TRIQS Jupyter notebook environment can be accessed directly at `triqs.github.io/notebook <https://triqs.github.io/notebook>`_.

.. note:: Once your binder session is closed, your work will be lost.

.. _ubuntu_debian:
Ubuntu Debian packages (experimental)
-------------------------------------

We provide Debian packages for the Ubuntu LTS Versions 16.04 (xenial) and 18.04 (bionic).::

        sudo apt-get update && sudo apt-get install -y software-properties-common apt-transport-https curl
        source /etc/lsb-release
        curl -L https://users.flatironinstitute.org/~ccq/triqs/$DISTRIB_CODENAME/public.gpg | sudo apt-key add -
        sudo add-apt-repository "deb https://users.flatironinstitute.org/~ccq/triqs/$DISTRIB_CODENAME/ /"
        sudo apt-get update && sudo apt-get install -y triqs

This will install the minimal runtime packages and triqs into the system tree at /usr.

.. note:: Since TRIQS version 2.1 we include cpp2py in the TRIQS debian package.
          If you have installed an earlier packaged version of triqs (or cpp2py)
          be sure to remove it before updating TRIQS to avoid package conflicts.

If you aim to compile applications against the triqs library, additional :ref:`development libraries <ubuntu>` have to be installed. You should further set the following :ref:`environment variables <environment_vars>` permanently::

        export CPLUS_INCLUDE_PATH=/usr/include/openmpi:/usr/include/hdf5/serial/:$CPLUS_INCLUDE_PATH
        source /usr/share/triqsvars.sh

For full c++2py functionality, please read the corresponding :ref:`section <ubuntu_cpp2py>` for the Ubuntu set-up.

Compiling TRIQS from source (Advanced)
======================================

Prerequisites
-------------

The TRIQS library relies on a certain number of standard libraries and tools
described in the :ref:`list of requirements <requirements>`. Please pay
particular attention to the :ref:`C++ compilers<require_cxx_compilers>` and to
:ref:`Python virtual environments<python_virtualenv>`.  Here are instructions to
install these necessary libraries on two standard systems:

.. toctree::
   :maxdepth: 1

   installation/ubuntu
   installation/osx_install

.. note:: Please ensure that you have all the dependencies installed before proceeding further!

Installation steps
------------------

We provide hereafter the build instructions in the form of a documented bash script.
You can adapt INSTALL_PREFIX, NCORES for your local settings.
Note that, contrary to previous versions of TRIQS,
the installation directory CMAKE_INSTALL_PREFIX is now mandatory in the cmake configuration.


.. literalinclude:: install.sh
   :language: bash

.. note:: Caution: The compilation of TRIQS, even if run in serial mode, can temporarily
          use up to 4 Gigabytes of RAM. The restrictions on the Login-Nodes of certain
          HPC Machines might not provide sufficient memory for the compilation.
          Please consider compiling within an interactive session on a Compute-Node
          or contact the admins of your HPC Machine.

Environment setup
^^^^^^^^^^^^^^^^^^^

TRIQS provides a small script (`triqsvars.sh`)
to load its installation into your :ref:`environment variables <environment_vars>`.
Please source it with the proper replacement of INSTALL_PREFIX::

        source $INSTALL_PREFIX/share/triqsvars.sh

To automate this process, please add this line to your `~/.bash_profile <https://en.wikipedia.org/wiki/Bash_(Unix_shell)#Startup_scripts>`_
(or `~/.zprofile <http://zsh.sourceforge.net/FAQ/zshfaq03.html#l19>`_)

Further reading
------------------
.. toctree::
   :maxdepth: 1

   installation/install_options
   installation/environment_vars
   installation/python
   installation/clang
