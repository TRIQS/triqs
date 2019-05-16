..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram__constructor:

triqs::statistics::histogram::constructor
=========================================

*#include <triqs/statistics/histogram.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`histogram` (int :param:`a`, int :param:`b`)

    2. | :red:`histogram` (double :param:`a`, double :param:`b`, long :param:`n_bins`)

    3. | :red:`histogram` ()

Documentation



 **1)**   Constructor

   Constructs a histogram over :math:`[a; b]` range with bin length equal to 1.



 **2)**   Constructor

   Constructs a histogram over :math:`[a; b]` range with a given number of bins.



 **3)**   Default constructor





Parameters
^^^^^^^^^^

 * :param:`a` Left end of the sampling range

 * :param:`b` Right end of the sampling range

 * :param:`n_bins` Number of bins