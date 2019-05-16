..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__constructor:

triqs::det_manip::det_manip::constructor
========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`det_manip` (FunctionType :param:`F`, size_t :param:`init_size`)

    2. | :green:`template<typename ArgumentContainer1, typename ArgumentContainer2>`
       | :red:`det_manip` (FunctionType :param:`F`, ArgumentContainer1 const & :param:`X`, ArgumentContainer2 const & :param:`Y`)

    3. | :red:`det_manip` (det_manip<FunctionType> const & )

    4. | :red:`det_manip` (det_manip<FunctionType> && :param:`rhs`) noexcept

Documentation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`ArgumentContainer`


Parameters
^^^^^^^^^^

 * :param:`F`         The function (NB : a copy is made of the F object in this class).

 * :param:`init_size` The maximum size of the matrix before a resize (like reserve in std::vector).
                  Like std::vector, resize is automatic (by a factor 2) but can yield a performance penalty
                  if it happens too often.

 * :param:`X,` Y : container for X,Y.