# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
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
# Authors: Olivier Parcollet, Nils Wentzell

r"""
Pipe-based redirection of the C++ ``stdout`` back to Python.

The C++ side of TRIQS writes to file descriptor ``1`` directly, which
:mod:`sys.stdout` redirection from Python does not capture (in
particular, Jupyter only sees writes that go through Python). This
module installs a background :class:`threading.Thread` that

1. duplicates the original file descriptor ``1`` so it can be
   restored later,
2. creates a pipe and ``dup2`` its write end onto descriptor ``1``,
   so all subsequent C++ output goes into the pipe,
3. periodically reads from the pipe (non-blocking) and writes the
   bytes back to Python's :data:`sys.stdout`.

The public API is :func:`start_redirect` and :func:`stop_redirect`;
they are idempotent and toggle the module-level :data:`started` flag.

Attributes
----------
started : bool
    ``True`` while a redirection thread is running. Set by
    :func:`start_redirect` and cleared by :func:`stop_redirect`.
"""

import threading, time, sys, os, fcntl

__all__ = ['start_redirect', 'stop_redirect']

# A thread that will redirect the c++ stdout to the ipython notebook
class T(threading.Thread):
  r"""
  Background thread that pumps the C++ ``stdout`` into Python's.

  On :meth:`run` the thread creates a pipe, redirects file descriptor
  ``1`` to its write end, and loops on a non-blocking read from the
  read end, forwarding any bytes to :data:`sys.stdout`. The previous
  file descriptor ``1`` (captured in :attr:`oldhandle` at
  construction) is restored when :meth:`stop` is invoked and the loop
  exits.

  Attributes
  ----------
  go_on : bool
      Loop condition for :meth:`run`; set to ``False`` by :meth:`stop`
      to request thread termination.
  oldhandle : int
      Saved duplicate of the original file descriptor ``1``, restored
      via :func:`os.dup2` when the thread stops.
  """

  def __init__(self):
    """Save a duplicate of the current ``stdout`` fd and initialise the thread."""
    threading.Thread.__init__(self)
    self.go_on = True

    # copy the c++ stdout handler for later
    self.oldhandle = os.dup(1)

  def stop(self):
    """Request termination; the run loop exits at the next iteration."""
    self.go_on = False

  def run(self):
    """
    Run the pipe-pumping loop until :meth:`stop` is called.

    Each iteration sleeps for one second to let the OS buffer up
    output, then attempts a non-blocking ``os.read`` of up to ``10000``
    bytes from the pipe and writes them to :data:`sys.stdout`. On exit
    the original file descriptor ``1`` is restored from
    :attr:`oldhandle` and the pipe is closed.
    """

    # create a pipe and glue the c++ stdout to its write end
    # make the read end non-blocking
    piper, pipew = os.pipe()
    os.dup2(pipew, 1)
    os.close(pipew)
    fcntl.fcntl(piper,fcntl.F_SETFL,os.O_NONBLOCK)

    while self.go_on:

      # give the system 1 second to fill up the pipe
      time.sleep(1)
      try:
        # read out the pipe and write it to the screen
        # if the pipe was empty it return an error (hence the try-except)
        sys.stdout.write(os.read(piper, 10000))
        sys.stdout.flush()
      except:
        pass

    # when we want to stop the thread put back the c++ stdout where it was
    # clear the pipe
    os.dup2(self.oldhandle, 1)
    os.close(piper)
    return

# flag to know if the thread was started
started = False

# start the redirection
def start_redirect():
  r"""
  Start a background thread that redirects the C++ ``stdout`` to Python.

  Instantiates a :class:`T` thread and starts it. If a redirection
  thread is already running, prints a notice and returns without
  doing anything (the call is idempotent).
  """
  global started
  global a
  if started:
    print("Already redirected c++ output")
  else:
    started = True
    # start a new redirection thread
    a = T()
    a.start()

# stop the redirection
def stop_redirect():
  r"""
  Stop the background redirection thread started by :func:`start_redirect`.

  Signals the running :class:`T` thread to exit (which restores the
  original file descriptor ``1``). No-op when no thread is currently
  running.
  """
  global started
  global a
  if started:
    started = False
    a.stop()
