..
   Generated automatically using the command :
   c++2doc.py all_triqs.hpp
   /Users/parcolle/triqs/BUILD/triqs/INSTALL_DIR/include/triqs/mc_tools/mc_generic.hpp

.. highlight:: c


.. _mc_generic_start:

start
=======

**Synopsis**:

.. code-block:: c

    int start (MCSignType sign_init, std::function<bool ()> stop_callback) ;

Starts the Monte Carlo

Parameters
-------------

* **sign_init**: The initial value of the sign (usually 1)

* **stop_callback**: A callback function () -> bool. It is called after each cycle
                      to and the computation stops when it returns true.
                      Typically used to set up the time limit, cf doc.


Return value
--------------


    =  =============================================
    0  if the computation has run until the end
    1  if it has been stopped by stop_callback
    2  if it has been  topped by receiving a signal
    =  =============================================