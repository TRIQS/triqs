
.. _HDF_Protocol1:

Solution 1. The class provides the transformation into a dict of hdf-compliant objects
--------------------------------------------------------------------------------------

Principle
^^^^^^^^^

The object which can be reduced to and reconstructed from a dictionary whose keys are strings,
and whose values are of one of the following types:

    - a scalar (int, float, complex ....)
    - a numpy array of scalars
    - an hdf-compliant object
    - a list, a tuple or a dict of any of the types above.

A class `cls` has to implement :

.. py:method:: __reduce_to_dict__()

   Returns a dictionary of objects implementing :ref:`hdf-compliant <HDF_Protocol>`
   or basic objects as number, strings, and arrays of numbers (any type and shape).

   :rtype:  a dictionary

.. py:classmethod:: __factory_from_dict__(cls,name,D) :

   A **classmethod** which reconstructs a new object of type `cls`
   from the dictionary  returned by :func:`__reduce_to_dict__`.

   :param cls: the class
   :param name: the attribute cls.name of the class
   :param D: the dictionary returned by __reduce_to_dict__.
   :rtype: a new object of type `cls`

Example
^^^^^^^

A little example::

 class Example:

    def __init__(self,d,t):
        self.d,self.t = d,t # some data

    def __reduce_to_dict__(self):
        return {'d': self.d, 't': self.t}

    @classmethod
    def __factory_from_dict__(cls,name,D) :
        return cls(D['d'],D['t'])

 # registering my class
 from h5.formats import register_class
 register_class (myclass)

 # Testing it:
 HDFArchive("myfile2.h5")['e'] = Example( [1,2,3], 56)

What happens in detail?
^^^^^^^^^^^^^^^^^^^^^^^

Let us consider an object `Ob` of class `Cls`, interacting with and :py:class:`~h5.hdf_archive.HDFArchive`/:py:class:`~h5.hdf_archive.HDFArchiveGroup`  `H`.

* **Writing** ::

    H[Name] = Ob

  * a subgroup `S` of path `Root_of_H/Name` is created.
  * Ob.__reduce_to_dict__() is called and returns a dictionary `D` of  scalar/numpy arrays/hdf-compliant objects.
  * The objects in `D` are then stored `S` in a regular way (scalar/arrays) or using the same recursive procedure (other objects).
  * `S` is tagged with the attribute `HDF5_data_scheme` of `Cls`.


* **Reading** ::

      res = H[Name]

  * The subgroup `S` of path `Root_of_H/Name` is explored. All objects are taken to build a dictionary `D`: name -> values.
    This procedure is recursive: the hdf-compliant objects in the subgroup are rebuilt.
  * An attribute `HDF5_data_scheme` is searched for `S`.

     * If it is found and it corresponds to a registered class `Cls` :

       * Cls.__factory_from_dict__(name,D) is called and returns a new object Obj of type Cls, which is returned as `res`.

     * Otherwise, a new :py:class:`~h5.hdf_archive.HDFArchiveGroup` is constructed with `S` as root, and returned as `res`.
