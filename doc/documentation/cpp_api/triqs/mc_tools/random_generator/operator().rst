..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _random_generator_operator():

triqs::mc_tools::random_generator::operator()
=============================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Returns a integer in [0,i-1] with flat distribution`
       | :green:`template<typename T>`
       | typename std::enable_if<std::is_integral<T>::value, T>::type :red:`operator()` (T :param:`i`)

    2. | double :red:`operator()` ()

    3. | :cppbrief:`Returns a double in [0,x[ with flat distribution`
       | double :red:`operator()` (double :param:`x`)

    4. | :cppbrief:`Returns a double in [a,b[ with flat distribution`
       | double :red:`operator()` (double :param:`a`, double :param:`b`)

Documentation