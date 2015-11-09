# Test from I. Krivenko.
from __future__ import print_function
from pytriqs.gf.local import *
from pytriqs.gf.local.descriptors import *
import sys
def print_err(*x) : print (*x, file= sys.stderr)

g_up = GfImFreq(indices = [0], beta = 1)
g_dn = GfImFreq(indices = [0], beta = 1)

g_up << iOmega_n
g_dn << iOmega_n

G = BlockGf(name_list=['up','dn'], block_list=[g_up,g_dn], make_copies=False)

N = len(G.mesh)/2

print("Before:")
print("G['up'] =", G['up'].data[N,0,0]) 
print("G['dn'] =", G['dn'].data[N,0,0]) 
print_err('id(g_up) =', id(g_up))
print_err('id(g_dn) =', id(g_dn))
print_err ("(id=", id(G['up']),")")
print_err ("(id=", id(G['dn']),")")

G['up'] += G['dn']

print("After G['up'] += G['dn']:")
print("G['up'] =", G['up'].data[N,0,0])
print("G['dn'] =", G['dn'].data[N,0,0])
print_err('id(g_up) =', id(g_up))
print_err('id(g_dn) =', id(g_dn))
print_err ("(id=", id(G['up']),")")
print_err ("(id=", id(G['dn']),")")


g_up += g_dn

print("After g_up += g_dn:")
print("G['up'] =", G['up'].data[N,0,0])
print("G['dn'] =", G['dn'].data[N,0,0])
print_err('id(g_up) =', id(g_up))
print_err('id(g_dn) =', id(g_dn))
print_err ("(id=", id(G['up']),")")
print_err ("(id=", id(G['dn']),")")


G += G
print("After G += G:")
print("G['up'] =", G['up'].data[N,0,0])
print("G['dn'] =", G['dn'].data[N,0,0])
print_err('id(g_up) =', id(g_up))
print_err('id(g_dn) =', id(g_dn))
print_err ("(id=", id(G['up']),")")
print_err ("(id=", id(G['dn']),")")


