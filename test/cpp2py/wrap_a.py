# Copyright (c) 2014-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
# Copyright (c) 2014-2015 Igor Krivenko
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
# Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

from my_module import *

from h5 import *
import h5.formats

import unittest
import pickle
import numpy

class test_wrap(unittest.TestCase):

    def test_wrap_a(self):

        a = Ac()
        print(a.m1(1,2.0))

        print_a (a)

        v1 =  make_vector(3)
        v2 = vector_x2(v1)

        print(v1, v2)

        print(vector_x2([1,2,3,4]))
        print(vector_x2((1,2,3)))

        print(a+ a)
        #print_a (8)

        f,s = a.__reduce__()
        b = f(*s)

        # lambda <-> std::function
        fc = make_fnt_ii()
        #print fc
        use_fnt_ii(fc)
        fp = lambda i,j : i + 20*j
        use_fnt_ii(fp)

        fc2 = make_fnt_iid()
        #print fc2
        use_fnt_iid(fc2)
        fp2 = lambda i,j,a : a+ i + 20*j
        use_fnt_iid(fp2)

        # Check that wrong return type leads to exception
        with self.assertRaises(Exception):
           fstr = lambda i,j,a : "hello"
           use_fnt_iid(fstr)

        # Check py2c conversion of complex
        assert(1.0 == complex_to_complex(1.0))
        assert(1j == complex_to_complex(1j))
        assert(1j == complex_to_complex(numpy.complex128(1j)))

        print(map_to_mapvec({'a':1, "b":2, "sjkdf":5}))

        print(set_to_set(set(['abcd','2','345','klmn'])))

        print(tuple_to_tuple_0([]))
        print(tuple_to_tuple_1((9,)))
        print(tuple_to_tuple_2((9,2.5)))
        print(tuple_to_tuple_3((9,2.5,'hi')))

        print(variant_to_variant(9))
        print(variant_to_variant("qwerty"))
        print(variant_to_variant(("name",7.65)))

        R = HDFArchive('ess.h5', 'w')    # Opens the file myfile.h5, in read/write mode
        R['a'] = 2.3*a
        del R

        R2 = HDFArchive('ess.h5', 'r')
        aa = R2['a']
        print(" rereading from hdf5 ", aa)

        s = pickle.dumps(a)
        print(repr(s))
        bb = pickle.loads(s)
        print(bb)

        #  Examples:
        # self.assertEqual(fp.readline(), 'This is a test')
        # self.assertFalse(os.path.exists('a'))
        # self.assertTrue(os.path.exists('a'))
        # self.assertTrue('already a backup server' in c.stderr)
        # self.assertIn('fun', 'disfunctional')
        # self.assertNotIn('crazy', 'disfunctional')
        # with self.assertRaises(Exception):
        #    raise Exception('test')
        #
        # Unconditionally fail, for example in a try block that should raise
        # self.fail('Exception was not raised')

if __name__ == '__main__':
    unittest.main()

