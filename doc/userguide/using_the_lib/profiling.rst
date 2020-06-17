Profiling in C++ and Python
=============================

One can easily profile c++ and Python code using `Google perftools <http://code.google.com/p/gperftools/>`_. In Ubuntu: ::

  libgoogle-perftools-dev
  google-perftools


C++
-------

There are two options to generate the profiling files that will be then
be analyzed by ``google-pprof``:

#. Link the executable with the profiling library at compilation
   with  flag ``-lprofiler``. Then run the C++ executable (here
   ``simple_tests``) after setting the environment variable ``CPUPROFILE``: ::

     CPUPROFILE=profile_test.prof ./simple_tests

#. If you do not want to recompile the executable, you can directly run
   the C++ executable but with an extra preload of the profiling library: ::

     LD_PRELOAD=/usr/lib/libprofiler.so.0 CPUPROFILE=profile_test.prof ./simple_tests

After the code has been executed, a new file `profile_test.prof` is generated.
It contains all the profiling information and can be analyzed with the
following command::

  google-pprof --text ./simple_tests profile_test.prof | less

See the documentation of `Google perftools <http://code.google.com/p/gperftools/>`_ for more information.

Python
--------

One needs to install the python package `yep <https://pypi.python.org/pypi/yep>`_ (e.g ``easy_install yep``)

First, run your script (``my_test.py``): ::

   triqs -myep -v  my_test.py

Then, analyze the results (stored in `my_test.py.prof`) with ``google-pprof``: ::

   google-pprof --text my_test.py my_test.py.prof | less

Alternatively, to view the results more graphically, run: ::

   google-pprof --web my_test.py my_test.py.prof
