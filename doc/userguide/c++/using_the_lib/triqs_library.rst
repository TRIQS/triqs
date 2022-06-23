.. highlight:: c

Writing you own C++ code with TRIQS
------------------------------------

Basically, this structure means that you have successfully installed TRIQS in
:file:`/home/triqs/install` and that you plan to have your new project under
:file:`/home/project`. Obviously you can choose any other directory but this
structure will be assumed below.

.. code-block :: bash

   /home/triqs/install --> TRIQS is installed here
   /home/project/src --> the sources of my project
   /home/project/build --> the directory where I will compile my code

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

  #include <nda/nda.hpp>
  using namespace nda;
  int main(){
    array<double,1> A {1,2,3}, B{10,20,30}, C;
    C = A+B;
    std::cout << "C = "<< C << std::endl;
  }

As you can see, the code includes headers from TRIQS. Along with
:file:`main.cpp` we write a :file:`CMakeLists.txt` file to compile our project.
In order to make this easy, there is a file called :file:`TRIQSConfig.cmake`
in :file:`/home/triqs/install/lib/cmake/triqs`. Including this file in
your :file:`CMakeLists.txt` automatically defines a certain number of useful
variables, especially the include directories related to the TRIQS headers and
the location of the TRIQS libraries. Here is what your simple
:file:`CMakeLists.txt` can be:

.. code-block :: cmake

    # Start configuration 
    cmake_minimum_required(VERSION 2.8.12)
    project(myproj CXX)
    set(CMAKE_BUILD_TYPE Release)

    # Load TRIQS, including all predefined variables from TRIQS installation
    find_package(TRIQS REQUIRED)

    # Create executable
    add_executable(example main.cpp)

    # Linking and include info
    target_link_libraries(example triqs)
    triqs_set_rpath_for_target(example)

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

A simple C++ project, with its tests and documentation
------------------------------------------------------

.. warning::

  TO BE WRITTEN

A mixed C++/Python project
------------------------------

.. warning::

  TO BE WRITTEN

