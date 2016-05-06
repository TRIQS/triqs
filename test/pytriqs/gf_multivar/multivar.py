from pytriqs.gf.local.multivar import *
from pytriqs.gf.local import *
from pytriqs.archive import *

##construct product mesh
m1=MeshImFreq(beta=1., S="Fermion", n_max=100)
m2=MeshImFreq(beta=1., S="Boson", n_max=100)
mprod=MeshImFreqImFreq(m1,m2)

assert  mprod.size==len(m1)*len(m2), "not ok: %s"%(mprod.size)
assert len(mprod.components[0])==len(m1), "not ok: %s"%len(mprod.components[0])
assert len(mprod.components[1])==len(m2), "not ok: %s"%len(mprod.components[1])

########################################
######### scalar-valued ################
########################################
##construct two-freq Gf
g=GfImFreq_x_ImFreq_s(mesh=mprod)

assert g.mesh.size==mprod.size, "not ok"
assert len(g.slice_at_const_w1(4).mesh)==len(m2), "not ok"
assert g.data.shape==(200,199)

##test h5
A=HDFArchive("g_multivar.h5",'a')
A["g"] = g
A["mprod"]=mprod
del A

A2=HDFArchive("g_multivar.h5",'r')
g2 = A2["g"]
mprod2=A2["mprod"]

assert g.mesh.size==g2.mesh.size, "not ok"

#G=BlockGfImFreq_x_ImFreq_s(["ch","sp"])
G=BlockGf(name_list = ["ch","sp"],block_list = [g,g])

A=HDFArchive("g_multivar_2.h5",'a')
A["G"] = G
del A

A2=HDFArchive("g_multivar_2.h5",'r')
G2 = A2["G"]
del A2

########################################
######### tensor-valued ################
########################################
##construct two-freq Gf, tensor-valued
g=GfImFreq_x_ImFreqTv3(mprod, [1,1,1])
assert g.data.shape==(200,199, 1, 1,1)
assert g.slice_at_const_w1(4).data.shape==(199,1,1,1)
assert g.slice_at_const_w2(4).data.shape==(200,1,1,1)
print g.tail
#print evaluate(g.tail,0)

#G=BlockGfImFreq_x_ImFreq_s(["ch","sp"])
G=BlockGf(name_list = ["ch","sp"],block_list = [g,g])
assert G['ch'].data.shape==(200,199,1,1,1)

A=HDFArchive("g_multivar_3.h5",'a')
A["G"] = G
del A

#mpi bcast
import pytriqs.utility.mpi as mpi
g2=GfImFreq_x_ImFreqTv3(mprod, [1,1,1])
if mpi.is_master_node():
 g2.data[:,:,:,:,:] = 5
 assert g2.data[0,0,0,0,0] == 5, "not ok : %s"%(g2.data[0,0,0,0,0])
if not mpi.is_master_node():
 assert g2.data[0,0,0,0,0] == 0, "not ok : %s"%(g2.data[0,0,0,0,0])
g2 = mpi.bcast(g2)
if not mpi.is_master_node():
 assert g2.data[0,0,0,0,0] == 5, "not ok : %s"%(g2.data[0,0,0,0,0])

#ImTime
##construct product mesh
m1=MeshImTime(beta=1., S="Fermion", n_max=100)
m2=MeshImTime(beta=1., S="Boson", n_max=100)
mprod=MeshImTimeImTime(m1,m2)
g=GfImTime_x_ImTimeTv3(mprod, [1,1,1])
f=GfImTime_x_ImTimeTv3(mprod, [1,1,1])
g.data[:]=2.5
f.data[:]=2.5
#operations
#inplace
f+=g
g*=2.0
assert g.data[0,0,0,0,0]==5.0, "not ok: %s"%g.data[0,0,0,0,0]
assert f.data[0,0,0,0,0]==5.0, "not ok: %s"%f.data[0,0,0,0,0]
g_mul = 2*g
assert g_mul.data[0,0,0,0,0]==10.0, "not ok: %s"%g_mul.data[0,0,0,0,0]
g_add = 2*g+g
assert g_add.data[0,0,0,0,0]==15.0, "not ok: %s"%g_add.data[0,0,0,0,0]
G=BlockGf(name_list=["a","b"], block_list=[g,g], make_copies=True)
F=BlockGf(name_list=["a","b"], block_list=[2*g,g], make_copies=True)

H = G+F
assert H["a"].data[0,0,0,0,0]==15.0, "not ok: %s"%H["a"].data[0,0,0,0,0]
assert H["b"].data[0,0,0,0,0]==10.0, "not ok: %s"%H["b"].data[0,0,0,0,0]

