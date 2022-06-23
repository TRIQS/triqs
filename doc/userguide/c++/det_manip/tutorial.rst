det_manip: Determinant manipulations for CTQMC
==============================================

.. highlight:: c

.. toctree::
   :maxdepth: 1

TRIQS comes with a class called det_manip to easily perform operations on a special type of matrices
(see :doc:`here <../../reference/determinant_manipulation/contents>`).
This library, among others, allows to easily add or remove lines or columns to the matrix, to calculate the determinant and the inverse.
Here are a couple of simple examples showing the basic use of this class.



Creation of an empty det_manip class
-------------------------------------

.. literalinclude:: det_manip_tutorial_0.cpp
   :language: cpp

Creation of a non empty det_manip class
----------------------------------------

.. literalinclude:: det_manip_tutorial_1.cpp
   :language: cpp

Get information about a det_manip class
-----------------------------------------

.. literalinclude:: det_manip_tutorial_2.cpp
   :language: cpp

Add a line and a column
-------------------------

.. literalinclude:: det_manip_tutorial_3.cpp
    :language: cpp

Remove a line and a column
---------------------------

.. literalinclude:: det_manip_tutorial_4.cpp
    :language: cpp

Add two lines and two columns
------------------------------

.. literalinclude:: det_manip_tutorial_5.cpp
    :language: cpp

Remove two lines and two columns
--------------------------------

.. literalinclude:: det_manip_tutorial_6.cpp
    :language: cpp

Remove/add the one/two last lines and columns
---------------------------------------------

.. literalinclude:: det_manip_tutorial_7.cpp
    :language: cpp

Replace one line and column
---------------------------------------------

.. literalinclude:: det_manip_tutorial_8.cpp
   :language: cpp

Learn more in the full reference, see :ref:`det_manip`
