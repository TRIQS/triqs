from __future__ import print_function

################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

import os,sys,time,cPickle
import pytriqs.utility.mpi as mpi

class DistributionOnNodes:
    """
       Distribution of the calculation of a function over the nodes.
       Derive from it and reimplement :
         - treate : will be called by the MASTER each time a point is computed. this function will typically
                    store it, and possibly affect the list of points waiting to be computed, as returned by next...
                    NB : "None" result is ignored.
         - next : next point to compute. If it returns None (which is not the same as finished())
                 the computation is ignored.
         - finished() : Whether the calculation is finished
         - the_function : function to be computed, with the argument given by next()
       In the module  DistributionOnNodeTest is a test example
    """
    SleepTime = 1

    #def treate(self,x,node_where_computed): pass
    #def next(self): return None
    #def finished(self): return True #None
    #def the_function(self,x): return x
 
    def run(self):
        """
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
              RequestList = filter(keep_request,RequestList)
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
                      RR = cPickle.load(open("res_master",'r'))
                      if RR != None : self.treate(*RR)
                  if not(self.finished()) :
                      pid=os.fork();
                      currently_calculated_by_master = next(self)
                      if pid==0 :  # we are on the child
                          if currently_calculated_by_master :
                              res = self.the_function(currently_calculated_by_master)
                          else:
                              res = None
                          cPickle.dump((res,mpi.rank),open('res_master','w'))
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
    A special of distribution, when one has a given stack of points to compute
    Reimplement just the_function
    """
    def __init__(self,the_stack) :
        self.__l = the_stack
        self.__l.reverse()
        self.__result = []
        
    def treate(self,x,node):
        self.__result.append((node,x))

    def finished(self):
        return self.__l==[]
    
    def next(self):
        return self.__l.pop()

    def result(self) : return self.__result
        
    def run(self):
        DistributionOnNodes.run(self)
        return self.result()


# TEST ONLY
class DistributionOnNodesTest(DistributionOnNodesOneStack) :
    """
    """
    def the_function(self,x):
        return (x,x+1,x+2)

if __name__ == '__main__' : 
    d = DistributionOnNodesTest(range(21))
    d.run()
    if mpi.rank==0 :
        print(d.result())
