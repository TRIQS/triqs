.. _triqs_userguide:

User guide
==========

Welcome to the TRIQS project!

This guide presents the main classes of TRIQS through ready-to-use examples in Python. These small
demo codes should help you understand the logic behind the general design and organization of the
library.

.. note::

     We strongly recommend first-time users to go through the extensive set of Jupyter Notebook tutorials provided on `GitHub <https://github.com/TRIQS/tutorials>`_.
     You can run the notebooks either in an interactive `Binder Session <https://triqs.github.io/notebook>`_ on the web
     or run them locally after setting up TRIQS on your computer.

.. note::

     This guide does not aim to replace the full :ref:`API Documentation <documentation>`.
     The C++ library together with its examples is documented separately in the
     `C++ API <./doxygen/index.html>`_.


Why use TRIQS?
--------------

TRIQS is a powerful library that allows you to easily write code to study strongly correlated problems.
It is designed as a toolkit containing all the essential ingredients to tackle condensed matter physics.
Before presenting the tools available both in C++ and Python, we want you to have a look at a small script
where a full Dynamical Mean-Field Theory calculation using the Continuous-Time Hybridization Expansion (CT-HYB) solver
is realized in... one page!

.. toctree::
   :maxdepth: 1

   userguide/python/dmft_one_page


Python Tutorials
----------------

We provide here different Tutorial series, with increasing difficulty, that introduce you to various aspects of the TRIQS Python interface. These Tutorials are also provided as Jupyter Notebooks in our `TRIQS/tutorials <https://github.com/TRIQS/tutorials>`_ repository.

.. toctree::
   :maxdepth: 2

   userguide/python/basics
   userguide/python/model_dmft
   userguide/python/two_particle_response
   userguide/python/tight_binding

If you want to learn more about Python, iPython and the notebook, you can check our external resources:

.. toctree::
   :maxdepth: 1

   userguide/python/external_resources

For the documentation of individual classes and functions — together with
ready-to-use code examples — see the :ref:`Python API <documentation>`.
