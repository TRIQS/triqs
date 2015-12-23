..
   Generated automatically using the command :
   c++2doc.py all_triqs.hpp
   /Users/parcolle/triqs/BUILD/triqs/INSTALL_DIR/include/triqs/mc_tools/mc_generic.hpp

.. highlight:: c


.. _mc_generic_add_measure:

add_measure
=============

**Synopsis**:

.. code-block:: c

    template<typename MeasureType>
    void add_measure (MeasureType && m, std::string name) ;

Register a measure

 If the measure m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.

Parameters
-------------

* **M**:                        The measure. Must model Measure concept

* **name**:                     Name of the measure

Return value
--------------

None