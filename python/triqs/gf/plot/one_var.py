# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

from . import plot_base

#---------------------------------------------------------------
# A list of plot functions for 
#---------------------------------------------------------------

def imfreq(self, opt_dict):
    r"""
    Plot protocol for GfImFreq objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
              - x_window: tuple, default None 
                          (xmin,xmax)
              - name: str
                     name of the gf for the label and legend
    """
    return plot_base.plot_base( self, opt_dict,  r'$\omega_n$',
            lambda x : r'%s$(i\omega_n)$'%x, [x.imag for x in list(self.mesh.values())] )

#----------------------------------------------------------------

def imtime(self, opt_dict):
    r"""
    Plot protocol for GfImTime objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
              - x_window: tuple, default None 
                          (xmin,xmax)
              - name: str
                     name of the gf for the label and legend
    """
    return plot_base.plot_base( self, opt_dict,  r'$\tau$', lambda x : r'%s$(\tau)$'%x, list(self.mesh.values()))

#----------------------------------------------------------------

def legendre(self, opt_dict):
    r"""
    Plot protocol for GfLegendre objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
              - x_window: tuple, default None 
                          (xmin,xmax)
              - name: str
                     name of the gf for the label and legend
    """
    return plot_base.plot_base( self, opt_dict,  r'$l_n$', lambda x : r'%s$(l_n)$'%x, list(self.mesh.values()))

#----------------------------------------------------------------

def refreq(self, opt_dict):
    r"""
    Plot protocol for GfReFreq objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
                      -- 'S': spectral function
              - x_window: tuple, default None 
                          (xmin,xmax)
              - name: str
                     name of the gf for the label and legend
    """
    return plot_base.plot_base(self, opt_dict,  r'$\omega$', lambda x : r'%s$(\omega)$'%x, list(self.mesh.values()), allow_spectral_mode = True)

#----------------------------------------------------------------

def retime (self, opt_dict):
    r"""
    Plot protocol for GfReTime objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
              - x_window: tuple, default None 
                          (xmin,xmax)
              - name: str
                     name of the gf for the label and legend
    """
    return plot_base.plot_base(self, opt_dict,  r'$\t$', lambda x : r'%s$(\t)$'%x, list(self.mesh.values()))

