# Guide for porting your application to TRIQS 3.0 and Python 3

Below we provide a detailed guide on how to port your application to both TRIQS Version 3.0.0 and Python 3.


## Preparation

Be sure to start from a clean git repository where all previous work has been committed.
Switch to the latest development branch of your application which should be compatible against TRIQS 2.2.x.
We suggest your create a new branch called e.g. `py3`. After each of the steps below, commit your changes with a proper message.


## Port your Python files to Python 3

The 2to3 tool allows for easy and convenient porting of Python files to Python 3.
It can be installed via pip

```bash
pip install --user 2to3
```

To update your files **in-place** without the creation of backups
you can run the following two commands at the top-level of your repository.

```bash
find . -name "*.py" -exec 2to3 -w -n {} \;
find . -name "*.py.in" -exec 2to3 -w -n {} \;
```

Review all changes and commit them.


## Porting IPython Notebooks

The 2to3 tool does not work for IPython notebooks.
With the help of a small [script](https://gist.githubusercontent.com/takluyver/c8839593c615bb2f6e80/raw/e64a8a616e4c1dbc3b1db7289555a3d03b36eda9/2to3_nb.py)
however we can run 2to3 on all cells of our notebook to convert it.
Download the script to the top-level of your repository and make it executable with `chmod u+x 2to3_nb.py`.
You can then port all iPython notebooks **in-place** using the command

```bash
find -name "*.ipynb" -exec ./2to3_nb.py {} \;
```

Review all changes and commit them.


## Floor division

From Python 2 to Python 3 the default behavior when diving two integers has changed from a rounding-based (floor) division to a floating-point divison.
The rounding-based integer division (`__floordiv__`) now has to be indicated by a double-slash, e.g. `3 // 2`.
This change is not properly taken care of by the 2to3 tool and is in general hard to detect.
A properly tested application is your best safety-net against this change.
It has however proven very useful to check all divisions matching particular patterns (regular expressions) explicitly.
This can be achieved for example in the vim editor. We can open all Python files in a given directory with

```
vim *.py
```

We can then execute a regular-expression based search-and-replace with

```
:bufdo :%s#\(\w *\)/\( *[Nn1-9]\)#\1//\2#gc
```

This command will iterate over all open files, searching for potential floor-divisions.
You will be asked you whether you want to replace the floating-point division `/` by a rounding-based division `//`.
For each case decide to execute the replacement with `y` or to decline it with `n`.

Once you are done, save and quit with `:xa`.


## Porting to TRIQS 3.0

TRIQS 3.0 introduced a number of changes that require adjustments in TRIQS-based applications.
These changes are summarized in the [ChangeLog](https://github.com/TRIQS/triqs/blob/unstable/doc/ChangeLog.md).
For convenient porting, we have created a [porting script](https://raw.githubusercontent.com/TRIQS/triqs/unstable/porting_tools/port_to_triqs3)
that you can download and run in the top-level directory of your respository.

```bash
wget https://raw.githubusercontent.com/TRIQS/triqs/unstable/porting_tools/port_to_triqs3
chmod u+x port_to_triqs3
./port_to_triqs3
```

Finally, adjust the version of your project in the top-level CMakeLists.txt file to e.g. `3.0.0`

```cmake
project(APPNAME VERSION 3.0.0 LANGUAGES C CXX)
```

and make sure that version `3.0.0` of TRIQS is detected

```cmake
find_package(TRIQS 3.0 REQUIRED)
```

Review all changes and commit them.


## Regenerate desc files

If you have auto-generated Python modules based on C++2Py be sure to regenerate these files

```bash
c++2py -r module_name_desc.py
```

Review all changes and commit them.


## Update app4triqs skeleton

If your triqs application is based on the [app4triqs](https://github.com/triqs/app4triqs) skeleton,
please update your application as outlined [here](https://github.com/triqs/app4triqs#merging-app4triqs-skeleton-updates).


## Final Steps

Finally you should build and test your application.
Be sure to have a recent version of Python 3 installed on your system.
During cmake configuration you can then choose a particular Python executable with `-DPYTHON_EXECUTABLE=path_to_python3`.

