# Copyright (c) 2021-2023 Simons Foundation
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
# Authors: John Bonini, Alexander Hampel, Nils Wentzell


# this module provides a simple dummy class for MPI in case no MPI env
# is loaded and triqs is executed in serial

import os,sys,datetime
myprint_err = lambda x : sys.stderr.write("%s\n"%x)
myprint_out = lambda x : sys.stdout.write("%s\n"%x)

world = None
rank = 0
size = 1

All_Nodes_report = False

# variable for send and receive
_sendval = None

myprint_err ("Starting serial run at: %s"%(str(datetime.datetime.now())))


# next are simplified implementations of all mpi functions triqs provides

def report(*x,**opt):
        myprint,myflush = (myprint_err,sys.stderr.flush)  if 'stderr' in opt and opt['stderr'] else (myprint_out,sys.stdout.flush)
        for y in x:
          myprint(y)  # open('report','a').write(str(y) + '\n') #             print y
          myflush() # be sure to flush the buffer!


def is_master_node(): return True

def bcast(x, root = 0): return x

def barrier(poll_msec=1) : return

# op doesn't do anything in serial so Ops like MINLOC or MAXLOC could fail
def all_reduce(x, comm=world, op=lambda x, y: x + y):
    if x is None:
        report(
            "WARNING: the signature for all_reduce is now 'all_reduce(x, comm=world, op=MPI.SUM)'\n\tattempting compatibility with old signature"
        )
        return comm
    return x

def send(val, dest):
    _sendval = val
    return

def recv(source = 0):
    if _sendval is None:
        raise Exception('send val not set')
    recval = _sendval
    _sendval = None
    return recval

def slice_array(A) :
    """Given an array A, it returns a VIEW of a slice over the first dim on the node"""
    return A

HostNames = {}
def master_gets_host_names():
    """ find the host name of all the nodes """
    from socket import gethostname
    print("Hostnames : ")
    print("Node 0  on machine %s"%(gethostname()))

