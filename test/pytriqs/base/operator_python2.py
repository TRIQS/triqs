from pytriqs.archive import *
from pytriqs.operators import *

op1 = n("up",0)
op2 = n("dn",0)
ops = [op1, op2]

# Write list of operators
with HDFArchive('list_of_ops.h5', 'w') as arch:
    arch['ops'] = ops
    arch['op1'] = op1

# Read list
arch = HDFArchive('list_of_ops.h5', 'r')
assert (arch['op1'] - op1).is_zero(), "pb ..."
assert all([(x - y).is_zero() for x,y in zip(arch['ops'],ops)]), "oops"


