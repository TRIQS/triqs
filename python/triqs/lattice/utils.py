# Copyright (c) 2021 Simons Foundation
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
# Authors: Nils Wentzell

import numpy as np

__all__ = ['k_space_path']

def k_space_path(paths, num=101, bz=None):

    """ Generate an array of k-vectors along a path defined by a list of pairs of k-vectors
        

    Parameters
    ----------
    paths : list of pairs of three-vectors of floats
       List of pairs of k-vectors (in reciprocal units) to create a path in-between.
    num : int, default=100
       Number of k-vectors along each segment the paths
    bz : brillouin_zone, optional
       When a Brillouin Zone is passed, calculate distance in absoulte units

    Returns
    -------
    kvecs: numpy.array [shape=(len(paths)*num,3)]
        Two-dimensional numpy array containing the path vectors as rows
    dist: numpy.array  [shape=(kvecs.shape[0])]
        One-dimensional numpy array containing, for each element in kvecs,
        the distance travelled along the path. Useful for plotting.
        The distances for the relevant k-points in paths can be obtained with dist[num-1::num].
    """

    if bz is None:
        cell = np.eye(3)
    else:
        cell = bz.units

    x = np.linspace(0., 1., num=num)
    kvecs = [ki[None, :] + x[:, None] * (kf - ki)[None, :] for ki, kf in paths]

    cur_dist = 0.0
    dist = np.array([], dtype=float)
    for kvec in kvecs:
        kvec_abs = np.dot(kvec, cell)
        dist_new = np.linalg.norm(kvec_abs - kvec_abs[0], axis=1) + cur_dist
        dist     = np.concatenate((dist, dist_new))

        cur_dist = dist[-1]

    return np.vstack(kvecs), dist
