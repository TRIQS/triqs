
.. _changelog:

Changelog
=========

This document describes the main changes in TRIQS.

version >= 1.3
------------

The `ChangeLog file <https://github.com/TRIQS/triqs/blob/778ba8590153db10950a531a73e6b6b42e78bb19/ChangeLog.md>`_ 
is now kept in the source directory.


version 1.2.0
-------------

* New C++/Python wrapper generator.
* HDF5 cleaned, now using only C library (not C++) for simpler installation.

version 1.1.0
-------------

* New constructors for the gf [api change]
* Fix for gf expression templates
* The gf tails now have fixed size to avoid mpi problems
* Fixes in gf expression templates
* New python random generator interface
* Fixes for issues #11, #18, #25

version 1.0.0
-------------

There have been changes from versions 0.x to 1.0.0 that will most likely have
consequences for your scripts and archives.

Python classes
~~~~~~~~~~~~~~

The number of python classes in the old 0.x releases was increasing with no
clear naming convention. In TRIQS 1.0 we have unified the naming of the classes
following the `PEP naming conventions
<http://www.python.org/dev/peps/pep-0008/#naming-conventions>`_:

* Package and module names: lowercase with underscores
* Class names: CapWords
* Function names: lowercase with underscores
* Function and method arguments: lowercase with underscores

Archives
~~~~~~~~

We provide :download:`an update script <scripts/update_archive.py>` which should
help you upgrade your archive. The usage is very simple::

  $ python update_archive.py old_archive new_archive

where ``old_archive`` is your old archive to be upgraded and ``new_archive`` is
the name of the new archive. If you encounter problems please post an
issue with a copy of your archive.
