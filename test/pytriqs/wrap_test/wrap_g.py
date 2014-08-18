from pytriqs.wrap_test.test_g import *

B = make_bgf(2.0)
pass_bgf(B+B)

# run in python the h5diff between the 2 files...

g = make_sgf(2.0)
pass_sgf(g)


