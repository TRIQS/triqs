Manipulation of determinants for Continuous-Time Monte-Carlo algorithms
=======================================================================

.. highlight:: c

.. toctree::
   :maxdepth: 1

TRIQS comes with a class called det_manip to easily perform operations on a special type of matrices 
(see  :doc:`here  <../../reference/c++/det_manip/contents>`). 
This library, among others, allows to easily add or remove lines or columns to the matrix, to calculate the determinant and the inverse. 
Here are a couple of simple examples showing the basic use of this class.



Creation of an empty det_manip class
-------------------------------------

.. triqs_example:: ./det_manip_tutorial_0.cpp
Creation of a non empty det_manip class
----------------------------------------

.. triqs_example:: ./det_manip_tutorial_1.cpp
Get informations about a det_manip class
-----------------------------------------

.. triqs_example:: ./det_manip_tutorial_2.cpp
Add a line and a column
-------------------------

.. triqs_example:: ./det_manip_tutorial_3.cpp
Remove a line and a column
---------------------------

.. triqs_example:: ./det_manip_tutorial_4.cpp
Add two lines and two columns
------------------------------

.. triqs_example:: ./det_manip_tutorial_5.cpp
Remove two lines and two columns
--------------------------------

.. triqs_example:: ./det_manip_tutorial_6.cpp
Learn more in the full reference, see :ref:`det_manip`


