.. highlight:: c

Interoperability with numpy arrays in Python
===================================================================

.. warning::
   This section assume that the reader has minimal knowledge of Python/C++ interface building  with boost.python.
   
.. warning::
   In order to activate python support, the macro TRIQS_ARRAYS_WITH_PYTHON_SUPPORT must be defined.

The array, matrix, vector and their views are fully interoperable with the numpy array objects in python.
We need to distinguish between : 

* the conversion of the object between C++ and python, which can happen anywhere in the code.
* the interfacing or wrapping of the function and classes, which is done at the interface of the languages.

The conversion processes is largely independent of the wrapping technique (boost.python, swig, e.g.).
But the wrapping technique of course uses the converters to convert the object at the interface between Python and C++.

Converters
--------------------------

The conversion of a C++ object to a numpy consists in two converters : 

* The C to Python conversion : 
  
  It returns a boost::python::object, the opaque description of a python object in C
  
  It is handled by the C_to_Py::convert<T> ::
   
    boost::python::object C_to_Py::convert<T>::invoke (T const & x); // convert T into a python object

*  The opposite conversion consists in constructing a C++ object from the boost::python::object::

    Py_to_C::convert<T>::possible(boost::python::object x);  // returns true iif the conversion of x into a T is possible
    T Py_to_C::convert<T>::invoke(boost::python::object x);  // do it ! 

* T can be : 

  * array, array_view, matrix, matrix_view, vector, vector_view
  
  * [this is not array lib but triqs] any std::vector, std::pair, boost::unordered_map of those, recursively.
    ---> cf the doc of this converters

  
Copy vs View
-----------------------

 The python conversion of the array and array_view follows the policy of the C++ classes :
   
 * `value classes` (array, matrix, vector) **always** make copies : 
     
     * when returning to python : they present a fresh copy of the data.
     * when being contructed from python data, they make their own copy.

 * `view classes` **never** make copies : 
     
     * when returning to python : they return a numpy, which is a view of their data.
       By doing this, they transfer ownership of the data to the python interpreter.
       Cf memorymanagement.

     * when being contructed from python data, they make view.
       If this is not possible (e.g. the python object is not a numpy, but a list, the type are not exactly the same)
       they throw an exception (`triqs::runtime_error`).

Interfacing, wrapping
---------------------------

* boost.python

  These converters can be registered to build regular boost.python converter, using, e.g. ::

   triqs::python_tools::register_converter< triqs::arrays::array<double,2> > ();

* swig : not implemented [ to do : add the typemap]


Examples 
-----------------------


Example 1 : wrapping a simple function with Boost Python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* The problem we want to solve : 

- write a simple function in C++ taking an array, and compute something from it.
- wrap this function and use it from python.

* The function :: 

      array_view<double,2> f( array_view<double,2> A) { 
       A(0,0) = 34;   // do something       
      } 

* The wrapping code ::

      #include <boost/python.hpp>
      #include <boost/python/def.hpp>
      BOOST_PYTHON_MODULE(_testarray) {
      using namespace boost::python;
      triqs::python_tools::register_converter< triqs::arrays::array_view<double,2> > ();
      def("f", f);
      }

* Use it in a python code.

  .. code-block:: python 

    import numpy,_testarray 
    a=numpy.array([[1.0,2],[3,4]]) 
    _testarray.f(a)


Example 2 : wrapping a simple function with Boost Python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* A variant: we can be more explicit in the type conversion 
  (in this case, we don't need to register the converter, since we do the job ourselves).


* The function :: 

      boost::python::object f( boost::python::object  M ) { 
       array_view<double,2> A(M.ptr());       // make a view of M. Throws if impossible
       A(0,0) = 100;                     // do something       
       return M;                         // return it   
      } 


* The wrapping code ::

      #include <boost/python.hpp>
      #include <boost/python/def.hpp>
      BOOST_PYTHON_MODULE(_testarray) {
      using namespace boost::python;
      def("f", f);
      }

* Use it in a python code.

  .. code-block:: python 

    import numpy,_testarray 
    a=numpy.array([[1.0,2],[3,4]]) 
    print _testarray.f(a)




