..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__chunk_range:

itertools::chunk_range
======================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | std::pair<std::ptrdiff_t, std::ptrdiff_t> :red:`chunk_range` (std::ptrdiff_t :param:`start`,
       |              std::ptrdiff_t :param:`end`,
       |              long :param:`n_chunks`,
       |              long :param:`rank`)





Given an integer range [start, end), chunk it as equally as possible into n_chunks.
If the range is not dividable in n_chunks equal parts, the first chunks have
one more element than the last ones.