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
# Authors: Thomas Ayral, Michel Ferrero, Nils Wentzell

r"""Helpers to select a path of mesh points through a 
Brillouin-zone-like mesh.

Used by :mod:`.bz` and :mod:`.bz_x_x` to draw band-structure-style 
plots along a piecewise-linear path through k-space.
"""

import numpy as np
import math
def dist(x,y):
 '''Euclidean 2D distance between ``x`` and ``y``.

 Parameters
 ----------
 x, y : sequence of float of length 2
     ``(x0, x1)`` coordinates.

 Returns
 -------
 float
     :math:`\\sqrt{(x_0 - y_0)^2 + (x_1 - y_1)^2}`.
 '''
 return math.sqrt((x[0]-y[0])**2+(x[1]-y[1])**2)


def pick_selection_vec(data, stride):
 """Return ``data[::stride]`` (every ``stride``-th element).

 Parameters
 ----------
 data : sequence
     Input sequence.
 stride : int
     Decimation factor.

 Returns
 -------
 list
     Selected elements.
 """
 Nmomentum = len(data)
 data_selected=[]
 for k in range(0,Nmomentum,stride): data_selected.append(data[k])
 return data_selected


def closest_point_in_line( x, L):
 '''Return the indices of the points in ``L`` closest to ``x`` 
 (ties returned in order).

 Parameters
 ----------
 x : sequence of float of length 2
     Query point.
 L : sequence of points
     Candidate points to compare against.

 Returns
 -------
 list of int
     Indices in ``L`` whose distance to ``x`` equals the minimum.

 Raises
 ------
 Exception
     If ``L`` is empty.
 '''

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
 '''Return the indices of the points in ``L`` that lie closest to 
 the oriented segment ``A -> B``.

 Parameters
 ----------
 A, B : sequence of float of length 2
     End-points of the segment.
 L : sequence of points
     Candidate points (e.g. a mesh).
 verbose : bool, optional
     If ``True``, print the discretised line during the refinement.

 Returns
 -------
 list of int
     Indices in ``L`` along the segment.
 '''
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
  '''Return mesh indices tracing a piecewise-linear ``path`` 
  through ``mesh``.

  Parameters
  ----------
  path : sequence of points
      Vertices of the path, in order.
  mesh : iterable of points
      Available mesh points.
  verbose : bool, optional
      If ``True``, also return a list of ``(path_index, mesh_index)``
      pairs marking the high-symmetry points.

  Returns
  -------
  list of int
      Mesh indices along the path.
  list of (int, int), optional
      Only returned when ``verbose=True``: ``(path_index, mesh_index)``
      pairs at each high-symmetry point.
  '''
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
