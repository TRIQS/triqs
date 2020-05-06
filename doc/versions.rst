
.. _versions:

Version compatibility
=====================

TRIQS version numbering
-----------------------

The TRIQS library has three-digit *version* numbers (e.g. 2.1.1) consisting
of a Major, Minor and Patch version number.

The Patch Version Number denotes bug fixes and improvements that do not change the API of the library .
In other words, applications based on the TRIQS library do not need to be changed 
between TRIQS library versions with identical Major and Minor version number (e.g. 2.1.0 and 2.1.3). 

The Minor version number indicates deeper modification of the library with possible
breaks of the API. These changes will often include new or adjusted library features.
Applications may have to be adjusted accordingly.

The Major version number indicates a profound modification of the library.
The changes can include major adjustments in the API.
Most Applications will have to be ported to the new Major library versions.

For a summary of changes in the TRIQS library refer to the `Changelog <https://triqs.github.io/triqs/master/changelog.html>`_

TRIQS applications version numbering
------------------------------------

In order to easily identify which versions of an application are compatible
with given versions of TRIQS, the applications follow a similar version
numbering: three-digit release numbers and two-digit version numbers.  The
compatibility rule is then simply that *an application is compatible with the
TRIQS library if it has the same Major and Minor version number*.

How to determine the version of the installed TRIQS library
------------------------------------------------------------

In order to determine the version of an installed TRIQS library, you can
either:

* Read the content of the ``version.hpp`` file which is in the
  ``path_to_install_directory/include/triqs/`` diretory.

* Run ``triqs`` and type the following commands::

    from triqs.version import *
    show_version()
