..
   Generated automatically by cpp2rst

.. highlight:: c


.. _histogram_constructor:

constructor
===========

**Synopsis**:

.. code-block:: c

    void histogram (int a, int b)                        (1)

    void histogram (double a, double b, long n_bins)     (2)

    void histogram ()                                    (3)

(1)Constructor


Parameters
----------

 * **a**: Left end of the sampling range

 * **b**: Right end of the sampling range


Documentation
-------------


    Constructs a histogram over :math:`[a; b]` range with bin length equal to 1.



(2)Constructor


Parameters
----------

 * **a**: Left end of the sampling range

 * **b**: Right end of the sampling range

 * **n_bins**: Number of bins


Documentation
-------------


    Constructs a histogram over :math:`[a; b]` range with a given number of bins.



(3)Default constructor