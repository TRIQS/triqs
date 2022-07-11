import math

class MatsubaraFreq:
    r""" TRIQS Matsubara frequency

    Castable to complex, arithmetic operations implemented by cast

    Parameters
    ----------
    n: int
        The Matsubara index
    beta: double
        The inverse temperature
    statistic: str
        The particle statistic, 'Fermion' or 'Boson'

    Attributes
    ----------
    n: int
        The Matsubara index
    beta: double
        The inverse temperature
    statistic: str
        The particle statistic, 'Fermion' or 'Boson'
    """

    def __init__(self, n, beta, statistic="Fermion"):
        self.n = n
        self.beta = beta
        self.statistic = statistic
        self.is_fermion = (True if self.statistic == "Fermion" else False)

    def __complex__(self):
        return 1j * (2*self.n + self.is_fermion) * math.pi / self.beta

    def __abs__(self):
        return abs(self.__complex__())

    def __add__(self, other):
        if isinstance(other, MatsubaraFreq):
            if not self.beta == other.beta:
                raise RuntimeError(f"Values of beta do not match: self.beta={self.beta} other.beta={other.beta}")
            is_fermion = self.is_fermion ^ other.is_fermion
            statistic = "Fermion" if is_fermion else "Boson"
            return MatsubaraFreq(self.n + other.n + (self.is_fermion and other.is_fermion), self.beta, statistic)
        else:
            return complex(self) + other

    def __neg__(self):
        return MatsubaraFreq(-self.n - self.is_fermion, self.beta, self.statistic)

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        return self.__add__(other.__neg__())

    def __rsub__(self, other):
        neg = self.__neg__()
        return neg.__add__(other)

    def __mul__(self, other):
        return complex(self) * other

    def __rmul__(self, other):
        return other * complex(self)

    def __truediv__(self, other):
        return complex(self) / other

    def __rtruediv__(self, other):
        return other / complex(self)

    def __pow__(self, other):
        return complex(self)**other
