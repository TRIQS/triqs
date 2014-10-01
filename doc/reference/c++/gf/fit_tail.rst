Fitting the tail of a Matsubara Green's function
#################################################

API
~~~~~

The tail of a given ``gf<imfreq>/gf<block_index, gf<imfreq>> gw`` can be fitted using the two following functions:

 ``void fit_tail(gf<imfreq> &gf, tail_view known_moments, int n_moments, size_t n_min, size_t n_max, bool replace_by_fit = false);``

 ``void fit_tail(gf<block_index, gf<imfreq>> &block_gf, tail_view known_moments, int n_moments, size_t n_min,  size_t n_max, bool replace_by_fit = false);``


where

+------------+----------------+----------------------------------------------------------------------+---------+
| type       | name           | description                                                          | default |
+============+================+======================================================================+=========+
| gf<imfreq> | gf             | Green's function to be fit (bosonic/fermionic)                       | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| tail_view  | known_moments  | known part of the tail                                               | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| int        | n_moments      | number of moments in the final tail (including known ones)           | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| size_t     | n_min          | Matsubara index on mesh to start the fit                             | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| size_t     | n_max          | final Matsubara index for fit (included)                             | no      |
+------------+----------------+----------------------------------------------------------------------+---------+
| bool       | replace_by_fit | if true, replace the gf data in the fitting range by the tail values | true    |
+------------+----------------+----------------------------------------------------------------------+---------+


Example
~~~~~~~~

.. triqs_example:: ./fit_tail_0.cpp
Implementation
~~~~~~~~~~~~~~~

The fitting problem is written as a linear system of equations, which is solved by SVD.

