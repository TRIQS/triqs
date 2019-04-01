..
   Generated automatically by cpp2rst

.. highlight:: c


.. _random_generator_operator():

operator()
==========

**Synopsis**:

.. code-block:: c

    template<typename T>                       (1)
    requires(std::is_integral<T>::value)
    T  operator() (T i)

    double operator() ()                       (2)

    double operator() (double x)               (3)

    double operator() (double a, double b)     (4)

(1)Returns a integer in [0,i-1] with flat distribution


(2)


(3)Returns a double in [0,x[ with flat distribution


(4)Returns a double in [a,b[ with flat distribution