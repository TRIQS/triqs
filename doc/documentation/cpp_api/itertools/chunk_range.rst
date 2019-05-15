..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _chunk_range:

chunk_range
===========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Given an integer range [start, end), chunk it as equally as possible into n_chunks.`
       | std::pair<std::ptrdiff_t, std::ptrdiff_t> :red:`chunk_range` (std::ptrdiff_t :param:`start`,
       |   std::ptrdiff_t :param:`end`,
       |   long :param:`n_chunks`,
       |   long :param:`rank`)





   If the range is not dividable in n_chunks equal parts, the first chunks have
 one more element than the last ones.