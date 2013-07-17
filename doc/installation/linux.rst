.. index:: Linux

.. _Linux:

.. highlight:: bash

Installation on a generic Linux distribution  
#######################################################

TRIQS has been installed successfully on several distributions.

#. Download the code (this is for the current development version) ::

     git clone git://github.com/TRIQS/TRIQS.git path_to_triqs_sources

#. Download the `sources of boost <http://ipht.cea.fr/triqs/download/boost_src.tar.bz2>`_ and untar them into ``path_to_boost_sources``.
   **Don't compile boost**.

#. Create a build directory (different from the TRIQS source directory nor a subdirectory thereof) ::
    
    mkdir triqs_build && cd triqs_build 
 
#. Configure ::

    cmake path_to_triqs_sources -DBOOST_SOURCE_DIR=path_to_boost_sources \
          -DCMAKE_INSTALL_PREFIX=path_to_install_directory

   [NB : the install directory is optional, the default value is `triqs_build/INSTALL_DIR`]
 
   Successful run of cmake will finish with ::

     -- Build files have been written to: *path_to_triqs_build*

#. Build TRIQS, test it and install it with (N being the number of cores of your machine) ::
  
    make -jN   && make test && make -jN install 
 
   [NB : the tests SingleSite and CDMFT-v2  take longer than the other ones (30s to 1 min) ].

#. If you use Wien2TRIQS, please complete the installation as described :ref:`here <wien2k_inst>`.

#. You can now :ref:`start ... <get_started>`

#. Optionally, you can be interested in : 

 * How to *simply* upgrade the ipython notebook with :ref:`virtualenv <virtualenv>` to the latest version ?

 * How to use the nice :ref:`clang <clang>` C++ compiler ?

 * How to use :ref:`Intel <icc>` C++ compiler ?

