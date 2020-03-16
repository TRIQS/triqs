from past.utils import old_div
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
    bin_width = optional_dict.pop('width', old_div((self.limits[1] - self.limits[0]),(len(self) - 1)))
    bin_edges = bin_centres - ((old_div(bin_width,2)) if plot_type=='bar' else 0)

    default_dict = {'xdata': bin_edges, 
                    'ydata': self.data, 
                    'label': 'Histogram', 
                    'plot_function': 'plot' if plot_type=="XY" else 'bar'
                   }
    if plot_type=='bar' and not 'width' in optional_dict:
        default_dict['width'] = bin_width
    default_dict.update(optional_dict)

    return [default_dict]
