from dcomplex cimport * 
from libcpp.string cimport string

# The c++ random generator class
# We wrap two members only
cdef extern from "triqs/mc_tools/random_generator.hpp": 

    cdef cppclass random_generator "triqs::mc_tools::random_generator":

        random_generator(string, long) except +
        double operator() () except +
        int operator() (int) except +

# This is the wrapping of the static member random_generator_names
# It is a bit of a hack but there is no notion of ststic members in cython
cdef extern from "triqs/mc_tools/random_generator.hpp" namespace "triqs::mc_tools::random_generator":

    string random_generator_names(string) except +


# The python RandomGenerator class
cdef class RandomGenerator:

  cdef random_generator * _c

  def __init__(self, name, seed):
    """This is a random number generator class based on boost.

       :param name: (string) Name of the random number generator
       :param seed: (int) Random number seed
    """
    self._c = new random_generator(name, seed)

  def __dealloc__(self):
    del self._c

  def rand(self):
    """Generate a float random number in [0,1["""
    return self._c[0]()

  def int_rand(self, N):
    """Generate an integer random number in [0,N-1]"""
    return self._c[0](N)

# The list of generator names accessed through the static member
def available_generator_names():
  """Get a list of available random generator names"""
  return random_generator_names(",").split(',')

