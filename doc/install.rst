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

.. _ubuntu_debian:
Ubuntu Debian packages
----------------------

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

If you aim to compile applications against the triqs library, additional :ref:`development libraries <ubuntu>` have to be installed. You should further set the following :ref:`environment variable <environment_vars>` permanently::

        export CPLUS_INCLUDE_PATH=/usr/include/openmpi:/usr/include/hdf5/serial/:$CPLUS_INCLUDE_PATH

For full c++2py functionality, please read the corresponding :ref:`section <ubuntu_cpp2py>` for the Ubuntu set-up.

.. _anaconda:
Anaconda (experimental)
-----------------------

We provide Linux and OSX packages for the `Anaconda <https://www.anaconda.com/>`_ distribution. The packages are provided through the `conda-forge <https://conda-forge.org/>`_ repositories. After `installing conda <https://docs.conda.io/en/latest/miniconda.html>`_ you can install TRIQS with::

        conda install -c conda-forge triqs

See also `github.com/conda-forge/triqs-feedstock <https://github.com/conda-forge/triqs-feedstock/>`_.
We further provide packages for `triqs_cthyb <https://github.com/conda-forge/triqs_cthyb-feedstock/>`_, `triqs_dft_tools <https://github.com/conda-forge/triqs_dft_tools-feedstock/>`_
and `triqs_tprf <https://github.com/conda-forge/triqs_tprf-feedstock/>`_.

.. _triqs_docker:
Docker
------

The virtualization software `docker <https://docs.docker.com/install>`_  can be used to
run a TRIQS environment for both Jupyter Notebook and shell access on a variety of operating
systems. Once docker is installed, just pull the latest image with::

        docker pull flatironinstitute/triqs

and follow the commands on the `image-website <https://hub.docker.com/r/flatironinstitute/triqs>`_.

.. _triqs_singularity:
Singularity
-----------

The virtualization software `singularity <https://www.sylabs.io/guides/latest/user-guide/>`_ allows for easy set-up on cluster machines.
Ask your cluster administator to install the singularity software. The TRIQS setup is then as simple as::

        singularity pull docker://flatironinstitute/triqs

which will generate an image file `triqs_latest.sif`. You can then execute commands inside this image file, e.g.::

        singularity exec triqs_latest.sif python -c "from triqs import *"

In order to run your code in parallel you will need to use Open MPI version 2.1.1 to match the version of the singularity image.
You can then run any command in parallel, e.g.::

        mpirun -np 2 singularity exec triqs_latest.sif python -c "from triqs.utility import mpi; print mpi.rank"

For more information on the use of singularity in conjunction with docker images please refer to the `documentation <https://www.sylabs.io/guides/latest/user-guide/singularity_and_docker.html>`_.

Binder
------

`Binder <https://mybinder.org>`_ is a web-service running Jupyter notebooks for direct access.
A TRIQS Jupyter notebook environment can be accessed directly at `triqs.github.io/notebook <https://triqs.github.io/notebook>`_.

.. note:: Once your binder session is closed, your work will be lost.


.. _triqs_easybuild:
Compiling TRIQS from source using EasyBuild
===========================================

`EasyBuild <https://easybuild.readthedocs.io/en/latest/>` is a build and installation framework for managing scientific software on HPC systems.
We here assume that a working copy of EasyBuild is already available (see `installation guide <https://easybuild.readthedocs.io/en/latest/Installation.html>` otherwise).
To bring EasyBuild to the latest release (including the latest definitions of all software installable via EasyBuild), type

      eb --install-latest-eb-release
      
To search for the available TRIQS packages, type

      eb -S TRIQS
      
To install TRIQS or a TRIQS applications, type (for example)

      eb --robot TRIQS-3.0.0-foss-2019a-Python-3.7.2.eb
      eb --robot TRIQS-cthyb-3.0.0-foss-2019a-Python-3.7.2.eb
     
This will fetch, compile and install the requested package, as well as all required dependencies (including toolchains, Python, various libraries). Corresponding environment modules will also be generated, thus a package can be loaded using (for example)

      module load TRIQS-cthyb/3.0.0-foss-2019a-Python-3.7.2
      
or simply

      module load TRIQS-cthyb
      
for the most recent version.    


.. _compilation_from_source:
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
^^^^^^^^^^^^^^^^^

TRIQS provides a small script (`triqsvars.sh`)
to load its installation into your :ref:`environment variables <environment_vars>`.
Please source it with the proper replacement of INSTALL_PREFIX::

        source $INSTALL_PREFIX/share/triqsvars.sh

To automate this process, please add this line to your `~/.bash_profile <https://en.wikipedia.org/wiki/Bash_(Unix_shell)#Startup_scripts>`_
(or `~/.zprofile <http://zsh.sourceforge.net/FAQ/zshfaq03.html#l19>`_)

Further reading
---------------
.. toctree::
   :maxdepth: 1

   installation/install_options
   installation/environment_vars
   installation/python
   installation/clang
