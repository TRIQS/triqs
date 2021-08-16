Statistical Analysis: Jackknife
================================



Introduction
------------

This library provides statistical tools to analyze samples that are generated during a simulation. In particular, it provides routines for (a) accumulating and binning observables which are directly sampled a Monte Carlo simulation (b) caclualte average and errors for data and functions of data using jackknife resampling, (c) constructs histograms.

Processing Correlated Data 
---------------------------

In general, the output samples produced in Monte Carlo type simulations are *not* independent of one another, but correlated over a characteristic number of steps -- the correlation time :math:`\tau`. This depends on the simulation and observable.  In order to proceed with conventional statistical analysis, we have to obtain independent samples from the correlated data by binning [LINK]. The central object in the code for this is the :ref:`accumulator class <triqs__stat__accumulator>`. This provides functions to both to estimate :math:`\tau` and to actually bin and store data.

Averages and Standard Errors 
-----------------------------

For independent samples :math:`\lbrace x_i\rbrace _{i=0\dots N-1}` of the elementary observable :math:`X`, estimates of the mean and standard error are:

.. math:: \bar{X} \approx \frac{1}{N} \sum_{i=0}^{N-1} x_{i} \\  (\Delta \bar{X})^2 \approx \frac{N-1}{N} \frac{ \sigma^2(x)}{N}

Functions to do these calculations (including mpi versions), are :ref:`implemented in the library <triqs__stat>`.

Generally we want to computing the mean and error for quantities, derived fromn our measured and binned data. Consider a general funciton :math:`f` that depend on several observables :math:`X,Y,\ldots`:

.. math:: f(\bar{X}, \bar{Y}, \dots)

Unless :math:`f` is linear, calculating a reliable estimate the error :math:`\Delta f` is a difficult problem, which can be addressed using resampling methods. The simplest of these methods is the jackknife [LINK]. We implement the :ref:`jacknife method <jackknife>` (and a :ref:`MPI version <jackknife_mpi>`), which allows for general user-specified functions :math:`f` and input data :math:`X,Y,\ldots`.

TODO: toc of sub modules

TODO: Note that X, Y are generally cross correlated since they come from the same Markov chain



Note: Numerical Stability 
-----------------------------


Accumulator
------------

The accumulator class takes in data obtain during the simulation run has a dual purpose: 

1. to bin this data and store the result according to some preset parameters (so-called "linear binning")
2. to give estimates of the autocorrelation time of the data (so-called "logarithmic binning")

The details of this are described below. The design of the library is purposefully flexible - it is useful both for cases where the auto-correlation time is unknown, for cases where it has already been measured, and cases where this has to be done on the fly. 

.. note:: If feasible from a computational perspective, it is advisable to always run the logarithmic binning, in order to check consistency and diagnose if the autocorrelation time was underestimated.

Linear (Lin) Binning
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The linear binning is responsible for bining and storing the data. It takes in data and accumulates them in a temporary object up to a user-specified size. The data in full full accumulator are averaged and stored in a list of completed bins -- these should give the independent statistical samples, which can be used for further analysis. 

[PICTURE]

The binning is controlled by two parameters:

:bin_size:
    the number of measurements in a full bin and, equivalently, the number of measurements taken by the temporary accumulator. If bin_size == 1, every measurement is considered an independent bin and stored -- no loss of information occurs in this case.

:max_number_bins: 
    The maximum number of bins that can be stored in the list. If more data is put in the accumulator than can fit with the specified max_number_bins and bin_size, then the accumulator will compress [LINK] the data. This guarantees that bins.size() <= max_number_bins. As compression groups together data from the bins, there is a loss of information involved. Special cases:
        * max_n_bins <= -1: no upper limit on the number of bins specified
        * max_n_bins == 0: turns off linear accumulation completely.
        * max_n_bins == 1: everything is accumulated in one bin. Overwrites bin_size from the beginning, since that notion does not make sense in this cases.
    Note that max_n_bins must be even, except for the three special cases above.


Logarithmic (Log) Binning
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The logarithmic binning estimates autocorrelation times of the data, by calculating the standard error of the binned data with different bin sizes. For correlated data, the variance should grow as the bin size increases up to the the autocorrelation time, where the variance saturates [REFERENCES].

The log binning uses bin sizes that are powers of two :math:`{2, 4, 8, 16, \ldots}` up to a maximum :math:`2^{\texttt{n_log_bins}}` set by the user. Binning is performed only if there is more than one full bin of data - any partial accumulation at the end is not considered in the calculation. As more data is added, higher levels of binning automatically start being calculated.

At the end, the log binning returns the list of variances which should be analyzed to see if saturation with bin size has occurred. An estimate of the autocorrelation time is given by [REF]:

FORMULA

for which this is a helper function REF[tau_estimates].

Input parameter: 
:n_log_bins:
    Sets the number of log bins to keep, must be >=0. Special Values:
        * n_log_bins == 0: turns off logarithmic accumulation completely


Entering Data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The accumulator is templated to take general types T. (where the basic elements are floating points). The input type must have the concept [?] of element wise multiplication [LINK BELOW]. (and addition). 

When constructing the accumulator, an object of type T -- so-called "object_instance" -- must be specified. This is needed 

This object is moved to the accumulator and sets the initial values on which the subsequence. In general, the object of type T with zero value(s) should passed as we start with no initial data (however, see REF[Entering Big Data]).

The standard way to add data is through the operator << [Link] to code refence. Consider the simple example.

// Simple Example

Entering Big Data [advanced]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Consider the case, where the construction of an object that we wish to accumulate is particularly intensive. We wish to make a new measurement by directly adding to the accumulator, rather than first creating a new object to be added. A simple example of this is a large matrix where we modify a few elements during each measurement.

For this, the accumulator class has an interface (evaluated lazily), that gives bracket or parentheses access directly to elements of accumulators (lin and log at the same time). Since a measurement can consist of many direct access operations, once these operations are complete we need to manually advance the accumulator to specify that a measurement should be counted.

In such cases, we will also generally specify a non-zero object in the constructor of the accumulator on which further manipulation occurs. [see model]

// Example

TODO: Only try and add things, no other operation ???

Calculating Averages and Variances
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Floating tail.

Raw Averages / Variances

Error and Accuracy during accumulation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Formulae used for avareaging in log binning (for stability):

For stability

Give refereces



Compression
------------
TODO

Note that summation is simple (not compensated). Suggested using higher things if needed (double-double etc.).


Paralleization / MPI Reduction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



Dependent Observables: Jackknife Resampling
------------------------------------------------------------

Concepts
------------------------

element wise multiplication

.. toctree::
   /cpp2rst_generated/triqs/stat/accumulator

