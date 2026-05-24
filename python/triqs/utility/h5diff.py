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

r"""
TRIQS-aware comparison of two HDF5 archives.

The module is also installed as a console entry point so that
``python -m triqs.utility.h5diff file1.h5 file2.h5`` can be used from
the shell.

Attributes
----------
verbose : int
    Module-level flag. When set to a truthy value, :func:`compare`
    prints the key being inspected at each level of the recursion.
    Default ``0``.
failures : list of str
    Module-level list to which :func:`compare` appends a human-readable
    error message for every mismatch. :func:`h5diff` inspects it after
    walking the archives and raises if it is non-empty.
"""

from h5 import *
from triqs.utility.comparison_tests import *
from triqs.gfs import *
from triqs.operators import *
import sys
import numpy

verbose = 0
failures = []

def compare(key, a, b, level, precision):
    r"""
    Recursively compare two objects identified by ``key``.

    Dispatches on the runtime type of ``a`` (and asserts that ``b``
    has the same type):

    - :class:`dict` or :class:`HDFArchiveGroup` -- compare the set of
      keys and recurse on each value.
    - :class:`Gf` / :class:`BlockGf` / :class:`Block2Gf` -- defer to
      :func:`assert_gfs_are_close` /
      :func:`assert_block_gfs_are_close` /
      :func:`assert_block2_gfs_are_close`.
    - :class:`Operator` -- check that ``(a - b).is_zero()``.
    - :class:`numpy.ndarray` -- defer to
      :func:`assert_arrays_are_close`.
    - :class:`int`, :class:`float`, :class:`complex` -- check
      ``abs(a - b) < 1e-10``.
    - :class:`bool` / :class:`numpy.bool_` -- check ``a == b``.
    - :class:`list` / :class:`tuple` -- compare length and recurse
      element-wise.
    - :class:`str` -- check ``a == b``.

    Any mismatch is appended to the module-level ``failures`` list
    rather than raised immediately, so that the full diff is reported
    at the end of :func:`h5diff`.

    Parameters
    ----------
    key : str
        Slash-separated path of the current node inside the archive,
        used in error messages.
    a, b : object
        Values to compare.
    level : int
        Current depth in the recursive walk, controls the indentation
        of the optional progress output emitted when the module-level
        ``verbose`` flag is set.
    precision : float
        Maximum allowed element-wise absolute difference passed to the
        underlying array / Green's-function comparisons.

    Raises
    ------
    NotImplementedError
        If the type of ``a`` is not one of the supported categories
        listed above.
    """

    if verbose and key : print(level *'  ' + "Comparing %s ...."%key)

    try :
        t = type(a)
        assert t == type(b), "%s have different types"%key

        if t == dict or isinstance(a, HDFArchiveGroup) :
            if set(a.keys()) != set(b.keys()):
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
    r"""
    Compare two HDF5 archives and raise on mismatch.

    Opens both archives in read-only mode and walks them recursively
    via :func:`compare`. Any mismatch is collected in the module-level
    ``failures`` list. If that list is non-empty after the walk, the
    collected messages are written to ``stderr`` and a
    :class:`RuntimeError` is raised.

    Parameters
    ----------
    f1, f2 : str
        Paths to the HDF5 files to compare.
    precision : float, optional
        Maximum allowed element-wise absolute difference for arrays
        and Green's-function data, passed through to the
        :mod:`triqs.utility.comparison_tests` assertions. Default
        ``1e-6``.

    Raises
    ------
    RuntimeError
        If any node of the two archives differs.
    """
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

