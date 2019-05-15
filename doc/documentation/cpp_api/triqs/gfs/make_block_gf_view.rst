..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _make_block_gf_view:

make_block_gf_view
==================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Make a block view from the G. Indices are '0', '1', '2', ....`
       | :green:`template<typename G0, typename G>`
       | block_gf_view_of<G0> :red:`make_block_gf_view` (G0 && :param:`g0`, G &&... :param:`g`)

    2. | :cppbrief:`Create block_gf_view from vector of views`
       | :green:`template<typename Gf>`
       | block_gf_view_of<Gf> :red:`make_block_gf_view` (std::vector<Gf> & :param:`v`)

    3. | :cppbrief:`Create block_gf_view from block_names and vector of views`
       | :green:`template<typename Gf>`
       | block_gf_view_of<Gf> :red:`make_block_gf_view` (std::vector<std::string> :param:`b`, std::vector<Gf> & :param:`v`)

Documentation