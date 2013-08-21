from pytriqs.operators import *

H = C('up',1) * Cdag('up',2) + C('up',2) * Cdag('up',1)
print H
print H - H.dagger()

print anti_commutator(C('up'),Cdag('up'))
print anti_commutator(C('up'),0.5*Cdag('down'))
