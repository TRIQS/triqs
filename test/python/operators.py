# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Nils Wentzell

import unittest, itertools

from h5 import *
from triqs.operators import *
from triqs.operators.util import *
from triqs.utility import capture_stdout

ref_output="""Anticommutators:
{ 1*c_dag(1,0) , 1*c(1,0) } = 1
{ 1*c_dag(1,0) , 1*c(2,0) } = 0
{ 1*c_dag(1,0) , 1*c(3,0) } = 0
{ 1*c_dag(2,0) , 1*c(1,0) } = 0
{ 1*c_dag(2,0) , 1*c(2,0) } = 1
{ 1*c_dag(2,0) , 1*c(3,0) } = 0
{ 1*c_dag(3,0) , 1*c(1,0) } = 0
{ 1*c_dag(3,0) , 1*c(2,0) } = 0
{ 1*c_dag(3,0) , 1*c(3,0) } = 1
Commutators:
[ 1*c_dag(1,0) , 1*c(1,0) ] = -1 + 2*c_dag(1,0)*c(1,0)
[ 1*c_dag(1,0) , 1*c(2,0) ] = 2*c_dag(1,0)*c(2,0)
[ 1*c_dag(1,0) , 1*c(3,0) ] = 2*c_dag(1,0)*c(3,0)
[ 1*c_dag(2,0) , 1*c(1,0) ] = 2*c_dag(2,0)*c(1,0)
[ 1*c_dag(2,0) , 1*c(2,0) ] = -1 + 2*c_dag(2,0)*c(2,0)
[ 1*c_dag(2,0) , 1*c(3,0) ] = 2*c_dag(2,0)*c(3,0)
[ 1*c_dag(3,0) , 1*c(1,0) ] = 2*c_dag(3,0)*c(1,0)
[ 1*c_dag(3,0) , 1*c(2,0) ] = 2*c_dag(3,0)*c(2,0)
[ 1*c_dag(3,0) , 1*c(3,0) ] = -1 + 2*c_dag(3,0)*c(3,0)

Algebra:
x = 1*c(0,0)
y = 1*c_dag(1,0)
-x = -1*c(0,0)
x + 2.0 = 2 + 1*c(0,0)
2.0 + x = 2 + 1*c(0,0)
x - 2.0 = -2 + 1*c(0,0)
2.0 - x = 2 + -1*c(0,0)
3.0*y = 3*c_dag(1,0)
y*3.0 = 3*c_dag(1,0)
x + y = 1*c_dag(1,0) + 1*c(0,0)
x - y = -1*c_dag(1,0) + 1*c(0,0)
(x + y)*(x - y) = 2*c_dag(1,0)*c(0,0)
Operator(3.14) = 3.14

N^3:
N = 1*c_dag(0,'dn')*c(0,'dn') + 1*c_dag(0,'up')*c(0,'up')
N^3 = 1*c_dag(0,'dn')*c(0,'dn') + 1*c_dag(0,'up')*c(0,'up') + 6*c_dag(0,'dn')*c_dag(0,'up')*c(0,'up')*c(0,'dn')
Monomials of N^3:
[(True, [0, 'dn']), (False, [0, 'dn'])] 1.0
[(True, [0, 'up']), (False, [0, 'up'])] 1.0
[(True, [0, 'dn']), (True, [0, 'up']), (False, [0, 'up']), (False, [0, 'dn'])] 6.0

X = (-1-2j)*c_dag('a',1)*c_dag('a',2)*c('a',4)*c('a',3)
dagger(X) = (-1+2j)*c_dag('a',3)*c_dag('a',4)*c('a',2)*c('a',1)
X.real = -1*c_dag('a',1)*c_dag('a',2)*c('a',4)*c('a',3)
X.imag = -2*c_dag('a',1)*c_dag('a',2)*c('a',4)*c('a',3)"""

class test_operators(unittest.TestCase):
    def setUp(self):
        pass

    def test_h5_io(self):

        op1 = n("up",0)
        op2 = n("dn",0)
        op_lst = [op1, op2]
        
        # Write list of operators
        with HDFArchive('op.h5', 'w') as arch:
            arch['op_lst'] = op_lst
            arch['op1'] = op1
        
        # Read list
        arch = HDFArchive('op.h5', 'r')
        # self.assertTrue((arch['op1'] -  op1).is_zero())
        # self.assertTrue(all([(x - y).is_zero() for x,y in zip(arch['op_lst'],op_lst)]))

        self.assertEqual(arch['op1'], op1)
        self.assertEqual(arch['op_lst'], op_lst)

    def test_algebra(self):

        with capture_stdout() as output:
            C_list = [c(1,0),c(2,0),c(3,0)]
            Cd_list = [c_dag(1,0), c_dag(2,0), c_dag(3,0)]
            
            print("Anticommutators:")
            for Cd,C in itertools.product(Cd_list,C_list):
                    print("{", Cd, ",", C, "} =", Cd*C + C*Cd)
            
            print("Commutators:")
            for Cd,C in itertools.product(Cd_list,C_list):
                    print("[", Cd, ",", C, "] =", Cd*C - C*Cd)
            
            x = c(0,0)
            y = c_dag(1,0)
            
            print()
            print("Algebra:")
            print("x =", x)
            print("y =", y)
            
            print("-x =", -x)
            print("x + 2.0 =", x + 2.0)
            print("2.0 + x =", 2.0 + x)
            print("x - 2.0 =", x - 2.0)
            print("2.0 - x =", 2.0 - x)
            print("3.0*y =", 3.0*y)
            print("y*3.0 =", y*3.0)
            print("x + y =", x + y)
            print("x - y =", x - y)
            print("(x + y)*(x - y) =", (x + y)*(x - y))
            print("Operator(3.14) =", Operator(3.14))
            
            # N^3
            print()
            print("N^3:")
            N = n(0,'up') + n(0,'dn')
            N3 = N*N*N
            print("N =", N)
            print("N^3 =", N3)
            
            print("Monomials of N^3:")
            for monomial, coef in N3: print(monomial, coef)
            
            # Dagger, real & imag
            X = (1+2j)*c_dag('a',1)*c_dag('a',2)*c('a',3)*c('a',4);
            
            print()
            print("X =", X)
            print("dagger(X) =", dagger(X))
            print("X.real =", X.real)
            print("X.imag =", X.imag)

        self.assertEqual(output, ref_output.splitlines())
        
        # Test Operator comparison
        anti_comm_ccdag = c(0,0) * c_dag(0,0) + c_dag(0,0) * c(0,0)
        self.assertEqual(Operator(1.), anti_comm_ccdag)

    def test_util_extractors(self):

        gf_struct = [('up',2), ('dn',2)]

        U, V = 1., 1.
        h_int = U * (n('up',0) * n('dn',0) + n('up',1) * n('dn',1)) + \
                V * (n('up',0) * n('dn',1) + n('up',1) * n('dn',0))

        U_dict2 = extract_U_dict2(h_int)
        print(U_dict2)
        print(dict_to_matrix(U_dict2, gf_struct))

    def test_hermiticity(self):
        from triqs.operators import is_op_hermitian

        op_test_1 = n('up',0) * n('dn',0) + n('up',1) * n('dn',1)
        assert is_op_hermitian(op_test_1)

        op_test_2 = c(0,0) + c(1,0)
        assert not is_op_hermitian(op_test_2)

        op_test_3 = n('up',0) * n('dn',0) + 1e-10*c('up',0)
        assert is_op_hermitian(op_test_3, 1e-9)

if __name__ == '__main__':
    unittest.main()
