..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram__operator+:

triqs::statistics::histogram::operator+
=======================================

*#include <triqs/statistics.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :ref:`histogram <triqs__statistics__histogram>` :red:`operator+` (:ref:`histogram <triqs__statistics__histogram>` :param:`h1`,
       |            :ref:`histogram <triqs__statistics__histogram>` const & :param:`h2`)





Addition of histograms

   Compute the sum of two histograms over the same range, and with equal numbers of bins.
   This operator will throw if histograms to be added up are incompatible.
   Summation is also performed on the numbers of accumulated and discarded points.





Parameters
^^^^^^^^^^

 * :param:`h1` First histogram to add up

 * :param:`h2` Second histogram to add up


Returns
^^^^^^^

Sum of histograms