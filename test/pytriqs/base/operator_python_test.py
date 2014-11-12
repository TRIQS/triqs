from pytriqs.operators import *
import itertools

C_list = [c(1,0),c(2,0),c(3,0)]
Cd_list = [c_dag(1,0), c_dag(2,0), c_dag(3,0)]

print "Anticommutators:"
for Cd,C in itertools.product(Cd_list,C_list):
	print "{", Cd, ",", C, "} =", Cd*C + C*Cd

print "Commutators:"
for Cd,C in itertools.product(Cd_list,C_list): 
	print "[", Cd, ",", C, "] =", Cd*C - C*Cd


# Algebra
x = c(0,0)
y = c_dag(1,0)

print
print "Algebra:"  
print "x =", x
print "y =", y

print "-x =", -x
print "x + 2.0 =", x + 2.0
print "2.0 + x =", 2.0 + x
print "x - 2.0 =", x - 2.0
print "2.0 - x =", 2.0 - x
print "3.0*y =", 3.0*y
print "y*3.0 =", y*3.0
print "x + y =", x + y
print "x - y =", x - y
print "(x + y)*(x - y) =", (x + y)*(x - y)

# N^3
print
print "N^3:"
N = n(0,'up') + n(0,'dn')
N3 = N*N*N
print "N =", N
print "N^3 =", N3
