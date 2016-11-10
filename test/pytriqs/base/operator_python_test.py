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
print "Operator(3.14) =", Operator(3.14)

# N^3
print
print "N^3:"
N = n(0,'up') + n(0,'dn')
N3 = N*N*N
print "N =", N
print "N^3 =", N3

print "Monomials of N^3:"
for monomial, coef in N3: print monomial, coef

# Dagger, real & imag
X = (1+2j)*c_dag('a',1)*c_dag('a',2)*c('a',3)*c('a',4);

print
print "X =", X
print "dagger(X) =", dagger(X)
print "X.real =", X.real
print "X.imag =", X.imag
