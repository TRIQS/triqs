# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Olivier Parcollet, Nils Wentzell, Alexander Hampel


# this module provides a simple dummy class for MPI in case no MPI env
# is loaded and triqs is executed in serial

import os,sys,datetime
myprint_err = lambda x : sys.stderr.write("%s\n"%x)
myprint_out = lambda x : sys.stdout.write("%s\n"%x)

world = None
rank = 0
size = 1

myprint_err ("Starting serial run at: %s"%(str(datetime.datetime.now())))


# next are simplified implementations of all mpi functions triqs provides

def report(*x,**opt):
        myprint,myflush = (myprint_err,sys.stderr.flush)  if 'stderr' in opt and opt['stderr'] else (myprint_out,sys.stdout.flush)
        for y in x:
          myprint( y)  # open('report','a').write(str(y) + '\n') #             print y
          myflush() # be sure to flush the buffer!


def is_master_node(): return True

def bcast(x): return x

def barrier() : return True

def all_reduce(WORLD, x, F) : return x

def send(val, dest): return True

# this one I am not sure how to implement
def recv(source = 0): return True

def slice_inf(imin,imax) :
  return imin

def slice_sup(imin,imax) :
  return imax

def slice_array(A) :
    """Given an array A, it returns a VIEW of a slice over the first dim on the node"""
    imax = A.shape[0]-1
    return A[slice_inf(0,imax):slice_sup(0,imax)+1] # +1 due to the slice convention

HostNames = {}
def master_gets_host_names():
    """ find the host name of all the nodes """
    from socket import gethostname
    print("Hostnames : ")
    print("Node 0  on machine %s"%(gethostname()))

