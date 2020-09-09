# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Nils Wentzell, tayral

import numpy as np
import math
def dist(x,y):
 '''return the 2D distance between x and y'''
 return math.sqrt((x[0]-y[0])**2+(x[1]-y[1])**2)


def pick_selection_vec(data, stride):
 Nmomentum = len(data) 
 data_selected=[]
 for k in range(0,Nmomentum,stride): data_selected.append(data[k])
 return data_selected


def closest_point_in_line( x, L):
 '''return the index of the closest point to x in the list of points L'''

 mins=[]
 dists=[]

 if len(L)>0 :
  min_dist = float('inf');
  min_index = 0;
  ik=0
  for k in L:
   d=dist(x,k);
   dists.append(d)
   if d<=min_dist: 
    min_dist = d;
    min_index = ik;
   ik+=1
  for i in range(len(L)):
   if dists[i] == min_dist: mins.append(i)

  return mins;
 raise Exception("Empty mesh")


def closest_to(A,B, L, verbose=False):
 '''returns list of indices of the points in list L (or mesh!) that are closest to the oriented line A->B'''
 A = np.array(A) 
 B = np.array(B) 
 closest_indices=[]
 closest_indices_final=[]
 #discretization of the line
 Nmesh =2;#at least two points, A and B

 size_before = -1;
 while len(closest_indices)!=size_before:

  size_before = len(closest_indices)
  closest_indices=[]
  closest_indices_final=[]

  for i in range(Nmesh):
   current_point = A +(B-A)*(i*1.0/(Nmesh-1));
   if verbose: print(" current_point is ",current_point)
   closest = closest_point_in_line(current_point,L)[0];
#print " closest point is ",closest
   if closest_indices.count(closest)==0: 
     closest_indices_final.append(closest);
     closest_indices.append(closest);

#print "** closest_index size = ",len(closest_indices)

  Nmesh*=2;
 
 return closest_indices_final;

def select_path_indices(path,mesh,verbose=False):
  path_indices=[]
  closest_indices=[]
  high_sym_pts_indices=[]
  for i in range(len(path)-1):
   start = path[i];end = path[i+1];
   closest_indices = closest_to(start,end,mesh);
   if i==0:  
    high_sym_pts_indices.append((0,closest_indices[0]))
    ratio = dist(start,end)/ len(closest_indices)
   stride = int(round(len(closest_indices)/dist(start,end)*ratio ))
   #if verbose: print "stride = ",stride
   closest_indices = pick_selection_vec(closest_indices,stride )
   path_indices.extend( closest_indices)
   high_sym_pts_indices.append((len(path_indices),closest_indices[-1]))
  if verbose==False:
   return path_indices
  else:
   return path_indices,high_sym_pts_indices
