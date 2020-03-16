# Test from I. Krivenko.
from past.utils import old_div
from pytriqs.gf import *
from pytriqs.gf.descriptors import *
from pytriqs.utility import capture_stdout

import unittest
import sys

ref_output="""Before:
G['up'] = 3.1415926536E+00 j
G['dn'] = 3.1415926536E+00 j
After G['up'] += G['dn']:
G['up'] = 6.2831853072E+00 j
G['dn'] = 3.1415926536E+00 j
After g_up += g_dn:
G['up'] = 9.4247779608E+00 j
G['dn'] = 3.1415926536E+00 j
After G += G:
G['up'] = 1.8849555922E+01 j
G['dn'] = 6.2831853072E+00 j"""

def print_err(*x) : print (*x, file= sys.stderr)

with capture_stdout() as output:
    g_up = GfImFreq(indices = [0], beta = 1)
    g_dn = GfImFreq(indices = [0], beta = 1)
    
    g_up << iOmega_n
    g_dn << iOmega_n
    
    G = BlockGf(name_list=['up','dn'], block_list=[g_up,g_dn], make_copies=False)
    
    N = old_div(len(G.mesh),2)
    
    print("Before:")
    print("G['up'] = %.10E j"%G['up'].data[N,0,0].imag)
    print("G['dn'] = %.10E j"%G['dn'].data[N,0,0].imag)
    print_err('id(g_up) =', id(g_up))
    print_err('id(g_dn) =', id(g_dn))
    print_err ("(id=", id(G['up']),")")
    print_err ("(id=", id(G['dn']),")")
    
    G['up'] += G['dn']
    
    print("After G['up'] += G['dn']:")
    print("G['up'] = %.10E j"%G['up'].data[N,0,0].imag)
    print("G['dn'] = %.10E j"%G['dn'].data[N,0,0].imag)
    print_err('id(g_up) =', id(g_up))
    print_err('id(g_dn) =', id(g_dn))
    print_err ("(id=", id(G['up']),")")
    print_err ("(id=", id(G['dn']),")")
    
    
    g_up += g_dn
    
    print("After g_up += g_dn:")
    print("G['up'] = %.10E j"%G['up'].data[N,0,0].imag)
    print("G['dn'] = %.10E j"%G['dn'].data[N,0,0].imag)
    print_err('id(g_up) =', id(g_up))
    print_err('id(g_dn) =', id(g_dn))
    print_err ("(id=", id(G['up']),")")
    print_err ("(id=", id(G['dn']),")")
    
    
    G += G
    print("After G += G:")
    print("G['up'] = %.10E j"%G['up'].data[N,0,0].imag)
    print("G['dn'] = %.10E j"%G['dn'].data[N,0,0].imag)
    print_err('id(g_up) =', id(g_up))
    print_err('id(g_dn) =', id(g_dn))
    print_err ("(id=", id(G['up']),")")
    print_err ("(id=", id(G['dn']),")")

assert(output == ref_output.splitlines())
