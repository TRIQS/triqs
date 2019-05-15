..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _foreach:

foreach
=======


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`foreach for this mesh`
       | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`gf_mesh\<imtime\> <triqs__gfs__gf_meshLTimtimeGT>` const & :param:`m`, Lambda :param:`F`)

    2. | :cppbrief:`foreach for this mesh`
       | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`gf_mesh\<imfreq\> <triqs__gfs__gf_meshLTimfreqGT>` const & :param:`m`, Lambda :param:`F`)

    3. | :cppbrief:`foreach for this mesh`
       | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`segment_mesh <triqs__gfs__segment_mesh>` const & :param:`m`, Lambda :param:`F`)

Documentation





Parameters
^^^^^^^^^^

 * **m**: a mesh

 * **F**: a function of synopsis auto F (matsubara_time_mesh::mesh_point_t)

  Calls F on each point of the mesh, in arbitrary order.