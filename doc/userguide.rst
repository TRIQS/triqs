.. _triqs_userguide:

.. warning::

     This part of the documentation is currently being rewritten

User guide
==========

Welcome to the TRIQS project!

This guide aims at presenting you the main classes of TRIQS and its applications through ready-to-use examples. These small demo codes
should help you understand the logic behind the general design and organization of the library.

.. note::

     We strongly recommend first-time users to go through the extensive set of Jupyter Notebook tutorials provided on `GitHub <https://github.com/TRIQS/tutorials>`_.
     You can run the notebooks either in an interactive `Binder Session <https://triqs.github.io/notebook>`_ on the web
     or run them locally after setting up TRIQS on your computer.

.. warning::

     This pages does not aim to replace the full reference of the code available :ref:`here <documentation>`.


Why use TRIQS?
--------------

TRIQS is a powerful library that allows you to easily write code to study strongly correlated problems.
It is designed as a toolkit containing all the essential ingredients to tackle condensed matter physics.
Before presenting you the tools available both in C++ and Python, we want you to have a look at a small script 
where a full Dynamical Mean-Field Theory calculation using the Continuous-Time Hybridization Expansion (CT-HYB) solver 
is realized in... one page!

.. toctree::
   :maxdepth: 1

   userguide/python/dmft_one_page


Python Tutorials
----------------

We provide here three different Tutorial series, with increasing difficulty, that introduce you to various aspects of the TRIQS Python interface. These Tutorials are also provided as Jupyter Notebooks in our `TRIQS/tutorials <https://github.com/TRIQS/tutorials>`_ repository.

.. toctree::
   :maxdepth: 2

   userguide/python/basics
   userguide/python/model_dmft
   userguide/python/two_particle_response

If you want to learn more about Python, iPython and the notebook, you can check our external resources:

.. toctree::
   :maxdepth: 1

   userguide/python/external_resources

A quick tour in C++
-------------------

We here provide a little tour of some aspects of TRIQS and its applications, at the C++
level.  

.. toctree::
   :maxdepth: 1

   userguide/c++/gfs/tutorial
   userguide/c++/arrays/tutorial
   userguide/c++/det_manip/tutorial
   userguide/c++/hdf5/tutorial


.. toctree::
   :maxdepth: 1

   userguide/c++/using_the_lib/contents
