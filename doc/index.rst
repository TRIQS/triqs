.. meta::
   :description: TRIQS: a Toolbox for Research on Interacting Quantum Systems
   :keywords: triqs quantum interacting systems toolbox research

.. _welcome:

Welcome
=======

.. sidebar:: TRIQS 3.3

   This is the homepage TRIQS Version 3.3
   For the changes in 3.3, Cf :ref:`changelog page <changelog>`

      .. image:: _static/logo_github.png
         :width: 75%
         :align: center
         :target: https://github.com/triqs/triqs


TRIQS (**T**\oolbox for **R**\esearch on **I**\nteracting **Q**\uantum **S**\ystems)
is a scientific software library providing various building blocks that facilitate the
implementation and developement of new tools and algorithms for the study of
interacting quantum systems.

TRIQS provides efficient implementations in C++ and exposes its functionality through
high-level interfaces in Python. This paradigm allows our users to quickly develop
new ideas and algorithms, while benefiting from the performance of a compiled language.

TRIQS started in 2005 as a collaboration between IPhT Saclay and Ecole Polytechnique,
and has since seen contributions from a large number of research groups around the world.
The initial publication `Comput. Phys. Commun. 196 398 <https://doi.org/10.1016/j.cpc.2015.04.023>`_
has been cited by more than 300 peer-reviewed publications.

TRIQS is free software distributed under the GPL license.


TRIQS applications
------------------

Within the broader TRIQS ecosystem we provide various :ref:`TRIQS applications<applications>`
that assemble the TRIQS library components to provide full-fleged and cutting-edge implementations
of different many-body algorithms. They allow for example to solve a generic quantum impurity model
or to run a complete DFT+DMFT calculation.

The TRIQS library and applications have enabled researchers to address topics as diverse as:

* Spin-Orbit Coupling and Electronic Correlations in Sr2RuO4, `Phys. Rev. Lett. 120 126401 <https://doi.org/10.1103/PhysRevLett.120.126401>`_
* Late transition metal oxides with infinite-layer structure: Nickelates versus cuprates, `Phys. Rev. B 101 081110 <https://doi.org/10.1103/PhysRevB.101.081110>`_
* Many-Body Electronic Structure of NdNiO2 and CaCuO2 `Phys. Rev. X 10 021061 <https://doi.org/10.1103/PhysRevX.10.021061>`_
* Tracking the Footprints of Spin Fluctuations: A MultiMethod, MultiMessenger Study of the Two-Dimensional Hubbard Model `Phys. Rev. X 11, 011058 <https://doi.org/10.1103/PhysRevX.11.011058>`_
* Linear resistivity and Sachdev-Ye-Kitaev (SYK) spin liquid behavior in a quantum critical metal with spin-1/2 fermions `Proc. Natl. Acad. Sci. USA 117, 18341 <https://doi.org/10.1073/pnas.2003179117>`_

Various other TRIQS related research publications can be found through a targeted `Google Scholar search <https://scholar.google.com/scholar?cites=11179891035589079889&as_sdt=5,33&sciodt=0,33&hl=en>`_.


Core libraries
--------------

The TRIQS library is built on a set of stand-alone core libraries providing important functionalities:

* `TRIQS/h5 <https://github.com/triqs/h5>`_ : a high-level C++ interface to the hdf5 library
* `TRIQS/mpi <https://github.com/triqs/mpi>`_ : a high-level C++ interface to the Message Passing Interface
* `TRIQS/itertools <https://github.com/triqs/itertools>`_ : a C++ library to create and adapt C++ ranges
* `TRIQS/nda <https://github.com/triqs/nda>`_ : a C++ library providing an efficient and flexible multi-dimensional array class


.. _numerical_algorithms:
Numerical Algorithms
--------------------

TRIQS provides implementations of advanced numerical algorithms that are directly relevant to
the study of interacting quantum systems, for example:

* Orthogonal polynomial representation of imaginary-time Green’s functions `Phys. Rev. B 84 075145 <https://doi.org/10.1103/PhysRevB.84.075145>`_

* Discrete Lehmann representation of imaginary time Green's functions `Phys. Rev. B 105 235115 <https://doi.org/10.1103/PhysRevB.105.235115>`_
  through the cppdlr library `J. Open Source Softw. 9(100) 6297 <https://doi.org/10.21105/joss.06297>`_

* Constrained residual minimization Dyson Solver `Phys. Rev. B 111 115155 <https://doi.org/10.1103/PhysRevB.111.115155>`_

Please cite the relevant papers when using these algorithms in your research.


TRIQS Community
---------------

Connect with fellow users and developers through our `TRIQS Slack Workspace <https://triqs.github.io/slack>`_,
where you can share insights, ask questions, and get involved in the development of TRIQS and applications.
We also provide information about new software releases and events such as summer schools and our annual
TRIQS Community Meeting. We look forward to welcoming you to our community!


Where to start?
---------------

First, visit our :ref:`Install <triqs_install>` section to obtain the TRIQS library.

We then strongly recommend first-time users to go through the extensive set of Jupyter Notebook tutorials provided on `GitHub <https://github.com/TRIQS/tutorials>`_.
You can run the notebooks either in an interactive `Binder Session <https://triqs.github.io/notebook>`_ on the web
or run them locally after setting up TRIQS on your computer.

To get an overview of the main TRIQS classes and their organizations, you can browse through the ready-to-use code snippets of our
:ref:`User guide <triqs_userguide>` section.

After those steps, you will be familiar with the library and can use the complete :ref:`Documentation <documentation>` to answer your questions.

If you experience any problem with the library, we have set up
a platform for discussions with other users and developers through the `Github Discussions <https://github.com/TRIQS/triqs/discussions>`_ forum.
You can also post an issue on the `TRIQS GitHub <https://github.com/TRIQS/triqs>`_.

Finally, if you want to contribute to the project, please check our :ref:`Contributing <triqs_contributing>` section.

.. raw:: html

   <style>
       .img {
           margin-right: 2%;
       }
   </style>

.. image:: _static/logo_simons.png
   :width: 19.0%
   :target: https://www.simonsfoundation.org
   :class: img

.. image:: _static/logo_ccq.png
   :width: 32.6%
   :target: https://www.simonsfoundation.org/flatiron/center-for-computational-quantum-physics/
   :class: img

.. image:: _static/logo_cdf.png
   :width: 4.7%
   :target: https://www.college-de-france.fr
   :class: img

.. image:: _static/logo_cea.png
   :width: 7.7%
   :target: http://ipht.cea.fr
   :class: img

.. image:: _static/logo_x.png
   :width: 4.6%
   :target: "https://www.cpht.polytechnique.fr
   :class: img

.. image:: _static/logo_cnrs.png
   :width: 6.3%
   :target: https://www.cnrs.fr
   :class: img

.. image:: _static/logo_erc.jpg
   :width: 6.6%
