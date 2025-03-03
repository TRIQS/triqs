.. index:: installation

.. highlight:: bash

.. _triqs_install:

############
Installation
############

TRIQS and its applications are provided *a la carte*:
after you have installed the TRIQS library, you will be able to easily install
various TRIQS-based applications: impurity solvers, tools for DFT+DMFT
calculations, etc.

This page describes the installation of the TRIQS library itself. The
installation procedure of the applications is described on their respective
sites, under 'Install'.

We provide pre-compiled versions of triqs and applications of each stable release.
If however you would like to try the latest development version you can use the
`unstable` branch instead and compile triqs manually. This will make the latest library
improvements and new features available to you. Note however that the development versions may include
changes of the API. We use `continuous integration <https://jenkins.flatironinstitute.org/job/TRIQS/>`_
to ensure that both the unstable and release branches compile and pass all tests.
This is checked for both the TRIQS library and several public (and private) applications.

.. note:: To guarantee reproducibility in scientific calculations we strongly recommend the use of a stable `release <https://github.com/TRIQS/triqs/releases>`_ of both TRIQS and its applications.


Packaged Versions of TRIQS
==========================

We provide various packages for the TRIQS library and some of the major applications.
Note that the packages are compiled for generic x86 CPU's and do not provide optimal performance
on modern CPU's. For optimal performance please consider using :ref:`easybuild <triqs_easybuild>`
or :ref:`compilation from source <triqs_compilation>` and be sure to enable `architecture-specific optimizations <https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html>`_.

.. _ubuntu_debian:

Ubuntu Debian packages
----------------------

We provide Debian packages for the Ubuntu LTS Version **22.04** (jammy) and **24.04** (noble).
They are hosted in a **TRIQS 3 specific** repository that can be added with::

        sudo apt-get update && sudo apt-get install -y software-properties-common apt-transport-https gpg-agent curl
        source /etc/lsb-release
        curl -L https://users.flatironinstitute.org/~ccq/triqs3/$DISTRIB_CODENAME/public.gpg | sudo apt-key add -
        sudo add-apt-repository "deb https://users.flatironinstitute.org/~ccq/triqs3/$DISTRIB_CODENAME/ /"

To install the required runtime packages and triqs into the system tree at /usr run::

        sudo apt-get update && sudo apt-get install -y triqs

If you aim to compile applications against the triqs library, additional :ref:`development libraries <ubuntu>` have to be installed. You should further set the following :ref:`environment variable <environment_vars>` permanently::

        export CPLUS_INCLUDE_PATH=/usr/include/openmpi:/usr/include/hdf5/serial/:$CPLUS_INCLUDE_PATH

For full functionality of the C++2Py Tool, please install clang and the libclang library::

        sudo apt-get install clang libclang-dev


.. _triqs_anaconda:

Anaconda
--------

We provide Linux and OSX packages for the `Anaconda <https://www.anaconda.com/>`_ distribution. The packages are provided through the `conda-forge <https://conda-forge.org/>`_ repositories. After `installing conda <https://docs.conda.io/en/latest/miniconda.html>`_ we recommend to first create a specific `Anaconda environment <https://conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html>`_ to avoid conflicts with other packages::

        conda create -n triqs
        conda activate triqs

You can then install TRIQS with::

        conda install -c conda-forge triqs

See also `github.com/conda-forge/triqs-feedstock <https://github.com/conda-forge/triqs-feedstock/>`_.
We further provide packages for `triqs_cthyb <https://github.com/conda-forge/triqs_cthyb-feedstock/>`_, `triqs_dft_tools <https://github.com/conda-forge/triqs_dft_tools-feedstock/>`_ and `triqs_tprf <https://github.com/conda-forge/triqs_tprf-feedstock/>`_.

In case you encounter troubles resolving the dependencies, you can try using `mamba <https://github.com/mamba-org/mamba>`_ instead of `conda`::

        conda install -c conda-forge mamba
        mamba install -c conda-forge triqs

.. note:: We do not recommend using Anaconda for the development of TRIQS applications. Please use the :ref:`compilation from source <triqs_compilation>` instead.

.. _triqs_docker:

Docker
------

The virtualization software `docker <https://docs.docker.com/install>`_  can be used to
run a TRIQS environment for both Jupyter Notebook and shell access on a variety of operating
systems. Once docker is installed, just pull the latest image with::

        docker pull flatironinstitute/triqs

and follow the commands on the `image-website <https://hub.docker.com/r/flatironinstitute/triqs>`_.

.. _triqs_apptainer:

Apptainer
---------

The virtualization software `apptainer <https://apptainer.org/docs/user/latest/>`_ allows for easy set-up on cluster machines.
Ask your cluster administator to install the apptainer software. The TRIQS setup is then as simple as::

        apptainer pull docker://flatironinstitute/triqs

which will generate an image file `triqs_latest.sif`. You can then execute commands inside this image file, e.g.::

        apptainer exec triqs_latest.sif python3 -c "from triqs.gf import *"

In order to run your code in parallel you will need to use Open MPI version 4.0.3 to match the version of the apptainer image.
You can then run any command in parallel, e.g.::

        mpirun -np 2 apptainer exec triqs_latest.sif python3 -c "from triqs.utility import mpi; print(mpi.rank)"

For more information on the use of apptainer in conjunction with docker images please refer to the `documentation <https://apptainer.org/docs/user/latest/docker_and_oci.html>`_.

.. _triqs_binder:

Binder
------

`Binder <https://mybinder.org>`_ is a web-service running Jupyter notebooks for direct access.
A TRIQS Jupyter notebook environment can be accessed directly at `triqs.github.io/notebook <https://triqs.github.io/notebook>`_.

.. note:: Once your binder session is closed, your work will be lost.


.. _triqs_easybuild:

