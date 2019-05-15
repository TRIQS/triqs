..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_gf:

make_gf
=======


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Construct a Green function given a Mesh, a Target and a Memory Layout`
       | :green:`template<typename Var, typename T, int R>`
       | :ref:`gf\<Var, typename T::target_t\> <triqs__gfs__gf>` :red:`make_gf` (:ref:`gf_mesh\<Var\> <triqs__gfs__gf_mesh>` :param:`m`,
       |   T const & :param:`t`,
       |   memory_layout_t<R> const & :param:`ml`)

    2. | :cppbrief:`Construct a Green function given a Mesh, and a Target`
       | :green:`template<typename Var, typename T>`
       | :ref:`gf\<Var, typename T::target_t\> <triqs__gfs__gf>` :red:`make_gf` (:ref:`gf_mesh\<Var\> <triqs__gfs__gf_mesh>` :param:`m`, T const & :param:`t`)

    3. | :cppbrief:`Create a Green function from another gf or view, changing the memory layout`
       | :green:`template<typename G>`
       | typename G::regular_type :red:`make_gf` (G const & :param:`g`, typename G::data_memory_layout_t const & :param:`ml`)

    4. | :cppbrief:`Create a Green function from another gf or view`
       | :green:`template<typename G>`
       | typename std::decay_t<G>::regular_type :red:`make_gf` (G && :param:`g`)

Documentation