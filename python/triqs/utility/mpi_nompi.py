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


r"""
Serial stub implementation of the TRIQS MPI interface.

Mirrors the public surface of :mod:`triqs.utility.mpi_mpi4py` for the
case where TRIQS is invoked outside of an MPI launcher. Every function
either returns its input unchanged, performs a no-op, or runs the
computation locally; the underlying :mod:`mpi4py` is never imported.

:mod:`triqs.utility.mpi` re-exports this module when
:func:`triqs.utility.mpi.check_for_mpi` returns ``False``.

Attributes
----------
world : None
    Placeholder for the communicator. Always ``None`` in serial.
rank : int
    Always ``0``.
size : int
    Always ``1``.
All_Nodes_report : bool
    Provided for API compatibility with the parallel backend; has no
    effect here. Default ``False``.
"""

# this module provides a simple dummy class for MPI in case no MPI env
# is loaded and triqs is executed in serial

import os,sys,datetime
myprint_err = lambda x : sys.stderr.write("%s\n"%x)
myprint_err.__doc__ = "Write ``x`` followed by a newline to ``sys.stderr``."

myprint_out = lambda x : sys.stdout.write("%s\n"%x)
myprint_out.__doc__ = "Write ``x`` followed by a newline to ``sys.stdout``."

world = None
rank = 0
size = 1

All_Nodes_report = False

# variable for send and receive
_sendval = None

myprint_err ("Starting serial run at: %s"%(str(datetime.datetime.now())))


# next are simplified implementations of all mpi functions triqs provides

def report(*x,**opt):
        r"""
        Print one line per argument to ``stdout`` (or ``stderr``).

        Serial counterpart of :func:`triqs.utility.mpi_mpi4py.report`.
        Always prints, since there is only a single rank.

        Parameters
        ----------
        *x : object
            Values to print, one per line.
        **opt
            Optional keyword arguments. ``stderr`` (bool): when truthy,
            write to ``sys.stderr`` instead of ``sys.stdout``.
        """
        myprint,myflush = (myprint_err,sys.stderr.flush)  if 'stderr' in opt and opt['stderr'] else (myprint_out,sys.stdout.flush)
        for y in x:
          myprint(y)  # open('report','a').write(str(y) + '\n') #             print y
          myflush() # be sure to flush the buffer!


def is_master_node():
    """Return ``True``; in serial there is only the master rank."""
    return True

def bcast(x, root = 0):
    r"""
    Serial broadcast: return ``x`` unchanged.

    Parameters
    ----------
    x : object
        Value to (pretend to) broadcast.
    root : int, optional
        Ignored; accepted for API compatibility with
        :func:`triqs.utility.mpi_mpi4py.bcast`. Default 0.

    Returns
    -------
    object
        The input ``x``, returned as-is.
    """
    return x

def barrier(poll_msec=1) :
    r"""
    No-op barrier provided for API compatibility.

    Parameters
    ----------
    poll_msec : float, optional
        Ignored; accepted for compatibility with
        :func:`triqs.utility.mpi_mpi4py.barrier`. Default 1.
    """
    return

# op doesn't do anything in serial so Ops like MINLOC or MAXLOC could fail
def all_reduce(x, comm=world, op=lambda x, y: x + y):
    r"""
    Serial reduction: return ``x`` unchanged.

    Operation ``op`` is ignored, so reductions such as ``MINLOC`` or
    ``MAXLOC`` that produce non-scalar values may give different
    results than their parallel counterparts; callers must therefore
    not rely on the reduced value being computed in serial.

    Parameters
    ----------
    x : object
        Local contribution. Returned unchanged.
    comm : object, optional
        Ignored; accepted for API compatibility. Default ``world``
        (``None`` in serial).
    op : callable, optional
        Reduction operation. Ignored in this stub.

    Returns
    -------
    object
        The input ``x``, returned as-is.

    Notes
    -----
    For backwards compatibility with an older signature, when ``x`` is
    ``None`` the function prints a warning and returns ``comm`` to
    mimic the legacy call.
    """
    if x is None:
        report(
            "WARNING: the signature for all_reduce is now 'all_reduce(x, comm=world, op=MPI.SUM)'\n\tattempting compatibility with old signature"
        )
        return comm
    return x

def send(val, dest):
    r"""
    Serial send: store ``val`` so that the next :func:`recv` can retrieve it.

    Provides a minimal in-process queue so user code that calls
    ``send``/``recv`` in pairs still works when running serially.

    Parameters
    ----------
    val : object
        Value to enqueue.
    dest : int
        Ignored; accepted for API compatibility.
    """
    _sendval = val
    return

def recv(source = 0):
    r"""
    Serial receive: return the value previously stored by :func:`send`.

    Parameters
    ----------
    source : int, optional
        Ignored; accepted for API compatibility. Default 0.

    Returns
    -------
    object
        The value that was passed to the most recent :func:`send`.

    Raises
    ------
    Exception
        If no :func:`send` has been issued since the last :func:`recv`.
    """
    if _sendval is None:
        raise Exception('send val not set')
    recval = _sendval
    _sendval = None
    return recval

def slice_array(A) :
    r"""
    Serial slice: return ``A`` unchanged.

    The parallel counterpart, :func:`triqs.utility.mpi_mpi4py.slice_array`,
    returns a view over the first axis of ``A`` corresponding to the
    current rank. In serial the whole array belongs to the single
    rank, so it is returned as-is.

    Parameters
    ----------
    A : numpy.ndarray
        Array to (pretend to) slice.

    Returns
    -------
    numpy.ndarray
        ``A`` itself.
    """
    return A

HostNames = {}
def master_gets_host_names():
    r"""
    Print the local hostname.

    Serial counterpart of
    :func:`triqs.utility.mpi_mpi4py.master_gets_host_names`. Only one
    node ever exists, so the function simply prints the result of
    :func:`socket.gethostname` for that node.
    """
    from socket import gethostname
    print("Hostnames : ")
    print("Node 0  on machine %s"%(gethostname()))

