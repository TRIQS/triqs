import pytriqs.parameters.parameters as P
p = P.Parameters()
p['i'] = 10
p['d'] = 2.3 
p['s'] = "my string"

print p

print p['i']
print p['d']
print p['s']

d = { 'a' : 10, 'b' : "good", 'c' : -9.2}
p.update(d)
print p

import numpy as np
a = np.array([[1,2],[3,4]])
p['A'] = a
p['Ad'] = 2.5*a

print " p['A'] = ", p['A'] 

p['A'][0,0] *=10
print " p['A'] = ", p['A'] 

print p

p2 = P.Parameters()
p2.update2( a = 4, b= "u")

p['sub'] = p2
p['sub2'] = { 'r' : 1, 'rrr' : 3}

print p['sub']
print p['sub2']

print p

