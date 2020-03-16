from builtins import range
from pytriqs.gf import *
from pytriqs.gf.gf_fnt import call_vec, call_s

beta = 10.0
n = 2

g= GfImFreq(beta=beta, indices=[0])

print(g.data.shape, g.indices.data)

def run() : 
    for i in range(10000):
      g(2)

def run_s() : 
    for i in range(10000):
      call_s(g)


def run_vec() : 
   return call_vec(g)




