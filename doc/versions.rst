
.. _versions:

Version compatibility
=====================

TRIQS version numbering
-----------------------

The TRIQS library has three-digit *release* numbers (e.g. 1.0.5) and two-digit
*version* numbers (e.g. 1.0). The first two digits of the release number always
correspond to the version number.

New releases within the same version correspond to bug fixes and improvements
that do not change the API of the library. In other words, applications based
on the TRIQS library do not need to be changed between such releases of the
TRIQS library.  Instead, a modification of the version number, say from 1.0 to
1.1, indicates deeper modifications of the library with possible breaks of the
API. In that case, the source codes of the applications might need to be
modified.

TRIQS applications version numbering
------------------------------------

In order to easily identify which versions of an application are compatible
with given versions of TRIQS, the applications follow a similar version
numbering: three-digit release numbers and two-digit version numbers.  The
compatibility rule is then simply that *an application is compatible with the
TRIQS library if it has the same version number*.

How to figure the version of the installed TRIQS library
--------------------------------------------------------

In order to figure the version of an installed TRIQS library, you can
either:

* Read the content of the ``version.hpp`` file which is in the
  ``path_to_install_directory/include/triqs/`` diretory.

* Run ``pytriqs`` and type the following commands::

    from pytriqs.version import *
    show_version()

