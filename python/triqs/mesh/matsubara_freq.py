# Copyright (c) 2022-2023 Simons Foundation
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
# Authors: Michel Ferrero, Dominik Kiese, Nils Wentzell

"""Stand-alone Matsubara frequency objects for use outside a mesh context."""

import math

class MatsubaraFreq:
    r"""A single Matsubara frequency :math:`i\omega_n`.

    Castable to :class:`complex` via ``complex(freq)``; arithmetic with
    scalars is implemented by casting to complex, while arithmetic between
    two :class:`MatsubaraFreq` instances returns a new
    :class:`MatsubaraFreq` whose statistic follows from the usual selection
    rules (fermion + fermion = boson, fermion + boson = fermion, etc.).

    The associated complex value is

    .. math::

       i\omega_n = i\, \frac{(2n + \mathrm{is\_fermion})\,\pi}{\beta},

    so fermionic frequencies are odd multiples of :math:`\pi/\beta` and
    bosonic frequencies are even multiples.
    
    Parameters
    ----------
    n : int
        The Matsubara index :math:`n`.
    beta : float
        Inverse temperature :math:`\beta > 0`.
    statistic : str, optional
        Particle statistic, either ``'Fermion'`` or ``'Boson'``. Default
        ``'Fermion'``.

    Attributes
    ----------
    n : int
        The Matsubara index :math:`n`.
    beta : float
        Inverse temperature :math:`\beta`.
    statistic : str
        Particle statistic, ``'Fermion'`` or ``'Boson'``.
    is_fermion : bool
        ``True`` for a fermionic frequency, ``False`` for a bosonic one.
    index : int
        Alias of :attr:`n`.
    value : complex
        Complex value :math:`i\omega_n` of this Matsubara frequency.
    real : float
        Real part of :math:`i\omega_n` (always ``0.0``).
    imag : float
        Imaginary part of :math:`i\omega_n`.
    """

    def __init__(self, n, beta, statistic="Fermion"):
        self.n = n
        self.beta = beta
        self.statistic = statistic
        self.is_fermion = (True if self.statistic == "Fermion" else False)

    def __complex__(self):
        r"""Cast to :class:`complex`.

        Returns
        -------
        complex
            :math:`i\omega_n = i\,(2n + \mathrm{is\_fermion})\,\pi/\beta`.
        """
        return 1j * (2*self.n + self.is_fermion) * math.pi / self.beta

    @property
    def index(self):
        r"""Matsubara index :math:`n` (alias of :attr:`n`).

        Returns
        -------
        int
            The integer Matsubara index :math:`n`.
        """
        return self.n

    @property
    def imag(self):
        r"""Imaginary part of :math:`i\omega_n`.

        Returns
        -------
        float
            :math:`(2n + \mathrm{is\_fermion})\,\pi / \beta`.
        """
        return complex(self).imag

    @property
    def real(self):
        r"""Real part of :math:`i\omega_n`, identically zero.

        Returns
        -------
        float
            Always ``0.0``.
        """
        return 0.0

    @property
    def value(self):
        r"""Complex value :math:`i\omega_n` of this Matsubara frequency.

        Returns
        -------
        complex
            :math:`i\,(2n + \mathrm{is\_fermion})\,\pi / \beta`.
        """
        return complex(self)

    def __eq__(self, other):
        """Equality test on :attr:`n`, :attr:`beta` and :attr:`statistic`.

        Parameters
        ----------
        other : object
            Right-hand side of the comparison. Anything other than a
            :class:`MatsubaraFreq` compares unequal.

        Returns
        -------
        bool
            ``True`` if ``other`` is a :class:`MatsubaraFreq` with the
            same ``n``, ``beta`` and ``statistic``.
        """
        if isinstance(other, MatsubaraFreq):
            return self.n == other.n and self.beta == other.beta and self.statistic == other.statistic
        else:
            return False

    def __abs__(self):
        r"""Modulus :math:`|i\omega_n|`.

        Returns
        -------
        float
            ``abs(complex(self))``.
        """
        return abs(complex(self))

    def __add__(self, other):
        r"""Add another Matsubara frequency or a scalar.

        Two :class:`MatsubaraFreq` instances are added as integer indices,
        with the resulting statistic following the selection rule
        ``self.is_fermion XOR other.is_fermion``. A scalar is added after
        casting ``self`` to :class:`complex`.

        Parameters
        ----------
        other : MatsubaraFreq or complex-like
            Right-hand side of the addition.

        Returns
        -------
        MatsubaraFreq or complex
            New :class:`MatsubaraFreq` when ``other`` is a Matsubara
            frequency, ``complex(self) + other`` otherwise.

        Raises
        ------
        RuntimeError
            If ``other`` is a :class:`MatsubaraFreq` with a different
            :attr:`beta`.
        """
        if isinstance(other, MatsubaraFreq):
            if not self.beta == other.beta:
                raise RuntimeError(f"Values of beta do not match: self.beta={self.beta} other.beta={other.beta}")
            is_fermion = self.is_fermion ^ other.is_fermion
            statistic = "Fermion" if is_fermion else "Boson"
            return MatsubaraFreq(self.n + other.n + (self.is_fermion and other.is_fermion), self.beta, statistic)
        else:
            return complex(self) + other

    def __neg__(self):
        r"""Negate the Matsubara frequency.

        Returns
        -------
        MatsubaraFreq
            New :class:`MatsubaraFreq` representing :math:`-i\omega_n`
            with the same :attr:`statistic`.
        """
        return MatsubaraFreq(-self.n - self.is_fermion, self.beta, self.statistic)

    def __radd__(self, other):
        """Right-hand addition; defers to :meth:`__add__`.

        Parameters
        ----------
        other : MatsubaraFreq or complex-like
            Left-hand side of the original ``other + self`` expression.

        Returns
        -------
        MatsubaraFreq or complex
            See :meth:`__add__`.
        """
        return self.__add__(other)

    def __sub__(self, other):
        """Subtraction, implemented as ``self + (-other)``.

        Parameters
        ----------
        other : MatsubaraFreq or complex-like
            Right-hand side of the subtraction.

        Returns
        -------
        MatsubaraFreq or complex
            See :meth:`__add__`.
        """
        return self.__add__(other.__neg__())

    def __rsub__(self, other):
        """Right-hand subtraction, implemented as ``(-self) + other``.

        Parameters
        ----------
        other : MatsubaraFreq or complex-like
            Left-hand side of the original ``other - self`` expression.

        Returns
        -------
        MatsubaraFreq or complex
            See :meth:`__add__`.
        """
        neg = self.__neg__()
        return neg.__add__(other)

    def __mul__(self, other):
        r"""Multiplication by a scalar, computed after casting to :class:`complex`.

        Parameters
        ----------
        other : complex-like
            Right-hand side of the product.

        Returns
        -------
        complex
            :math:`i\omega_n \cdot \mathrm{other}`.
        """
        return complex(self) * other

    def __rmul__(self, other):
        r"""Right-hand multiplication by a scalar.

        Parameters
        ----------
        other : complex-like
            Left-hand side of the original ``other * self`` expression.

        Returns
        -------
        complex
            :math:`\mathrm{other} \cdot i\omega_n`.
        """
        return other * complex(self)

    def __truediv__(self, other):
        r"""Division by a scalar, computed after casting to :class:`complex`.

        Parameters
        ----------
        other : complex-like
            Divisor.

        Returns
        -------
        complex
            :math:`i\omega_n / \mathrm{other}`.
        """
        return complex(self) / other

    def __rtruediv__(self, other):
        r"""Right-hand division.

        Parameters
        ----------
        other : complex-like
            Dividend.

        Returns
        -------
        complex
            :math:`\mathrm{other} / i\omega_n`.
        """
        return other / complex(self)

    def __pow__(self, other):
        r"""Exponentiation, computed after casting to :class:`complex`.

        Parameters
        ----------
        other : complex-like
            Exponent.

        Returns
        -------
        complex
            :math:`(i\omega_n)^{\mathrm{other}}`.
        """
        return complex(self)**other

    def __str__(self):
        """Human-readable string ``MatsubaraFreq(n: ..., beta: ..., statistic: ...)``.

        Returns
        -------
        str
            String describing the Matsubara frequency.
        """
        return f"MatsubaraFreq(n: {self.n}, beta: {self.beta}, statistic: {self.statistic})"

    def __repr__(self):
        """REPL representation; identical to :meth:`__str__`.

        Returns
        -------
        str
            Same string as :meth:`__str__`.
        """
        return self.__str__()
