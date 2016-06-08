import numpy as np

# Only the plot function, everything else is wrrapped from c++
def plot(self, optional_dict):
    r"""
    Plot protocol for histograms.

    Parameters
    ----------
    mode : string
           Mode to plot the histogram:
           - 'plot': plot using lines, default
           - 'bar': plot using bars
    width : float, default bin width
            The width of the bars in a bar plot.
    Returns
    -------
    plot_data: list of dict
               Object passed to oplot to plot the histogram.
    """

    mode = optional_dict.pop('mode','plot')
    bin_centres = np.linspace(self.limits[0],self.limits[1],len(self))
    bin_width = optional_dict.pop('width', (self.limits[1] - self.limits[0])/(len(self) - 1))
    bin_edges = bin_centres - ((bin_width/2) if mode=='bar' else 0)

    default_dict = {'xdata': bin_edges, 
                    'ydata': self.data, 
                    'label': 'Histogram', 
                    'plot_function': mode
                   }
    if mode=='bar' and not 'width' in optional_dict:
        default_dict['width'] = bin_width
    default_dict.update(optional_dict)

    return [default_dict]
