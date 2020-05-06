from numpy import array, zeros
import math 
from triqs.lattice.tight_binding import *

# Define the Bravais Lattice : a square lattice in 2d
BL_1 = BravaisLattice(units = [(1,0,0), (0,1,0) ], orbital_positions= [(0,0,0)] )
BL_2 = BravaisLattice(units = [(1,1,0) , (-1,1,0) ], orbital_positions= [ (0,0,0),(.5,.5,0)] )
BL_4 = BravaisLattice(units = [(2,0,0) , (0,2,0) ], orbital_positions= [(0,0,0),(0,.5,0), (.5,0,0), (.5,.5,0)] )

# Hopping dictionaries
t = .25; tp = -.1;

hop_1= {  (1,0)  :  [[ t]],      (-1,0) :  [[ t]],     (0,1)  :  [[ t]],   (0,-1) :  [[ t]],
        (1,1)  :  [[ tp]],      (-1,-1):  [[ tp]],        (1,-1) :  [[ tp]],     (-1,1) :  [[ tp]]
        }

hop_2= {  (0,0) :[[0.,t],
                  [t,0.]],
        (1,0)  :  [[ tp, 0],
                   [ t ,tp]],
        (-1,0) :  [[ tp, t],
                   [ 0 ,tp]],
        (0,1)  :[[ tp, 0],
                 [ t, tp]],
        (0,-1) :[[ tp, t],
                 [ 0 ,tp]],
        (1,1)  :  [[ 0, 0],
                   [ t,0]],
        (-1,-1) :[[ 0, t],
                  [ 0,0]],
        (-1,1) :  [[ 0, 0],
                   [ 0,0]],
        (1,-1)  : [[ 0, 0],
                   [ 0,0]]
        }

hop_4= {  (0,0) :[[0.,t, tp,t],
                  [t,0., t,tp],
                  [tp,t,0,t],
                  [t,tp,t,0]],
        
        (1,0)  :  [[0.,0, 0,0],
                   [t,0.,0,tp],
                   [tp,0,0,t],
                   [0,0,0,0]],
        
        (-1,0) :  [[0.,t, tp,0],
                   [0,0.,0,0],
                   [0,0,0,0],
                   [0,tp,t,0]], 
        
        (0,1)  : [[0.,0, 0,0],
                  [0,0.,0,0],
                  [tp,t,0,0],
                  [t,tp,0,0]],   
        
        (0,-1) :[[0.,0, tp,t],
                 [0,0.,t,tp],
                 [0,0,0,0],
                 [0,0,0,0]],
        
         (1,1)  :  [[0.,0, 0,0],
                   [0,0.,0,0],
                   [tp,0,0,0],
                   [0,0,0,0]],
         
        (-1,-1) :  [[0.,0, tp,0],
                   [0,0.,0,0],
                   [0,0,0,0],
                   [0,0,0,0]], 
        
        (-1,1)  : [[0.,0, 0,0],
                  [0,0.,0,0],
                  [0,0,0,0],
                  [0,tp,0,0]],  
        
        (1,-1) :[[0.,0, 0,0],
                 [0,0.,0,tp],
                 [0,0,0,0],
                 [0,0,0,0]],
        
        }

TB_1 = TightBinding(BL_1, hop_1)
TB_2 = TightBinding(BL_2, hop_2)
TB_4 = TightBinding(BL_4, hop_4)

# High-symmetry points
Gamma     = array([0.        ,0.       ]);
PiPi      = array([math.pi   ,math.pi  ])*1/(2*math.pi);
Pi0       = array([math.pi   ,0        ])*1/(2*math.pi);
PihPih    = array([math.pi/2 ,math.pi/2])*1/(2*math.pi)
TwoPi0    = array([2*math.pi ,0        ])*1/(2*math.pi);
TwoPiTwoPi= array([math.pi*2 ,math.pi*2])*1/(2*math.pi)

n_pts=50

# Paths along high-symmetry directions
path_1=[Gamma,Pi0,PiPi,Gamma]
path_2=[Gamma,PiPi,TwoPi0,Gamma] #equivalent to path_1 in coordinates of 2at/ucell basis
path_4=[Gamma,TwoPi0,TwoPiTwoPi,Gamma] #equivalent to path_1 in coordinates of 4at/ucell basis

def energies_on_path(path, TB, n_pts, n_orb=1):
    E=zeros((n_orb,n_pts*(len(path)-1)))
    for i in range(len(path)-1,0,-1):
        energies = energies_on_bz_path (TB, path[i-1], path[i], n_pts)
        for orb in range(n_orb):     E[orb,(i-1)*n_pts:(i)*n_pts]=energies[orb,:]
        print("index of point #"+str(i-1)+" = "+str((i-1)*n_pts))
        
    return E
E_1= energies_on_path(path_1,TB_1,n_pts,1)
E_2= energies_on_path(path_2,TB_2,n_pts,2)
E_4= energies_on_path(path_4,TB_4,n_pts,4)

from matplotlib import pylab as plt
plt.plot(E_1[0], '--k', linewidth=4, label = "1 at/unit cell")
plt.plot(E_2[0],'-.g', linewidth=4, label = "2 ats/unit cell")
plt.plot(E_2[1],'-.g', linewidth=4)
plt.plot(E_4[0],'-r', label = "4 ats/unit cell")
plt.plot(E_4[1],'-r')
plt.plot(E_4[2],'-r')
plt.plot(E_4[3],'-r')
plt.grid()
plt.legend()
plt.axes().set_xticks([0,50,100,150])
plt.axes().set_xticklabels([r'$\Gamma_1$',r'$M_1$',r'$X_1$',r'$\Gamma_1$'])
plt.ylabel(r"$\epsilon$")
