# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2023 Simons Foundation
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
# Authors: John Bonini, Alexander Hampel, Olivier Parcollet, Nils Wentzell


r"""
:mod:`mpi4py`-backed implementation of the TRIQS MPI interface.

This module is selected at runtime by :mod:`triqs.utility.mpi` when an
MPI launcher is detected. It exposes a thin, TRIQS-specific wrapper
around the :mod:`mpi4py` API plus a few utility functions for slicing
arrays per rank and printing only on the master.

A parallel serial stub with the same public surface lives in
:mod:`triqs.utility.mpi_nompi`.

Attributes
----------
world : mpi4py.MPI.Comm
    The default communicator (``MPI.COMM_WORLD``).
rank : int
    Rank of the current process inside ``world``.
size : int
    Total number of processes in ``world``.
master : int
    Rank of the master process. Always 0.
All_Nodes_report : bool
    When ``True``, :func:`report` writes one file per non-master rank
    (``report_node<rank>``) instead of dropping their output. Default
    ``False``.
Verbosity_Level_Report_Max : int
    Upper bound on the ``Verbosity_Level`` keyword accepted by
    :func:`report`; messages above this threshold are suppressed.
    Default 1.
"""

import os,sys,datetime, time
myprint_err = lambda x : sys.stderr.write("%s\n"%x)
myprint_err.__doc__ = "Write ``x`` followed by a newline to ``sys.stderr``."

myprint_out = lambda x : sys.stdout.write("%s\n"%x)
myprint_out.__doc__ = "Write ``x`` followed by a newline to ``sys.stdout``."

from mpi4py import MPI

world = MPI.COMM_WORLD
rank = world.Get_rank()
size = world.Get_size()

if rank==0 :
    myprint_err ("Starting run with %s MPI rank(s) at : %s"%(size,str(datetime.datetime.now())))

master =0

All_Nodes_report = False

def bcast(x, root = 0):
    r"""
    Broadcast a Python object from one rank to all ranks.

    Thin wrapper around :meth:`mpi4py.MPI.Comm.bcast` on ``world``.

    Parameters
    ----------
    x : object
        Object to broadcast. Only meaningful on rank ``root``; ignored
        on all other ranks.
    root : int, optional
        Rank that holds the value to broadcast. Default 0.

    Returns
    -------
    object
        The broadcast value, identical on every rank.
    """
    return world.bcast(x, root = root)

def send(val, dest):
    r"""
    Send a Python object to another rank.

    Thin wrapper around :meth:`mpi4py.MPI.Comm.send` on ``world``.

    Parameters
    ----------
    val : object
        Object to transmit.
    dest : int
        Rank of the destination process.
    """
    world.send(val, dest = dest)
    #print "node ", rank, " has sent ", val, " to node ", dest

def recv(source = 0):
    r"""
    Receive a Python object from another rank.

    Thin wrapper around :meth:`mpi4py.MPI.Comm.recv` on ``world``.

    Parameters
    ----------
    source : int, optional
        Rank to receive from. Default 0.

    Returns
    -------
    object
        The received value.
    """
    r = world.recv(source = source)
    #print "node ", rank, " receives ", r, " from node ", source
    return r


def barrier(poll_msec=1):
    r"""
    Synchronisation barrier that yields the CPU while waiting.

    Uses a non-blocking :meth:`mpi4py.MPI.Comm.Ibarrier` followed by a
    :meth:`time.sleep` loop on its ``Test()``, so that subprocesses
    invoked by the user (e.g. solvers running outside Python) can use
    the otherwise idle CPU. When ``poll_msec`` is falsy (``0`` or
    ``None``), falls back to the standard blocking
    :meth:`mpi4py.MPI.Comm.barrier`.

    Parameters
    ----------
    poll_msec : float, optional
        Sleep duration in milliseconds between successive ``Test()``
        calls. Default 1.
    """
    if not poll_msec:
        world.barrier()
    else:
        req = world.Ibarrier()
        while not req.Test():
            time.sleep(poll_msec / 1000)

def all_reduce(x, comm=world, op=MPI.SUM):
    r"""
    Reduce ``x`` across all ranks of ``comm`` and return the result.

    Thin wrapper around :meth:`mpi4py.MPI.Comm.allreduce`.

    Parameters
    ----------
    x : object
        Value contributed by the local rank.
    comm : mpi4py.MPI.Comm, optional
        Communicator over which to reduce. Default :data:`world`.
    op : mpi4py.MPI.Op, optional
        Reduction operation. Default ``MPI.SUM``.

    Returns
    -------
    object
        The reduced value, identical on every rank of ``comm``.

    Notes
    -----
    For backwards compatibility with an earlier signature that had the
    communicator as the first positional argument, if the call raises
    :class:`AttributeError` or :class:`TypeError` and ``x`` is itself a
    communicator, the call is retried as ``world.allreduce(comm)`` and
    a warning is printed.
    """
    try:
        return comm.allreduce(x, op=op)
    except (AttributeError, TypeError) as e:
        if isinstance(x, MPI.Intracomm):
            report(
                "WARNING: the signature for all_reduce is now 'all_reduce(x, comm=world, op=MPI.SUM)'\n\tattempting compatibility with old signature"
            )
            return world.allreduce(comm)
        raise e


