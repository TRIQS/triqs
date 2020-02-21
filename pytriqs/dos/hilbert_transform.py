from __future__ import division
from __future__ import unicode_literals

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

from builtins import zip
from builtins import range
from builtins import object
from past.utils import old_div
from pytriqs.gf import *
import types, string, inspect, itertools
from operator import isSequenceType
from pytriqs.dos import DOS, DOSFromFunction
import pytriqs.utility.mpi as mpi
import numpy

class HilbertTransform(object):
    r"""
    Computes the Hilbert Transform from a DOS object

    .. math::

       \int_{-\infty}^\infty d \epsilon \rho(\epsilon) \Bigl(  (\omega + \mu +
       I\eta)\mathbf{1} - \hat\varepsilon(\epsilon) - \text{field} - \Sigma(\epsilon)
       \Bigr)^{-1}

    """
    def __init__(self, rho):
        """
        :param rho: a DOS object.
        """
        self.dos  = rho
        assert isinstance(rho, DOS),  "See Doc. rho must be a DOS"
        self.__normalize()

    #-------------------------------------------------------------

    def __reduce__(self):
        return self.__class__, (self.rho)

    #-------------------------------------------------------------

    def __normalize(self):
        # normalisation. dos is not the value of the function, is the weight of the integrals
        R = numpy.array(self.dos.rho, copy=True)
        self.rho_for_sum = R
        eps = self.dos.eps
        R[0]  *= (eps[1] - eps[0])
        R[-1] *= (eps[-1] - eps[-2])
        for i in range(1, eps.shape[0] - 1):
            R[i] *=  old_div((eps[i+1] - eps[i]),2)+old_div((eps[i] - eps[i-1]),2)
        R /= numpy.sum(R)

    #-------------------------------------------------------------

    def __call__ (self, Sigma, mu=0, eta=0, field=None, epsilon_hat=None, result=None,
                  n_points_integral=None, test_convergence=None):
        r"""
        Compute the Hilbert Transform

        Parameters
        -----------

        mu: float
        eta: float
        Sigma: a GFBloc or a function epsilon-> GFBloc
        field: anything that can added to the GFBloc Sigma, e.g.:
                 * an Array_with_GFBloc_Indices (same size as Sigma)
                 * a GBloc
        epsilon_hat: a function that takes a 1d array eps[i] and returns 3d-array   eps[i,:,:]
                            where the:,: has the matrix structure of Sigma. Default: eps[i] * Identity_Matrix
                            Used only when DOS is a DOSFromFunction:
        n_points_integral: How many points to use. If None, use the Npts of construction
        test_convergence: If defined, it will refine the grid until CV is reached
                          starting from n_points_integral and multiplying by 2

        Returns
        --------

        Returns the result. If provided, use result to compute the result locally.
        """

        # we suppose here that self.eps, self.rho_for_sum such that
        # H(z) = \sum_i self.rho_for_sum[i] * (z- self.eps[i])^-1

        # Check Sigma
        # case 1) Sigma is a Gf
        if Sigma.__class__.__name__[0:2] == 'Gf':
            model = Sigma
            Sigma_fnt = False
        # case 2) Sigma is a function returning a Gf
        else:
            assert callable(Sigma), "If Sigma is not a Gf it must be a function"
            assert len(inspect.getargspec(Sigma)[0]) == 1, "Sigma must be a function of a single variable"
            model = Sigma(self.dos.eps[0])
            Sigma_fnt = True

        # Check that Sigma is square
        N1 = model.N1
        N2 = model.N2
        assert N1 == N2, "Sigma must be a square Gf"

        # Check result
        if result:
            assert result.target_shape == (N1, N2), "Size of result and Sigma mismatch"
        else:
            result = model.copy()

        if not(isinstance(self.dos, DOSFromFunction)):
            assert n_points_integral==None and test_convergence == None, " Those parameters can only be used with an dos_from_function"
        if field !=None:
            try:
                result += field
            except:
                assert 0, "field cannot be added to the Green function blocks !. Cf Doc"

        def HT(res):
            # First compute the eps_hat array
            eps_hat = epsilon_hat(self.dos.eps) if epsilon_hat else numpy.array( [ x* numpy.identity (N1) for x in self.dos.eps] )
            assert eps_hat.shape[0] == self.dos.eps.shape[0], "epsilon_hat function behaves incorrectly"
            assert eps_hat.shape[1] == eps_hat.shape[2], "epsilon_hat function behaves incorrectly (result not a square matrix)"
            assert N1 == eps_hat.shape[1], "Size of Sigma and of epsilon_hat mismatch"

            res.zero()

            # Perform the sum over eps[i]
            tmp, tmp2 = res.copy(), res.copy()
            tmp << iOmega_n + mu + eta * 1j
            if not(Sigma_fnt):
                tmp -= Sigma
            if field != None: tmp -= field

            # I slice all the arrays on the node. Cf reduce operation below.
            for d, e_h, e in  zip (*[mpi.slice_array(A) for A in [self.rho_for_sum, eps_hat, self.dos.eps]]):
                tmp2.copy_from(tmp)
                tmp2 -= e_h
                if Sigma_fnt: tmp2 -= Sigma(e)
                tmp2.invert()
                tmp2 *= d
                res += tmp2
            # sum the res GF of all nodes and returns the results on all nodes...
            # Cf Boost.mpi.python, collective communicator for documentation.
            # The point is that res is pickable, hence can be transmitted between nodes without further code...
            res << mpi.all_reduce(mpi.world, res, lambda x, y: x+y)
            mpi.barrier()
        # END of HT

        def test_distance(G1, G2, dist):
            def f(G1, G2):
                dS = max(abs(G1.data - G2.data).flatten())
                aS = max(abs(G1.data).flatten())
                return dS <= aS*dist
            #return reduce(lambda x, y: x and y, [f(g1, g2) for (i1, g1), (i2, g2) in izip(G1, G2)])
            return f(G1, G2) # for block function, the previous one is for GF functions

        if isinstance (self.dos, DOSFromFunction):

            if not(n_points_integral): # if not defined, use the defaults given at construction of the dos
                n_points_integral=  len(self.dos.eps)
            else:
                self.dos._DOS__f(n_points_integral)
                self.__normalize()

            HT(result)

            nloop, test = 1, 0
            while test_convergence and nloop < 10 and (nloop == 1 or test > test_convergence):
                if nloop>1:
                    self.dos._DOS__f(n_points_integral)
                    self.__normalize()

                result_old = result.copy()
                result = DOS.HilbertTransform(self, Sigma, mu, eta, epsilon_hat, result)
                test = test_distance(result, result_old, test_convergence)
                n_points_integral *=2

        else:  # Ordinary DOS
            HT(result)

        return result
