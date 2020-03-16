from pytriqs.gf import *
from pytriqs.utility import capture_stdout

ref_output="""['0', '1', '2']
['0', '1'] ['0']
['a', 'b', 'c']
['a', 'b'] ['a']"""

with capture_stdout() as output:
    g = GfImFreq(beta=10, indices=[0,1,2])
    print(g.indices[0])
    print(g[0:2,0:1].indices[0], g[0:2,0:1].indices[1])
    
    g = GfImFreq(beta=10, indices=['a','b','c'])
    print(g.indices[0])
    print(g[0:2,0:1].indices[0], g[0:2,0:1].indices[1])

assert(output == ref_output.splitlines())
