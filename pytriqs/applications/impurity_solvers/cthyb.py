r"""
Deprecated module
"""

import warnings
warnings.warn("""
***************************************************

from pytriqs.applications.impurity_solvers.cthyb import XXX

is deprecated. Replace it by 
      
from triqs_cthyb import XXX 

****************************************************""")

try:
  from triqs_cthyb import *
except ImportError : 
    print "triqs_cthyb package is not installed"
    raise

#__all__ = []
