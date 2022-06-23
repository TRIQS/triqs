
.. _hdf5_tut_ex3:

Example 3: Using the dictionary interface of the archive
--------------------------------------------------------------------

The archive is like a dictionary, persistent on disk.
`[for Python afficionados, it is similar to a shelve, but in a portable format]`.
Therefore, one can iterate on its elements.

Let us imagine that you have stored 5 Green functions in an hdf5 file.
For example, we can create such a file as :download:`[file] <./tut_ex3.py>`:

.. literalinclude:: tut_ex3.py

Imagine that we want to plot those functions :download:`[file] <./tut_ex3b.py>`:

.. literalinclude:: tut_ex3b.py
   :lines: 1-13

This produces the following plot (scaled semi-circular density of states).

.. image:: tut_ex3b.png
   :width: 750
   :align: center

Various pythonic constructs can be used in combinaison with HDFArchive, e.g. 
in order to plot only a few curves from a list ::
  
 keylist = ['D=1', 'D=3']

 for g in ( R[x] for x in keylist): # use an iterator
    oplot( (- 1/pi * g).imag, "-o", name = 'g' )
   
or if we want to add the names ::

 for n,g in ( (x,R[x]) for x in keylist): # use an iterator
    oplot( (- 1/pi * g).imag, "-o", name = n )
 
The advantage of using an iterator is that the object is only retrieved from disk when needed.

