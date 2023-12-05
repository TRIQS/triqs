import numpy as np
from cpp2py.compiler import compile

filename = 'clef_speed.cpp'

code = ''.join(open(filename, 'r').readlines())

cxxflags = '-O3 -march=native -Ofast -mavx -mfma -mavx2 -g -ggdb -Wno-invalid-partial-specialization '

includes = ' -I /opt/local/include/ -I /opt/local/include/openmpi-clang40 '

M = compile(code, modules = "pytriqs", cxxflags=cxxflags + includes)

print '--> Running clef timing tests'
tests = np.array([
    getattr(M, method) for method in M.__dict__.keys() if 'test_' in method])
times = np.array([test(n=80, m=80) for test in tests])

sidx = np.argsort(times)[::-1]
times, tests = times[sidx], tests[sidx]
    
for time, test in zip(times, tests):
    print '%40s -- %10.2f ms' % (test.__name__, 1000.0 * time)
