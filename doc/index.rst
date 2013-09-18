.. meta::
   :description: TRIQS: a Toolbox for Research on Interacting Quantum Systems
   :keywords: triqs quantum interacting systems toolbox research

.. _welcome:
  
Welcome
=======

.. sidebar:: TRIQS 1.0

   This is the homepage of the new TRIQS 1.0. Many things
   have changed and been improved since the versions 0.x.
   The format of the archives and names of some python classes
   have changed too. So go look at our :ref:`changelog page <changelog>`
   to find out how to upgrade to 1.0.

TRIQS (**T**\oolbox for **R**\esearch on **I**\nteracting **Q**\uantum **S**\ystems)
is a scientific project providing a set of C++ and Python libraries to develop new tools
for the study of interacting quantum systems. 

The goal of this toolkit is to provide condensed matter physicists with 
some high level, efficient and simple to use libraries in C++ and Python,
and to promote the use of modern programming techniques in our field.

TRIQS is free software (GPL).

TRIQS applications
-----------------------

Based on the TRIQS toolkit, several :ref:`full-fledged applications <applications>`
are also maintained by the TRIQS collaboration. They allow for example to
solve a generic quantum impurity model or to run a complete LDA+DMFT
calculation.

Elaborated in a collaboration between IPhT Saclay and Ecole Polytechnique since 2005, 
the TRIQS library and applications have allowed to address questions as diverse as:

* Momentum-selective aspects on cuprate superconductors (with various cluster DMFT)
* Degree of correlation in iron-based superconductors (within an LDA+DMFT approach)
* Fermionic Mott transition and exploration of Sarma phase in cold-atoms

 
Python & C++
-----------------------------

The libraries exist at two
complementary levels: on the one hand, C++ libraries allow to quickly develop
performant low-level codes; on the other hand python libraries implement the
most common many-body objects, like Green's functions, that can be manipulated
easily in python scripts.

This duality is a real advantage in the development of new many-body tools.
Critical parts where performance is essential can be written in C++ (like a
quantum impurity solver) while the manipulation of the results, preparation of
the inputs or interface with other programs can be done at the very
user-friendly python level.


