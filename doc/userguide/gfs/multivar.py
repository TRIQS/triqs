from pytriqs.gf.multivar import *
from pytriqs.gf import *
from h5 import *
from pytriqs.plot.mpl_interface import *
from matplotlib.gridspec import GridSpec
from math import pi

##construct the mesh
beta=50.0
n_max=100
m1=MeshImFreq(beta=beta, S="Fermion", n_max=n_max) #fermionic Matsubara frequency mesh
m2=MeshImFreq(beta=beta, S="Boson", n_max=n_max) # bosonic
mprod=MeshImFreqImFreq(m1,m2) # mesh product of m1 and m2

##construct the Gf
U=1.0
l_block=GfImFreq_x_ImFreqTv3(mesh=mprod, shape=[1,1,1]) #instantiation of the function
Lambda=BlockGf(name_list = ["ch","sp"],block_list = [l_block,l_block])

##fill with expression
g_om = GfImFreq(mesh=m1, shape=[1,1])
w=lambda n: 2*n*pi/beta*1j
for n in range(-n_max+1,n_max):
    g_om << U**2/4*inverse(iOmega_n+w(n))*inverse(iOmega_n)
    Lambda["ch"].data[:,n+n_max-1,0,0,0]=g_om.data[:,0,0]

###plot
gs=GridSpec(1,2)

plt.subplot(gs[0], aspect="equal")
oplot(Lambda["ch"][0,0,0], type="contourf")
plt.xlim(-2,2)
plt.ylim(-2,2)
plt.colorbar()

plt.subplot(gs[1])
for n in [0,5,10]:
 oplot(Lambda["ch"][0,0,0].slice_at_const_w2(n), mode="R", x_window=(-2,2), label=r"$\Lambda^\mathrm{ch}(i\omega, i\Omega_{%s})$"%n)
plt.ylabel("");
plt.legend(loc="best")
plt.tight_layout()
plt.show()


