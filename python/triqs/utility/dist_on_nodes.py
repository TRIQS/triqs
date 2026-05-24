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


"""
Master / worker pattern for distributing a function evaluation over MPI.

Defines :class:`DistributionOnNodes`, an abstract base class implementing
the master-side dispatch loop, plus :class:`DistributionOnNodesOneStack`,
a ready-to-use subclass that consumes a fixed stack of input points.
:class:`DistributionOnNodesTest` is a minimal example used by the
self-test at the bottom of the module.
"""

import os,sys,time,pickle
import triqs.utility.mpi as mpi

class DistributionOnNodes:
    r"""
    Abstract master / worker dispatcher distributing function calls over MPI.

    Subclasses are expected to provide:

    - ``__next__(self)`` -- return the next input to evaluate, or
      ``None`` to skip a step. A return of ``None`` is not the same as
      :meth:`finished` returning ``True``.
    - ``finished(self)`` -- return ``True`` when no more inputs remain
      to be produced.
    - ``the_function(self, x)`` -- the actual work performed on a
      worker rank for input ``x``.
    - ``treate(self, value, node)`` -- called on the master rank for
      each result returned by a worker. A ``None`` result is silently
      ignored.

    See :class:`DistributionOnNodesOneStack` for a minimal concrete
    implementation and :class:`DistributionOnNodesTest` for a runnable
    example.

    Attributes
    ----------
    SleepTime : float
        Class attribute. Seconds the master sleeps between polls of the
        local child process when an evaluation is also running on the
        master. Default 1.
    """
    SleepTime = 1

    #def treate(self,x,node_where_computed): pass
    #def next(self): return None
    #def finished(self): return True #None
    #def the_function(self,x): return x

    def run(self):
        """
        Run the master / worker dispatch loop until :meth:`finished`.

        On a single MPI rank the function loops sequentially over
        :meth:`__next__`, calling :meth:`the_function` and then
        :meth:`treate` for every input. On multiple ranks the master
        rank dispatches inputs to the workers (and to a forked child
        on itself), asynchronously collects results, and calls
        :meth:`treate` for each one; the workers loop on receive /
        compute / send until the master signals completion. The call
        is collective: every rank must enter and leave it together.
        """
        mpi.barrier()
        if mpi.size==1 : # single machine. Avoid the fork
            while not(self.finished()):
                n = next(self)
                if n!=None : 
                    self.treate(self.the_function(n),0)
            return

        # Code for multiprocessor machines
        RequestList,pid = [],0   # the pid of the child on the master
        node_running,node_stopped= mpi.size*[False],mpi.size*[False]

        if mpi.rank==0 :
          while not(self.finished()) or pid or [n for n in node_running if n] != [] :
              # Treat the request which have self.finished
              def keep_request(r) :
                  #if not(mpi.test(r)) :  return True
                  #if r.message !=None : self.treate(*r.message)
                  #node_running[r.status.source] = False
                  T = r.test()
                  if T is None :  return True
                  value = T[0]
                  if value !=None : self.treate(*value)
                  node_running[T[1].source] = False
                  return False
              RequestList = list(filter(keep_request,RequestList))
              # send new calculation to the nodes or "stop" them
              for node in [ n for n in range(1,mpi.size) if not(node_running[n] or node_stopped[n]) ] :
                  #open('tmp','a').write("master : comm to node %d %s\n"%(node,self.finished()))
                  mpi.send(self.finished(),node)
                  if not(self.finished()) :
                      mpi.send(next(self),node) # send the data for the computation
                      node_running[node] = True
                      RequestList.append(mpi.irecv(node)) #Post the receive
                  else :
                      node_stopped[node] = True

              # Look if the child process on the master has self.finished.
              if not(pid) or os.waitpid(pid,os.WNOHANG) :
                  if pid :
                      RR = pickle.load(open("res_master",'r'))
                      if RR != None : self.treate(*RR)
                  if not(self.finished()) :
                      pid=os.fork();
                      currently_calculated_by_master = next(self)
                      if pid==0 :  # we are on the child
                          if currently_calculated_by_master :
                              res = self.the_function(currently_calculated_by_master)
                          else:
                              res = None
                          pickle.dump((res,mpi.rank),open('res_master','w'))
                          os._exit(0) # Cf python doc. Used for child only.
                  else : pid=0
              if (pid): time.sleep(self.SleepTime) # so that most of the time is for the actual calculation on the master

        else : # not master
            while not(mpi.recv(0)) :  # master will first send a finished flag
                omega = mpi.recv(0)
                if omega ==None :
                    res = None
                else :
                    res = self.the_function(omega)
                mpi.send((res,mpi.rank),0)
                
        mpi.barrier()

#########################################
    
class DistributionOnNodesOneStack(DistributionOnNodes) :
    """
    Concrete :class:`DistributionOnNodes` consuming a fixed stack of inputs.

    Stores the inputs internally as a (reversed) list so that
    :meth:`__next__` returns them in the original order. Each result
    returned by a worker is collected as a ``(node, value)`` tuple and
    can be retrieved via :meth:`result`. Subclasses only need to
    override :meth:`the_function`.

    Parameters
    ----------
    the_stack : list
        Sequence of inputs to dispatch. Consumed destructively: the
        list is reversed in place and popped from the end.
    """
    def __init__(self,the_stack) :
        """Store ``the_stack`` (reversed) as the work list."""
        self.__l = the_stack
        self.__l.reverse()
        self.__result = []

    def treate(self,x,node):
        """Append the worker result as ``(node, x)`` to the result list."""
        self.__result.append((node,x))

    def finished(self):
        """Return ``True`` once every input has been consumed."""
        return self.__l==[]

    def __next__(self):
        """Pop and return the next input from the stack."""
        return self.__l.pop()

    def result(self) :
        """Return the collected list of ``(node, value)`` result tuples."""
        return self.__result

    def run(self):
        """Run the dispatch loop and return :meth:`result`."""
        DistributionOnNodes.run(self)
        return self.result()


# TEST ONLY
class DistributionOnNodesTest(DistributionOnNodesOneStack) :
    """
    Minimal :class:`DistributionOnNodesOneStack` used by the self-test.

    Overrides :meth:`the_function` with ``x -> (x, x + 1, x + 2)`` so
    the dispatch logic can be exercised on integer inputs without
    pulling in any TRIQS-specific objects.
    """
    def the_function(self,x):
        """Return ``(x, x + 1, x + 2)`` as the per-input computation."""
        return (x,x+1,x+2)

if __name__ == '__main__' : 
    d = DistributionOnNodesTest(list(range(21)))
    d.run()
    if mpi.rank==0 :
        print(d.result())
