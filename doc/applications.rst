.. _applications:

**Applications based on TRIQS**
===============================

The TRIQS collaboration not only works at the elaboration of libraries to
construct new tools for the study of quantum systems. Some full-fledged
applications have readily been developed and are maintained by the TRIQS
collaboration.

**Hybridization-expansion solver**
----------------------------------

The hybridization-expansion solver allows to solve the generic problem
of a quantum impurity embedded in a conduction bath. It is based on a quantum
Monte Carlo algorithm that stochastically samples the diagrams of a
hybridization expansion of the partition function.

*Homepage:* `<https://triqs.github.io/cthyb/>`_

**DFT tools**
-------------

DFT tools provides an interface to DFT packages such as Wien2k for LDA+DMFT
calculation. It allows you to turn band-structure calculations obtained from
the Wien2k package to inputs to full-fledged LDA+DMFT calculations in a few
lines!

*Homepage:* `<https://triqs.github.io/dft_tools/>`_

**Hubbard I solver**
--------------------
.. warning::

   THIS APPLICATION CURRENTLY ONLY COMPATIBLE WITH TRIQS 1.4

This solver gives the Hubbard I solution of a quantum impurity problem,
i.e. it solves the atomic limit. This simple approximation allows for
very quick answers even for large multiband systems.

*Homepage:* `<https://triqs.github.io/triqs/1.4/applications/hubbardI/>`_

**Applications for Analytic Continuation**
------------------------------------------

Maxent
~~~~~~

This TRIQS application provides a modular Maximum Entropy
program to perform analytic continuation.

*Homepage:* `<https://triqs.github.io/maxent/>`_

Omegamaxent Interface
~~~~~~~~~~~~~~~~~~~~~

This TRIQS application is an interface to the `OmegaMaxent <https://www.physique.usherbrooke.ca/MaxEnt/index.php/Main_Page>`_
Maximum Entropy code.

*Homepage:* `<https://triqs.github.io/omegamaxent_interface/>`_

SOM
~~~

This TRIQS application is an optimized implementation of an analytic
continuation method proposed by Andrey S. Mishchenko.
A detailed description of the method can be found `here <http://www.cond-mat.de/events/correl12/manuscripts/mishchenko.pdf>`_.

*Homepage:* `<http://krivenko.github.io/som/>`_
