..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _histogram_constructor:

triqs::statistics::histogram::constructor
=========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Constructor`
       | :red:`histogram` (int :param:`a`, int :param:`b`)

    2. | :cppbrief:`Constructor`
       | :red:`histogram` (double :param:`a`, double :param:`b`, long :param:`n_bins`)

    3. | :cppbrief:`Default constructor`
       | :red:`histogram` ()

Documentation



 **1)**
    Constructs a histogram over :math:`[a; b]` range with bin length equal to 1.



 **2)**
    Constructs a histogram over :math:`[a; b]` range with a given number of bins.





Parameters
^^^^^^^^^^

 * **a**: Left end of the sampling range

 * **b**: Right end of the sampling range

 * **n_bins**: Number of bins