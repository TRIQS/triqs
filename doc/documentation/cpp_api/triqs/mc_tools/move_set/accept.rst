..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _move_set_accept:

triqs::mc_tools::move_set::accept
=================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`accept the move previously selected and tried.`
       | MCSignType :red:`accept` ()





    Returns the Sign computed as, if M is the move :
  Sign = sign (M.attempt()) * M.accept()