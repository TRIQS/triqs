
TRIQS as a library
==================

.. highlight:: c

Let's say you want to write a new code that uses the libraries and headers of
TRIQS but that is not meant to be a part of TRIQS. Here's how you should
proceed in order to do that.


Setting the stage
-----------------

Just to be clear, let me start by showing the directory structure
I will use:

.. code-block :: bash

   /home/triqs/install --> TRIQS is installed here
   /home/project/src --> the sources of my project
   /home/project/build --> the directory where I will compile my code

Basically, this structure means that you have successfully installed TRIQS in
:file:`/home/triqs/install` and that you plan to have your new project under
:file:`/home/project`. Obviously you can choose any other directory but this
structure will be assumed below.


Start your project
------------------

As we just said, we will start our project in a directory
:file:`/home/project`. We will have the sources in :file:`/home/project/src`
and later build (compile) the project in :file:`/home/project/build`.  Let's
start by writing some sources:

.. code-block :: bash

  $ cd /home
  $ mkdir project
  $ cd project
  $ mkdir src
  $ cd src

OK, our project will be just one :file:`main.cpp` file, e.g.::

  #include <triqs/arrays/array.hpp>

  namespace tqa = triqs::arrays;

  int main(){

    tqa::array<double,1> A(10), B(10);
    A()=2; B()=3;
    tqa::array<double,1> C = A+B;
    std::cout << "C = "<< C << std::endl;

  }

As you can see, the code includes headers from TRIQS. Along with
:file:`main.cpp` we write a :file:`CMakeLists.txt` file to compile our project.
In order to make this easy, there is a file called :file:`TRIQSConfig.cmake`
in :file:`/home/triqs/install/share/triqs/cmake`. Including this file in
your :file:`CMakeLists.txt` automatically defines a certain number of useful
variables, especially the include directories related to the TRIQS headers and
the location of the TRIQS libraries. Here is what your simple
:file:`CMakeLists.txt` can be:

.. code-block :: bash

  cmake_minimum_required(VERSION 2.8)

  project(myproj CXX)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

  set(CMAKE_BUILD_TYPE Release)

  SET(CMAKE_INSTALL_RPATH "${TRIQS_PATH}/lib")
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

  include(${TRIQS_PATH}/share/triqs/cmake/TRIQSConfig.cmake)

  add_executable(example main.cpp)

  include_directories(${TRIQS_INCLUDE} ${EXTRA_INCLUDE} ${CBLAS_INCLUDE} ${FFTW_INCLUDE})
  target_link_libraries(example ${TRIQS_LIBRARY} ${EXTRA_LIBRARY})

We're all set! Everything is ready to compile our project. If we want to build
everything in :file:`/home/project/build`, we do as follows:

.. code-block :: bash

  $ cd /home/project
  $ mkdir build
  $ cd build
  $ cmake -DTRIQS_PATH=/home/triqs/install /home/project/src
  $ make
  $ ./example

That's it! You can modify your sources and then recompile with make. Obviously
with bigger projects your :file:`CMakeLists.txt` file will change, but the
principle remains the same.