Compiling TRIQS from source using EasyBuild
===========================================

`EasyBuild <https://easybuild.readthedocs.io/en/latest/>`_ is a build and installation framework for managing scientific software on HPC systems.
We here assume that a working copy of EasyBuild is already available (see `installation guide <https://easybuild.readthedocs.io/en/latest/Installation.html>`_ otherwise). TRIQS 3.0.0 is available starting from EasyBuild version 4.3.1.
To bring EasyBuild to the latest release (including the latest definitions of all software installable via EasyBuild), type::

      eb --install-latest-eb-release

To search for the available TRIQS packages, type::

      eb -S TRIQS

To install TRIQS or a TRIQS applications, type (for example)::

      eb --robot TRIQS-3.0.0-foss-2020a-Python-3.8.2.eb
      eb --robot TRIQS-cthyb-3.0.0-foss-2020a-Python-3.8.2.eb

This will fetch, compile and install the requested package, as well as all required dependencies (including toolchains, Python, various libraries). Corresponding environment modules will also be generated, thus a package can be loaded using (for example)::

      module load TRIQS-cthyb/3.0.0-foss-2020a-Python-3.8.2

or simply::

      module load TRIQS-cthyb

for the most recent version.


.. _triqs_compilation:

Compiling TRIQS from source (Advanced)
======================================

Prerequisites
-------------

The TRIQS library relies on a certain number of standard libraries and tools
described in the :ref:`list of requirements <requirements>`. Please pay
particular attention to the :ref:`C++ compilers<require_cxx_compilers>` and to
:ref:`Python virtual environments<python_virtualenv>`.  We provide explicit
version requirements and instructions on how to install these necessary libraries
on two standard systems:

.. toctree::
   :maxdepth: 1

   installation/requirements
   installation/ubuntu
   installation/osx_install

.. note:: Please ensure that you have all the dependencies installed before proceeding further!

Installation steps
------------------

Using the command line
^^^^^^^^^^^^^^^^^^^^^^

We provide hereafter the build instructions for the TRIQS library.
First export `INSTALL_PREFIX` for the installation location, and `NCORES` for the number of cores to use during compilation, for example::

    export INSTALL_PREFIX=$(pwd)/install
    export NCORES=4

Then clone the TRIQS repository and create a build directory::

    git clone https://github.com/TRIQS/triqs
    cd triqs && mkdir -p build && cd build

Use cmake to configure the TRIQS build process::

    cmake ../ -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX

Note, that we have to provide cmake with the flag `CMAKE_INSTALL_PREFIX` to define the installation directory for TRIQS and all its applications. If you are using a :ref:`Python virtual environment<python_virtualenv>`, make sure that it is activated. Finally, build, test and install TRIQS::

    make -j$NCORES && ctest -j$NCORES test && make install

.. note:: Caution: The compilation of TRIQS, even if run in serial mode, can temporarily
          use up to 4 Gigabytes of RAM. The restrictions on the Login-Nodes of certain
          HPC Machines might not provide sufficient memory for the compilation.
          Please consider compiling within an interactive session on a Compute-Node
          or contact the admins of your HPC Machine.

Using the provided Python script
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To install TRIQS together with several of its :ref:`applications<applications>` we provide a Python script that
automatizes the clone, build, install and test process. It is located in the `packaging <https://github.com/TRIQS/triqs/tree/3.3.x/packaging>`_
directory of the TRIQS repository and supports the installation of stable release branches as well as unstable
development branches:

* To get the Python script for the latest stable release, go to `<https://github.com/TRIQS/triqs/blob/3.3.x/packaging/clone_archive_and_install.py>`_
  and click on ``Download raw file``.

* To get the Python script for the unstable development branch, go to `<https://github.com/TRIQS/triqs/blob/unstable/packaging/clone_archive_and_install.py>`_
  and click on ``Download raw file``.

The script can be used in different ways. For example,

* Install TRIQS and its applications locally into the default directory `./triqs_repositories/install`::

    python clone_archive_and_install.py -c -i -j $NCORES

  Here, ``$NCORES`` is the number of cores to use for the build process. If it is not set in your current environment,
  you should replace it with a specific number.

* Clone and archive TRIQS and all applications into a tarball `triqs_repositories.tar.gz`::

    python clone_archive_and_install.py -c -a

  This tarball can be used to perform an offline installation of TRIQS. Simply copy the tarball to the machine where you
  want to do the installation and run::

    tar -xzf triqs_repositories.tar.gz
    python triqs_repositories/clone_archive_and_install.py -i -j $NCORES

* Only install a specific application, e.g. `TRIQS/cthyb <https://github.com/TRIQS/cthyb>`_::

    python clone_archive_and_install.py -i triqs cthyb -j $NCORES

For more information, please take a look at the help message of the script::

    python clone_archive_and_install.py -h

.. note:: The script assumes that all dependencies / required libraries are already installed.

Environment setup
-----------------

TRIQS provides a small script (`triqsvars.sh`)
to load its installation into your :ref:`environment variables <environment_vars>`.
Please source it with the proper replacement of INSTALL_PREFIX::

        source $INSTALL_PREFIX/share/triqs/triqsvars.sh

To automate this process, please add this line to your `~/.bash_profile <https://en.wikipedia.org/wiki/Bash_(Unix_shell)#Startup_scripts>`_
(or `~/.zprofile <http://zsh.sourceforge.net/FAQ/zshfaq03.html#l19>`_)

We also provide a module file for the `Lmod <https://lmod.readthedocs.io/en/latest/>`_ environment module system in the top build dir called `triqs.modulefile`.

Further reading
---------------
.. toctree::
   :maxdepth: 1

   installation/install_options
   installation/environment_vars
   installation/python_virtualenv
   installation/clang
