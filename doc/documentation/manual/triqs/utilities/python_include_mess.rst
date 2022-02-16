Python include warnings
=======================

Very often compile warnings of Python can be seen, something like ::

    /usr/local/install/python-2.7.5/include/python2.7/pyconfig.h:1173:0: warning: "_POSIX_C_SOURCE" redefined [enabled by default]
    #define _POSIX_C_SOURCE 200112L
    ^
    In file included from /usr/local/install/gcc-4.8.1/include/c++/4.8.1/x86_64-unknown-linux-gnu/bits/os_defines.h:39:0,
     ......
    /usr/include/features.h:162:0: note: this is the location of the previous definition
    # define _POSIX_C_SOURCE 200809L
    ...
    #define _XOPEN_SOURCE 600


It is due to the fact that Python.h must be included
before some other headers (cf python documentation).

Solutions:

#. include first some triqs library, like arrays.hpp, gfs.hpp, etc...

#. include first <utility/first_include.hpp>
   that conditionally includes python if python is to be supported.
   (used by all triqs libs, hence the first point).
