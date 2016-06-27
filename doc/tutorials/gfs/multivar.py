from pytriqs.gf.local.multivar import *
from pytriqs.gf.local import *
from pytriqs.archive import *
from pytriqs.plot.mpl_interface import *
from math import pi

##construct the mesh
beta=10.0
m1=MeshImFreq(beta=beta, S="Fermion", n_max=100) #fermionic Matsubara frequency mesh
m2=MeshImFreq(beta=beta, S="Boson", n_max=100) # bosonic
mprod=MeshImFreqImFreq(m1,m2) # mesh product of m1 and m2


##construct the Gf
U=1.0
g=GfImFreq_x_ImFreqTv3(mesh=mprod, shape=[1,1,1]) #instantiation of the function
G=BlockGf(name_list = ["ch","sp"],block_list = [g,g])

##fill with expression
g_om = GfImFreq(mesh=m1, shape=[1,1])
w=lambda n: 2*n*pi/beta*1j
for n in range(-100,99):
 g_om << U**2/4*inverse(iOmega_n+w(n))*inverse(iOmega_n)
 G["ch"].data[:,n+100,0,0,0]=g_om.data[:,0,0]+0.3*n #the shift 0.4*n is just for plotting purposes

###plot
for n in range(-10,10):
 oplot(G["ch"][0,0,0].slice_at_const_w2(n),'-r' if n%5==0 else '--k', mode="R", x_window=(-6,6), label=None)
ylabel("")
