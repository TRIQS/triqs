Profiling in C++ and Python
=============================

One can easily profile c++ and Python code using `Google perftools <http://code.google.com/p/gperftools/>`_. In Ubuntu: ::

  libgoogle-perftools-dev
  google-perftools

One must link the executable with the profiling library with the flag ``-lprofiler``.

C++
-------


First run the C++ executable (here ``simple_tests``) after setting the environment variable ``CPUPROFILE``: ::

   CPUPROFILE=profile_test.prof ./simple_tests

Then, analyze the results (stored in `profile_test.prof`) with  ``google-pprof``: ::

   google-pprof --text ./simple_tests profile_test.prof | less 

See the documentation of `Google perftools <http://code.google.com/p/gperftools/>`_ for more information.

Python
--------
One needs to install the python package `yep <https://pypi.python.org/pypi/yep>`_ (e.g ``easy_install yep``)

First, run your script (``my_test.py``): ::

   pytriqs -myep -v  my_test.py

Then, analyze the results (stored in `my_test.py.prof`) with ``google-pprof``: ::

   google-pprof --text my_test.py my_test.py.prof | less 

Alternatively, to view the results more graphically, run: ::

   google-pprof --web my_test.py my_test.py.prof
