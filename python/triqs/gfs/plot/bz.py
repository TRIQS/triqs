# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Thomas Ayral, Michel Ferrero, Olivier Parcollet, Nils Wentzell

r"""Plot helpers for Green's functions on a 2D Brillouin-zone or 
cyclic-lattice mesh.

Provides

* :func:`~triqs.gfs.plot.bz.make_plottable` — interpolate the data 
  on a regular ``nk x nk`` grid suitable for ``contourf`` / heatmaps.
* :func:`~triqs.gfs.plot.bz.slice_on_path` — sample the Green's 
  function along a piecewise-linear path through k-space 
  (band-structure-style plots).
* :func:`~triqs.gfs.plot.bz.plot` — TRIQS plot-protocol dispatcher 
  consumed by :func:`~triqs.plot.mpl_interface.oplot`.
"""

from scipy.interpolate import griddata
import numpy as np

def make_plottable(self, method="cubic", nk=50):
   '''Interpolate ``self`` on a regular ``nk x nk`` k-grid for plotting.

   Parameters
   ----------
   method : {'cubic', 'linear', 'nearest'}, optional
       Interpolation method passed to :func:`scipy.interpolate.griddata`.
       Default ``'cubic'``.
   nk : int, optional
       Resolution of the regular grid. Default ``50``.

   Returns
   -------
   x, y : numpy.ndarray
       1D arrays defining the regular grid.
   z : numpy.ndarray
       Interpolated values, shape ``(n_orb1, n_orb2, nk, nk)``.
   zmin, zmax : numpy.ndarray
       Per-orbital element min/max (complex), shape
       ``(n_orb1, n_orb2)``.
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
def dist(A, B):
    """Euclidean distance between two 2D points.

    Parameters
    ----------
    A, B : sequence of float
        Points ``(x, y)`` in the plane.

    Returns
    -------
    float
        :math:`\\sqrt{(A_x - B_x)^2 + (A_y - B_y)^2}`.
    """
    return np.sqrt((A[0]-B[0])**2+(A[1]-B[1])**2)

def length(path):
    """Total Euclidean length of a piecewise-linear path.

    Parameters
    ----------
    path : sequence of (float, float)
        Vertices of the path in order.

    Returns
    -------
    float
        Sum of segment lengths between consecutive vertices.
    """
    return sum([dist(path[i],path[i+1]) for i in range(len(path)-1)])

def generate_points(A, B, n_points):
    """Sample ``n_points`` evenly-spaced points on the segment ``[A, B]``.

    Parameters
    ----------
    A, B : sequence of float
        Endpoints ``(x, y)`` of the segment. ``A`` and ``B`` are
        included as the first and last returned points.
    n_points : int
        Number of points along the segment (must be ``>= 2``).

    Returns
    -------
    list of (float, float)
        The sampled points, in order from ``A`` to ``B``.
    """
    pts=[]
    for i in range(n_points):
        x=A[0]+(B[0]-A[0])/(n_points-1)*i
        y=A[1]+(B[1]-A[1])/(n_points-1)*i
        pts.append((x, y))
    return pts

def generate_points_on_path(path, n_points):
    """Distribute ``n_points`` samples along a piecewise-linear path.

    Points are distributed segment-by-segment proportionally to each
    segment's length, so the sampling density is approximately uniform
    along the whole path.

    Parameters
    ----------
    path : sequence of (float, float)
        Vertices of the path in order.
    n_points : int
        Total number of points to distribute across all segments.

    Returns
    -------
    l_points : list of (float, float)
        The sampled points along the path.
    high_sym : list of int
        Indices in ``l_points`` corresponding to the input vertices
        (the first entry is always ``0``; useful for placing
        high-symmetry-point ticks on band-structure plots).
    """
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
 '''Sample the (0, 0) component of ``self`` along a path in the BZ.

 Parameters
 ----------
 path : sequence of (float, float)
     Vertices of the piecewise-linear path in the BZ.
 n_pts : int, optional
     Total number of sampling points distributed across the path.
     Default ``100``.
 method : {'cubic', 'linear', 'nearest'}, optional
     Interpolation method used to build the dense grid. Default
     ``'cubic'``.

 Returns
 -------
 L : list of (float, float)
     Momenta visited along the path.
 Lz_on_path : numpy.ndarray
     Interpolated values at those momenta.
 high_sym : list of int
     Indices in ``L`` corresponding to the input path vertices.
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
    r"""Plot-protocol implementation for Green's functions on a 2D BZ / 
    cyclic-lattice mesh.

    Parameters
    ----------
    opt_dict : dict
        Plot options, e.g.

        * ``type`` — ``'XY'`` (default; band-structure style along
          ``path``) or ``'contourf'`` (2D heat map).
        * ``method`` — interpolation method for
          :func:`~triqs.gfs.plot.bz.make_plottable`. Default ``'nearest'``.
        * ``mode`` — ``'R'`` for the real part (default) or ``'I'``
          for the imaginary part.
        * ``path`` — required for ``type='XY'``, a list of BZ
          coordinates defining the cut.

    Returns
    -------
    list of dict
        Curve / contour descriptors consumed by
        :func:`~triqs.plot.mpl_interface.oplot`.
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

     default_dict = {'xdata': list(range(0,len(L))), 
                     'ydata': component(Lpt), 
                     'label': r'$G_\mathbf{%s}$'%X_label, 
                     'xlabel': r'$\mathbf{%s}$'%X_label, 
                     'plot_function': 'plot',
                     'xticks' : xticks_args,
                     }
    else: raise Exception("Unknown plot type %s. Should be 'XY' (default) or 'contourf'"%mode)

    default_dict.update(opt_dict)

    return [default_dict]


