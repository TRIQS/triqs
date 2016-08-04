from scipy.interpolate import griddata
import numpy as np

def make_plottable(self, method="cubic", nk=50):
   ''' 
   :param method: cubic|nearest|linear
   :param nk: number of k points
   :return: x,y,z, zmin, zmax
   '''
   pl = np.zeros((len(self.mesh), 2))
   ik=0
   for k in self.mesh:
    pl[ik, 0]=k[0]
    pl[ik, 1]=k[1]
    ik+=1
   x = pl[:,0]
   y = pl[:,1]
   xi = np.linspace(min(x), max(x),nk)
   yi = np.linspace(min(y), max(y),nk)
   zmin,zmax=np.zeros((self.data.shape[1], self.data.shape[2]), np.complex64), np.zeros((self.data.shape[1], self.data.shape[2]), np.complex64)
   zi=[]
   for ind_x in range(self.data.shape[1]):
    zi.append([])
    for ind_y in range(self.data.shape[2]):

     z = self.data[:,ind_x,ind_y]
     zmin[ind_x,ind_y]=np.amin(z.real)+np.amin(z.imag)*1j
     zmax[ind_x,ind_y]=np.amax(z.real)+np.amax(z.imag)*1j
     zi[ind_x].append(griddata((x, y), z, (xi[None,:], yi[:,None]), method=method))
   return xi,yi,np.array(zi),zmin,zmax

from scipy import interpolate
import itertools
def dist(A,B):
    return np.sqrt((A[0]-B[0])**2+(A[1]-B[1])**2)
def length(path):
    return sum([dist(path[i],path[i+1]) for i in range(len(path)-1)])
def generate_points(A,B, n_points):
        pts=[]
        for i in range(n_points):
            x=A[0]+(B[0]-A[0])/(n_points-1)*i
            y=A[1]+(B[1]-A[1])/(n_points-1)*i
            pts.append((x, y))
        return pts
def generate_points_on_path(path, n_points):
    n_segs = len(path)-1
    l_path = length(path)
    l_points=[]
    #n_seg = n_points/n_segs
    high_sym=[0]
    for i in range(len(path)-1):
        n_seg = int(n_points*dist(path[i],path[i+1])/l_path)
        pts=generate_points(path[i],path[i+1],n_seg)
        
        l_points=list(itertools.chain(l_points,pts))
        high_sym.append(len(l_points))
    return l_points, high_sym

def slice_on_path(self, path, n_pts=100, method="cubic"):
 ''' 
 :param path: a list of points defining a path in the BZ [array(x,y), ...]
 :param n_pts: the number of points on the path
 :param method: interpolation method (cubic|linear|nearest)
 :return: L,Ltot
  with L: list of momenta on path
  and Ltot: Ltot[0][0] : list of eps(k)(0,0) for each k
 '''
 x,y,z,zmin,zmax = make_plottable(self, method=method)
 #print z
 #where_are_NaNs = np.isnan(z)
 #z[where_are_NaNs] = -10
 z=np.nan_to_num(z)

 sp_real = interpolate.RectBivariateSpline(x, y, z[0,0,:,:].real, kx=2, ky=2, s=0)
 sp_imag = interpolate.RectBivariateSpline(x, y, z[0,0,:,:].imag, kx=2, ky=2, s=0)

 L, high_sym=generate_points_on_path(path,n_pts)
 Lz_on_path = [sp_real(x0,y0)[0][0]+1j*sp_imag(x0,y0)[0][0] for x0,y0 in L]
    
 return L, np.array(Lz_on_path), high_sym



def plot(self, opt_dict):
    r"""
    Plot protocol for GfBrillouinZone objects.
    """

    plot_type = opt_dict.pop('type','XY')
    method = opt_dict.pop('method', 'nearest')
    comp = opt_dict.pop('mode', 'R')
    component=  lambda x : x.real if comp=="R" else x.imag

    if 'BrillouinZone' in str(type(self.mesh)):
     X_label = r"k"
    elif 'CyclicLattice' in str(type(self.mesh)):
     X_label = r"R"
    else:
     X_label = "X"

    if plot_type=="contourf":
     x,y,z,zmin, zmax = make_plottable(self, method=method)

     default_dict = {'xdata': x, 
                     'ydata': y, 
                     'label': r'$G_\mathbf{%s}$'%X_label, 
                     'xlabel': r'$%s_x$'%X_label, 
                     'ylabel': r'$%s_y$'%X_label, 
                     'zdata' : component(z[0,0, :, :]),
                     'levels':np.linspace(component(zmin[0,0]),component(zmax[0,0]),50), 
                     'plot_function': plot_type,
                     'title': r'$\mathrm{%s}G(\mathbf{%s})$'%('Re' if comp=='R' else 'Im', X_label), 
                    }
    elif plot_type=="XY":
     path=opt_dict.pop("path")
     L,Lpt, high_sym = slice_on_path(self, path=path, method=method)
     xticks_args=(high_sym, ["%1.3f,%1.3f"%(x,y) for x,y in path],)

     default_dict = {'xdata': range(0,len(L)), 
                     'ydata': component(Lpt), 
                     'label': r'$G_\mathbf{%s}$'%X_label, 
                     'xlabel': r'$\mathbf{%s}$'%X_label, 
                     'plot_function': 'plot',
                     'xticks' : xticks_args,
                     }
    else: raise Exception("Unknown plot type %s. Should be 'XY' (default) or 'contourf'"%mode)

    default_dict.update(opt_dict)

    return [default_dict]


