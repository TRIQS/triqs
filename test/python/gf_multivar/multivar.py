# Copyright (c) 2016-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2016 Igor Krivenko
# Copyright (c) 2020 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell, tayral

from triqs.gf import *
from h5 import *
from itertools import product

##construct product mesh
m1=MeshImFreq(beta=1., S="Fermion", n_iw=100)
m2=MeshImFreq(beta=1., S="Boson", n_iw=100)
mprod=MeshProduct(m1,m2)

assert len(mprod)==len(m1)*len(m2), "not ok: %s"%(mprod.size)
assert len(mprod.components[0])==len(m1), "not ok: %s"%len(mprod.components[0])
assert len(mprod.components[1])==len(m2), "not ok: %s"%len(mprod.components[1])

########################################
######### scalar-valued ################
########################################
##construct two-freq Gf
g=Gf(mesh=mprod, target_shape = [])

assert len(g.mesh)==len(mprod), "not ok"

##assert len(g.slice_at_const_w1(4).mesh)==len(m2), "not ok"

assert g.data.shape==(200,199)

## test h5
with HDFArchive("g_multivar.h5",'w') as A:
  A["g"] = g
  A["mprod"] = mprod
  
with HDFArchive("g_multivar.h5",'r') as A2:
    mprod2=A2["mprod"]
    g2 = A2["g"]

assert len(g.mesh) == len(g2.mesh), "not ok"

G=BlockGf(name_list = ["ch","sp"],block_list = [g,g])

A=HDFArchive("g_multivar_2.h5",'w')
A["G"] = G
del A

A2=HDFArchive("g_multivar_2.h5",'r')
G2 = A2["G"]
del A2

########################################
######### tensor-valued ################
########################################
##construct two-freq Gf, tensor-valued
g=Gf(mesh = mprod, target_shape = [1,1,1])
assert g.data.shape==(200,199, 1, 1,1)

## DEBG
##assert g.slice_at_const_w1(4).data.shape==(199,1,1,1)
##assert g.slice_at_const_w2(4).data.shape==(200,1,1,1)

#print evaluate(g.tail,0)

#G=BlockGfImFreq_x_ImFreq_s(["ch","sp"])
G=BlockGf(name_list = ["ch","sp"],block_list = [g,g])
assert G['ch'].data.shape==(200,199,1,1,1)

A=HDFArchive("g_multivar_3.h5",'w')
A["G"] = G
del A

#mpi bcast
import triqs.utility.mpi as mpi
g2=Gf(mesh = mprod, target_shape = [1,1,1])
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
m1=MeshImTime(beta=1., S="Fermion", n_tau=100)
m2=MeshImTime(beta=1., S="Boson", n_tau=100)
mprod=MeshProduct(m1,m2)
g=Gf(mesh = mprod, target_shape = [1,1,1])
f=Gf(mesh = mprod, target_shape =[1,1,1])
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

# Legendre
m1 = MeshImFreq(beta=2., S = "Boson", n_iw=20)
m2 = MeshLegendre(beta=2., S = "Fermion", max_n=10)
m3 = MeshLegendre(beta=2., S = "Fermion", max_n=10)
mprod = MeshProduct(m1,m2,m3)
assert len(mprod) == len(m1)*len(m2)*len(m3)

g = Gf(mesh = mprod, target_shape = [2,2,2,2])

for iw,l,lp in product(list(range(len(m1))),list(range(len(m2))),list(range(len(m3)))):
    g.data[iw,l,lp,...] = iw + l - lp

# for iw in range(len(m1)):
    # gs = g.slice_at_const_w1(iw-19)
    # for l,lp in product(range(len(m2)),range(len(m3))):
        # assert (gs.data[l,lp,...] == iw + l - lp).all()
# for l in range(len(m2)):
    # gs = g.slice_at_const_w2(l)
    # for iw, lp in product(range(len(m1)),range(len(m3))):
        # assert (gs.data[iw,lp,...] == iw + l - lp).all()
    # gs = g.slice_at_const_w3(l)
    # for iw, lp in product(range(len(m1)),range(len(m3))):
#         assert (gs.data[iw,lp,...] == iw - l + lp).all()