Verbosity_Level_Report_Max = 1
def report(*x,**opt):
    r"""
    Print to ``stdout`` (or ``stderr``) on the master rank only.

    Parameters
    ----------
    *x : object
        Values to print, one per line.
    **opt
        Optional keyword arguments:

        - ``Verbosity_Level`` (int) -- if greater than the module-level
          :data:`Verbosity_Level_Report_Max`, the call is silently
          dropped.
        - ``stderr`` (bool) -- if truthy, write to ``sys.stderr``
          instead of ``sys.stdout``.

    Notes
    -----
    When the module-level :data:`All_Nodes_report` flag is ``True``,
    non-master ranks additionally append their messages to per-rank
    files named ``report_node<rank>``.
    """
    try :
        if opt['Verbosity_Level'] >  Verbosity_Level_Report_Max : return
    except :
        pass
    if All_Nodes_report and rank!=0 :
          for y in x:
              open('report_node%s'%rank,'a').write(str(y) + '\n') #             print y

    if rank==0 :
        myprint,myflush = (myprint_err,sys.stderr.flush)  if 'stderr' in opt and opt['stderr'] else (myprint_out,sys.stdout.flush)
        for y in x:
          myprint( y)  # open('report','a').write(str(y) + '\n') #             print y
          myflush() # be sure to flush the buffer!

def slice_inf(imin,imax) :
  r"""
  Lower index of the local slice of the integer range ``[imin, imax]``.

  Splits the inclusive range ``[imin, imax]`` into :data:`size`
  contiguous chunks as evenly as possible and returns the first index
  of the chunk owned by the current rank.

  Parameters
  ----------
  imin, imax : int
      Inclusive lower and upper bounds of the global range to split.

  Returns
  -------
  int
      Lower index of the local chunk.
  """
  j=(imax - imin + 1)//size
  i= imax - imin + 1 - size*j
  return imin + rank*(j+1)  if  rank<=i-1 else imin + rank*j + i

def slice_sup(imin,imax) :
  r"""
  Upper (inclusive) index of the local slice of ``[imin, imax]``.

  Companion to :func:`slice_inf`. Returns the last index of the chunk
  owned by the current rank.

  Parameters
  ----------
  imin, imax : int
      Inclusive lower and upper bounds of the global range to split.

  Returns
  -------
  int
      Upper (inclusive) index of the local chunk.
  """
  j=(imax - imin + 1)//size
  i= imax - imin + 1 - size*j;
  return imin + (rank+1)*(j+1) -1  if  rank<=i-1  else imin + (rank+1)*j  + i - 1

def slice_array(A) :
    r"""
    Return the local view of ``A`` along its first dimension.

    Computes ``[slice_inf(0, N - 1), slice_sup(0, N - 1) + 1]`` with
    ``N = A.shape[0]`` and returns the corresponding slice. The result
    is a view on ``A`` (no copy).

    Parameters
    ----------
    A : numpy.ndarray
        Array to slice along its first dimension.

    Returns
    -------
    numpy.ndarray
        View on the local chunk of ``A``.
    """
    imax = A.shape[0]-1
    return A[slice_inf(0,imax):slice_sup(0,imax)+1] # +1 due to the slice convention

def is_master_node():
    """Return ``True`` on the master rank (``rank == master``)."""
    return rank == master

HostNames = {}
def master_gets_host_names():
    r"""
    Collect the hostnames of every rank on the master.

    Each non-master rank sends :func:`socket.gethostname` to rank
    ``master``, which stores the results in the module-level
    ``HostNames`` dictionary and prints them as ``Node <rank> on
    machine <hostname>``. A :func:`barrier` is taken at the end to
    keep the ranks in sync.
    """
    from socket import gethostname
    global HostNames
    HostNames[rank] = gethostname()
    if is_master_node() :
      for proc in range (1,size) :
        HostNames[proc],status = recv(proc)
    else:
      send(gethostname(),master)
    barrier()
    assert len(HostNames)==size," internal pb MPI module"
    
    if is_master_node() :
      print("Hostnames : ")
      for u,host in list(HostNames.items()) :
        print("Node %d  on machine %s"%(u,host))

