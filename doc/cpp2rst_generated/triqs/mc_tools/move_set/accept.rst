..
   Generated automatically by cpp2rst

.. highlight:: c


.. _move_set_accept:

accept
======

**Synopsis**:

.. code-block:: c

    MCSignType accept ()

accept the move previously selected and tried.


Documentation
-------------

  Returns the Sign computed as, if M is the move :
  Sign = sign (M.attempt()) * M.accept()