* Any class T should provide :
  
* broadcast::

   void mpi_broadcast(T& x, communicator c, int root);

* send, recv.

* reduce::

    X mpi_reduce  (T const & x, communicator c= {}, int root = 0, bool all= false);

* gather/scatter ::

    X mpi_scatter (T const & x, communicator c={}, int root=0);
    X mpi_gather  (T const & x, communicator c={}, int root=0, bool all=false);

  where X is such that t = x; is correct T t, X x.
  This allow for lazy mechanism or not, to be chosen by the class.


