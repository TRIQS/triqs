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

import threading, time, sys, os, fcntl

__all__ = ['start_redirect', 'stop_redirect']

# A thread that will redirect the c++ stdout to the ipython notebook
class T(threading.Thread):

  def __init__(self):
    threading.Thread.__init__(self)
    self.go_on = True
    
    # copy the c++ stdout handler for later
    self.oldhandle = os.dup(1)

  def stop(self):
    self.go_on = False

  def run(self):

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
  global started
  global a
  if started:
    started = False
    a.stop()
