# Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Thomas Ayral, Olivier Parcollet, Nils Wentzell

from h5 import *
from triqs.utility.comparison_tests import *
from triqs.gf import *
from triqs.operators import *
import sys
import numpy

verbose = 0
failures = []

def compare(key, a, b, level, precision):
    """Compare two objects named key"""

    if verbose and key : print(level *'  ' + "Comparing %s ...."%key)

    try :
        t = type(a)
        assert t == type(b), "%s have different types"%key

        if t == dict or isinstance(a, HDFArchiveGroup) :
            if list(a.keys()) != list(b.keys()):
                failures.append("Two archive groups '%s' with different keys \n %s \n vs\n %s"%(key,list(a.keys()), list(b.keys())))
            for k in a.keys():
                compare(key + '/'+ k, a[k], b[k], level + 1, precision)

        # The TRIQS object which are comparable starts here ....
        #elif t in [GfImFreq, GfImTime, GfReFreq, GfReTime, GfLegendre, GfImFreq_x_ImFreqTv3] :
        elif t in [Gf] :
            assert_gfs_are_close(a,b,precision)

        elif t in [BlockGf]:
            assert_block_gfs_are_close(a,b,precision)

        elif t in [Block2Gf]:
            assert_block2_gfs_are_close(a,b,precision)

        elif t in [Operator]:
            assert (a-b).is_zero(), "Many body operators not equal"

        # elif t in [BlockMatrix]:
            # for i in range(len(a.matrix_vec)):
             # assert_arrays_are_close(a(i),b(i))

        # ... until here
        elif isinstance(a, numpy.ndarray):
            assert_arrays_are_close(a,b)

        elif t in [int, float, complex]:
            assert abs(a-b) < 1.e-10, " a-b = %"%(a-b)

        elif t in [bool, numpy.bool_]:
           assert a==b

        elif t in [list, tuple]:
            assert len(a) == len(b), "List of different size"
            for x,y in zip(a,b):
                compare(key, x, y, level+1, precision)

        elif t in [str]:
            assert a==b, "Strings '%s' and '%s' are different"%(a,b)

        else:
            raise NotImplementedError("The type %s for key '%s' is not comparable by h5diff"%(t, key))

    except (AssertionError, RuntimeError, ValueError) as e:
        #eliminate the lines starting with .., which are not the main error message
        mess = '\n'.join([l for l in str(e).split('\n') if l.strip() and not l.startswith('..')])
        failures.append("Comparison of key '%s'  has failed:\n """%key + mess)

def h5diff(f1, f2, precision= 1.e-6):
    compare('', HDFArchive(f1,'r'), HDFArchive(f2,'r'), 0, precision)
    if failures :
        print ('-'*50, file=sys.stderr )
        print ('-'*20 + '  FAILED  ' +  '-'*20, file=sys.stderr)
        print ('-'*50, file=sys.stderr)
        for x in failures:
            print (x, file=sys.stderr)
            print ('-'*50, file=sys.stderr)
        raise RuntimeError("FAILED")

if __name__== "__main__":

    # --- Parsing the arguments of the script and options
    import argparse
    parser = argparse.ArgumentParser(description="""h5diff with proper support of TRIQS object """)
    parser.add_argument('archive1', help = "Name of the first h5")
    parser.add_argument('archive2', help = "Name of the second h5")
    parser.add_argument('--verbose', '-v',  action='store_true', help="")
    parser.add_argument('--precision', '-p',  action='store', type=float, default= 1.e-8, help="")

    args = parser.parse_args()
    verbose = args.verbose

    h5diff (args.archive1, args.archive2, args.precision)

