
      ALPHA: BEING TESTED

Cpp2Py is the Python-C++ interfacing tool of the TRIQS project, provided here as a standalone project. (The TRIQS website is under http://ipht.cea.fr/triqs. Start there to learn about TRIQS.)

Installation
====================

To install Cpp2Py, follow the installation steps:

```bash
git clone  https://github.com/TRIQS/cpp2py.git cpp2py
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=INSTALL_DIR ../cpp2py
make && make install
```

This installs the library in `INSTALL_DIR`. You should now update your PYTHONPATH to make it available to the python interpreter:

```bash
export PYTHONPATH=INSTALL_DIR/lib/python2.7/site-packages:$PYTHONPATH
```



Example
================

Suppose you have created a c++ source file `mymodule.hpp` in a folder `SRC`:

```c++
///A wonderful little class
class myclass{
	int a,b;

	public:
		myclass(int a_, int b_):a(a_),b(b_){}

	///getter for member a
	int get_a() const { return a;} 
};
```

In the same folder, create a file `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.5)
find_package(Cpp2Py REQUIRED)

set(CPP2PY_ADD_MODULE_ADDITIONAL_PYTHONPATH "INSTALL_DIR/lib/python2.7/site-packages/")

include_directories(${Cpp2Py_DIR}/../../include)
include_directories(.)

add_cpp2py_module(mymodule)
```

Then, in the `SRC` folder, issue the command

```
INSTALL_DIR/bin/c++2py mymodule.hpp 
```

This creates a file `mymodule_desc.py`.

Exit the `SRC` folder and create a `BUILD` folder. Then, issue the following commands:

```bash
cd BUILD
cmake -DCMAKE_PREFIX_PATH=INSTALL_DIR/lib/cmake/cpp2py ../SRC
make 
```

In the `BUILD` dir, you should see a `mymodule.so` file. You can now use your c++ class in Python:

```python
import mymodule
A = mymodule.Myclass(4,5)
A.get_a() #outputs 4
```

By convention, c++ classes of the type `my_little_class` are converted in python classes of the type `MyLittleClass`.

License
===============

Before you proceed, make sure you have read the `LICENSE.txt` file.

Enjoy!

The TRIQS team
