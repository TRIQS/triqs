.. highlight:: c

::

  namespace Py_to_C { 
   template<typename T> struct convert{ 
    static bool possible (boost::python::object x);  // returns true iif the conversion of x into a T is possible
    static T invoke(boost::python::object x);        // do it ! 
   };
  } 

  namespace C_to_Py { 
  template<typename T> struct convert { 
    static boost::python::object invoke (T const & x); // convert T into a python object
    };
  }


