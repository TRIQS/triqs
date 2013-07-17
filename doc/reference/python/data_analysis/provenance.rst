Reproducibility, provenance.
=================================

Scientific numerical calculations are ... scientific calculations.

Hence, like any other kind of calculations, according to the basic principles of science,
everyone should be able to reproduce them, reuse or modify them.
Therefore, the detailed instructions leading to results or figures 
should be published along with them.
To achieve these goals, in practice we need to be able to do simply the following things : 

* Store along with the data the version of the code used to produced them (or even the code itself !), 
  and the configuration options of this code.

* Keep with the figures all the instructions (i.e. the script) that have produced it.

* We want to do that **easily, at no cost in human time**, hence 
  without adding a new layer of tools (which means new things to learn, which takes time,  etc...). 
  Indeed this task is important but admittedly extremely boring for physicists...

Fortunately, python helps solving these issues easily and efficiently.

TRIQS adds very little to the standard python tools here.
So this page should be viewed more as a wiki page of examples.
TRIQS does not impose any framework on you, it just provides tools
and lets you organize your work as you wish.


TRIQS code version
----------------------

The tiny module `pytriqs.version` 
contains various pieces of information configured automatically at compile time ::

 import pytriqs.version as V

 V.revision           # The branch and the git_hash commit number or version number
 V.git_hash           # The commit number if under git, or ""
 V.branch             # the branch on which the code was compiled

 # publishing these informations may lead to a security issue....
 V.cmake_options()    # returns the contents of the CMakeCache.txt file 
 V.machine_info       # Hostname and login of the compilation
 V.all_info           # a print of most info (except Cache)

.. warning:: 
 
  While the informations of the configurations, the machine, and login may be useful for *you* to 
  reproduce the computation, publishing them is not recommended since it contains informations on your machine, your login.
  For public data, just use V.revision.

Saving the script in the data archive
-------------------------------------------

It is actually very simple to  save the script 
(hence the parameters) along with the data, 
simply by putting it in the HDFArchive, e.g. ::

  # ... computation ...
  Results = HDFArchive("solution.h5",'w')
  Results["G"] = S.G # save the results
  
  import sys, pytriqs.version as version
  Results.create_group("log")
  log = Results["log"]
  log["code_version"] = version.revision
  log["script"] = open(sys.argv[0]).read() # read myself !
   
The script that is currently being executed will be copied into the file `solution.h5`, under the subgroup `/log/script`.

In a more complex situation, you may decompose your computation in several scripts, e.g. 

* A script common.py, with some common functions, classes...
* A little one, computation1.py for each computations.

In such situation, one can simply use the `inspect` module of the python standard library e.g. ::
  
   import common
   # set parameters
   # run...
   # save...
   # Ok, I need to save common too !
   import inspect,sys, pytriqs.version as version
   log = Results.create_group("log")
   log["code_version"] = version.revision()
   log["script"] = open(sys.argv[0]).read()
   log["common"] = inspect.getsource(common) # This retrieves the source of the module in a string


From the data to the figures
-------------------------------------------

[TO BE WRITTEN]








