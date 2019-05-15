..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _histogram_operator+:

triqs::statistics::histogram::operator+
=======================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Addition of histograms`
       | :ref:`histogram <triqs__statistics__histogram>` :red:`operator+` (:ref:`histogram <triqs__statistics__histogram>` :param:`h1`,
       |   :ref:`histogram <triqs__statistics__histogram>` const & :param:`h2`)






    Compute the sum of two histograms over the same range, and with equal numbers of bins.
    This operator will throw if histograms to be added up are incompatible.
    Summation is also performed on the numbers of accumulated and discarded points.





Parameters
^^^^^^^^^^

 * **h1**: First histogram to add up

 * **h2**: Second histogram to add up


Returns
^^^^^^^

 * Sum of histograms