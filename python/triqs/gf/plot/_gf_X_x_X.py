# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Thomas Ayral, Michel Ferrero

import numpy as np
#from gf import MeshImFreq, MeshReFreq, MeshImTime, MeshReTime

def plot(self, opt_dict):
    r"""
    Plot protocol for GfBrillouinZone objects.
    """

    plot_type = opt_dict.pop('type','contourf')
    method = opt_dict.pop('method', 'nearest')
    comp = opt_dict.pop('mode', 'R')
    component=  lambda x : x.real if comp=="R" else x.imag
    if 'MeshImFreq' in str(type(self.mesh.components[0])):
     X_label = r"i\omega"
     mp_comp = lambda x : x.imag
    elif 'MeshReFreq' in str(type(self.mesh.components[0])):
     X_label = r"\omega"
     mp_comp = lambda x : x.real
    elif 'MeshReTime' in str(type(self.mesh.components[0])):
     X_label = "t"
     mp_comp = lambda x : x.real
    elif 'MeshImTime' in str(type(self.mesh.components[0])):
     X_label = r"\tau"
     mp_comp = lambda x : x.real
    else:
     X_label = "X"
     mp_comp = lambda x : x.real
    if 'MeshImFreq' in str(type(self.mesh.components[1])):
     Y_label = r"i\omega'"
     mp_comp = lambda x : x.imag
    elif 'MeshReFreq' in str(type(self.mesh.components[1])):
     Y_label = r"\omega'"
     mp_comp = lambda x : x.real
    elif 'MeshReTime' in str(type(self.mesh.components[1])):
     Y_label = "t'"
     mp_comp = lambda x : x.real
    elif 'MeshImTime' in str(type(self.mesh.components[1])):
     Y_label = r"\tau'"
     mp_comp = lambda x : x.real
    else:
     Y_label = "X"
     mp_comp = lambda x : x.real

    if plot_type=="contourf":
     x = np.array([mp_comp(m) for m in self.mesh.components[0]])
     y = np.array([mp_comp(m) for m in self.mesh.components[1]])
     z = np.transpose(self.data[:,:,0,0,0])
     zmin=np.amin(z.real)+np.amin(z.imag)*1j
     zmax=np.amax(z.real)+np.amax(z.imag)*1j

     default_dict = {'xdata': x, 
                     'ydata': y, 
                     'label': r'$G_\mathbf{k}$', 
                     'xlabel': r'$%s$'%X_label, 
                     'ylabel': r'$%s$'%Y_label, 
                     'zdata' : component(z),
                     'levels':np.linspace(component(zmin),component(zmax),50), 
                     'plot_function': plot_type,
                     'title': r'$\mathrm{%s}G(%s,%s)$'%('Re' if comp=='R' else 'Im',X_label, Y_label), 
                    }
    else: raise Exception("Unknown plot type %s. Should be 'contourf' (default).'"%plot_type)

    default_dict.update(opt_dict)

    return [default_dict]
