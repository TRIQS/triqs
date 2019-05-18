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
# Authors: Michel Ferrero, Priyanka Seth, Nils Wentzell, tayral

import numpy as np

# Only the plot function, everything else is wrrapped from c++
def plot(self, optional_dict):
    r"""
    Plot protocol for histograms.

    Parameters
    ----------
    type : string
           Mode to plot the histogram:
           - 'XY': plot using lines, default
           - 'bar': plot using bars
    width : float, default bin width
            The width of the bars in a bar plot.
    Returns
    -------
    plot_data: list of dict
               Object passed to oplot to plot the histogram.
    """

    plot_type = optional_dict.pop('type','XY')
    bin_centres = np.linspace(self.limits[0],self.limits[1],len(self))
    bin_width = optional_dict.pop('width', (self.limits[1] - self.limits[0])/(len(self) - 1))
    bin_edges = bin_centres - ((bin_width/2) if plot_type=='bar' else 0)

    default_dict = {'xdata': bin_edges, 
                    'ydata': self.data, 
                    'label': 'Histogram', 
                    'plot_function': 'plot' if plot_type=="XY" else 'bar'
                   }
    if plot_type=='bar' and not 'width' in optional_dict:
        default_dict['width'] = bin_width
    default_dict.update(optional_dict)

    return [default_dict]
