# Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
# Copyright (c) 2016 Igor Krivenko
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
# Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

r"""Small expression-tree library used by Green's-function descriptors.

The :class:`~triqs.gfs.lazy_expressions.LazyExpr` / 
:class:`~triqs.gfs.lazy_expressions.LazyExprTerminal` pair lets TRIQS 
defer arithmetic on Green's functions and descriptors until ``<<`` 
evaluates the resulting tree on a concrete target. For example,

    >>> g << iOmega_n + 0.5 - SemiCircular(1.0)

is parsed by the operator overloads on 
:class:`~triqs.gfs.lazy_expressions.LazyExprTerminal` into a
binary tree of ``+`` and ``-`` nodes whose leaves are the descriptors
and scalars, and that tree is then walked by
:func:`~triqs.gfs.lazy_expressions.eval_expr_with_context` inside 
:meth:`~triqs.gfs.gf.Gf.__lshift__`.

End users do not need to touch this module directly; it is documented
mainly so that authors of new descriptors understand the protocol.
"""

from functools import reduce

#__all__ = ['LazyExpr', 'LazyExprTerminal', 'eval_expr_with_context', 'lazy', 'lazy_function', 'transform', 'eval_expr']
__all__ = ['LazyExpr', 'LazyExprTerminal', 'eval_expr_with_context', 'lazy_function', 'transform', 'eval_expr']

class __aux:
    """Mixin providing the operator overloads for descriptors and 
    :class:`~triqs.gfs.lazy_expressions.LazyExpr` nodes.

    Each operator builds a fresh :class:`~triqs.gfs.lazy_expressions.LazyExpr` 
    node rather than evaluating eagerly.
    """

    def __add__(self, y):
        """Build the lazy node ``self + y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("+", LazyExpr(self), LazyExpr(y))
    
    def __sub__(self, y):
        """Build the lazy node ``self - y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("-", LazyExpr(self), LazyExpr(y))
    
    def __mul__(self, y):
        """Build the lazy node ``self * y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("*", LazyExpr(self), LazyExpr(y))
    
    def __truediv__(self, y):
        """Build the lazy node ``self / y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("/", LazyExpr(self), LazyExpr(y))

    def __radd__(self, y):
        """Reflected lazy ``y + self``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("+", LazyExpr(y), LazyExpr(self))
    
    def __rsub__(self, y):
        """Reflected lazy ``y - self``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("-", LazyExpr(y), LazyExpr(self))
    
    def __rmul__(self, y):
        """Reflected lazy ``y * self``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("*", LazyExpr(y), LazyExpr(self))
    
    def __rtruediv__(self, y):
        """Reflected lazy ``y / self``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("/", LazyExpr(y), LazyExpr(self))

    def __iadd__(self, y):
        """In-place lazy addition ``self += y`` via 
        :meth:`~triqs.gfs.lazy_expressions.LazyExpr.set_from`.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return self.set_from(self+y)
    
    def __isub__(self, y):
        """In-place lazy subtraction ``self -= y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return self.set_from(self-y)
    
    def __imul__(self, y):
        """In-place lazy multiplication ``self *= y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return self.set_from(self*y)
    
    def __itruediv__(self, y):
        """In-place lazy division ``self /= y``.

        Parameters
        ----------
        y : LazyExpr, descriptor or scalar

        Returns
        -------
        LazyExpr
        """
        return self.set_from(self/y)

    def __call__(self, *args):
        """Build the lazy function-application node ``self(*args)``.

        Parameters
        ----------
        *args
            Arguments to bind into the resulting ``"F"``-tagged node.

        Returns
        -------
        LazyExpr
        """
        return LazyExpr("F", make_lazy(self), *list(map(make_lazy, args)))

class LazyExprTerminal (__aux):
    """Base class for any object that can appear as a leaf of a 
    :class:`~triqs.gfs.lazy_expressions.LazyExpr`.

    Inheriting from this class brings in the arithmetic operator
    overloads from ``__aux``, so derived objects (typically descriptors)
    can be combined with scalars and with each other to build a
    :class:`~triqs.gfs.lazy_expressions.LazyExpr` tree.
    """
    pass

class LazyExpr (__aux):
    """A node in a lazy expression tree.

    Each node carries a ``tag`` and a list of ``childs``. Tags are:

    * ``"T"`` — terminal; ``childs[0]`` is the wrapped value
      (descriptor, scalar, ...).
    * ``"+"``, ``"-"``, ``"*"``, ``"/"`` — binary arithmetic; the two
      children are themselves :class:`~triqs.gfs.lazy_expressions.LazyExpr` 
      nodes.
    * ``"F"`` — function application; ``childs[0]`` is a terminal
      wrapping the callable, the remaining children are its arguments.

    Operator overloads on the parent ``__aux`` build these trees from
    Python expressions (``a + b``, ``f(a, b)``, ...).

    Parameters
    ----------
    *args
        * Single argument — if it is a :class:`~triqs.gfs.lazy_expressions.LazyExpr` 
          its ``tag`` and ``childs`` are aliased; otherwise the argument is
          wrapped as a terminal (``tag = 'T'``).
        * Two or more arguments — first is interpreted as the
          ``tag``, the rest as ``childs``.

    Attributes
    ----------
    tag : str
        Node kind.
    childs : list
        Child nodes (or wrapped payload for terminals).
    """

    def __init__ (self, *args):
        if len(args) == 1:
            a0 = args[0]
            self.tag, self.childs = (a0.tag, a0.childs) if isinstance(a0, self.__class__) else ("T", [a0])
        elif len(args) >1:
            self.tag, self.childs = args[0], args[1:]
        else: raise ValueError("too few arguments")

    def copy(self):
        """Shallow copy of this node (children are shared).

        Returns
        -------
        LazyExpr
            A new node with the same ``tag`` and ``childs`` list.
        """
        return LazyExpr(self.tag, self.childs)

    def set_from(self, y):
        """In-place assignment: ``self`` is rewritten to match ``y``.

        Parameters
        ----------
        y : LazyExpr
            Source node; its ``tag`` and ``childs`` are aliased into
            ``self``.

        Returns
        -------
        LazyExpr
            ``self``, after the rewrite (for chaining).
        """
        self.tag, self.childs = y.tag, y.childs
        return self

    def is_terminal(self):
        """Whether this node is a leaf node.

        Returns
        -------
        bool
            ``True`` if ``tag == 'T'``, ``False`` otherwise.
        """
        return self.tag == "T"

    def get_terminal(self):
        """Wrapped payload, or ``None`` if this node is not a leaf.

        Returns
        -------
        object or None
            ``childs[0]`` when ``tag == 'T'``, otherwise ``None``.
        """
        return self.childs[0] if self.tag == "T" else None

    def __aux_print(self, F):
        op_priority = {'T': 100, "+": 1 , '-': 1, '*': 2, '/': 2}
        if self.tag == "T":  return F(self.childs[0])
        if self.tag == "F":
            return reduce (lambda s, e: s+ F(e), self.childs[1:], self.childs[0].get_terminal()[0] + "(" ) + ')'
        par = lambda op, e: "%s"%e if op_priority[e.tag] >= op_priority[op] else "(%s)"%e
        return "%s %s %s "%(par(self.tag , self.childs[0]), self.tag , par(self.tag , self.childs[1]))

    def __str__(self):
        """Human-readable rendering of the expression tree.

        Returns
        -------
        str
            Infix string with parentheses inserted according to
            operator precedence; terminals are stringified via
            :func:`str`.
        """
        return self.__aux_print(str)
    
    def __repr__(self):
        """:func:`repr`-based rendering of the expression tree.

        Returns
        -------
        str
        """
        return self.__aux_print(repr)

    #def __call__ (self, *args, **kwargs):


#-----------------------------------------------------

def eval_expr_with_context(eval_term, expr ):
    """Recursively reduce ``expr`` using ``eval_term`` on every leaf.

    Parameters
    ----------
    eval_term : callable
        Called as ``eval_term(value)`` on each terminal payload to map
        it to a concrete value (e.g. a :class:`~triqs.gfs.gf.Gf` produced 
        by applying a descriptor to a fresh target).
    expr : LazyExpr
        Expression tree to reduce.

    Returns
    -------
    object
        The numerical value of the expression after substitution and
        evaluation of binary operations and function applications.
    """
    if expr.tag == "T": return eval_term(expr.childs[0]) #eval the terminals

    if expr.tag == "F":
        f = expr.childs[0].get_terminal()[1]
        return f (*[eval_expr_with_context(eval_term, e) for e in expr.childs[1:]] )

    # Binary operations:
    ops = { "+": lambda x, y: x + y, "-": lambda x, y: x - y, "*": lambda x, y: x * y, "/": lambda x, y: x / y }
    return ops[expr.tag] (*[eval_expr_with_context(eval_term, e) for e in expr.childs] )

#-----------------------------------------------------

def make_lazy(x):
    """Wrap ``x`` in a :class:`~triqs.gfs.lazy_expressions.LazyExpr` 
    terminal node.

    Parameters
    ----------
    x : object
        Value to wrap (descriptor, scalar, 
        :class:`~triqs.gfs.lazy_expressions.LazyExpr`, ...). An existing 
        :class:`~triqs.gfs.lazy_expressions.LazyExpr` is returned as-is.

    Returns
    -------
    LazyExpr
        Terminal node holding ``x``.
    """
    return LazyExpr(x)

#-----------------------------------------------------

def lazy_function(name, F):
    """Wrap a Python callable so it can be applied to lazy expressions.

    Parameters
    ----------
    name : str
        Display name used when stringifying the tree.
    F : callable
        Function to invoke at evaluation time.

    Returns
    -------
    LazyExpr
        Terminal node holding ``(name, F)``; calling it as
        ``wrapped(*args)`` produces a ``"F"``-tagged expression node.
    """
    return LazyExpr("T", (name, F))

#-----------------------------------------------------

def transform (expr, Fnode, Fterm = lambda x: x ):
    """Recursively map ``Fnode`` / ``Fterm`` over a lazy expression.

    Parameters
    ----------
    expr : LazyExpr
        Expression to rewrite.
    Fnode : callable
        ``Fnode(tag, childs) -> (tag, childs)`` applied at every
        non-terminal node.
    Fterm : callable, optional
        ``Fterm(value) -> value`` applied at every terminal. Default 
        is the identity.

    Returns
    -------
    LazyExpr
        A new tree obtained by applying the rewrites bottom-up.
    """
    if expr.tag == "T": return LazyExpr("T", Fterm(expr.childs[0]))
    tag, ch = Fnode (expr.tag, [transform (e, Fnode) for e in expr.childs])
    ch = [LazyExpr(x) for x in ch]
    return LazyExpr (tag, *ch)

#-----------------------------------------------------

def all_terminals (expr):
    """Walk ``expr`` and yield each terminal's payload in left-to-right 
    order.

    Parameters
    ----------
    expr : LazyExpr
        Expression tree to traverse.

    Yields
    ------
    object
        Payload (``childs[0]``) of each terminal encountered.
    """
    if expr.tag == "T":
        yield expr.childs[0]
    else:
        for ch in expr.childs:
            for t in all_terminals(ch):
                yield t

def eval_expr (expr):
    """Reduce ``expr`` using an evaluation context discovered in its 
    terminals.

    If ``expr`` is not a :class:`~triqs.gfs.lazy_expressions.LazyExpr`, 
    it is returned unchanged. Otherwise the function looks for a terminal 
    exposing ``__lazy_expr_eval_context__()`` (typically a 
    :class:`~triqs.gfs.gf.Gf`), checks that every such terminal yields a 
    compatible context, and delegates to 
    :func:`~triqs.gfs.lazy_expressions.eval_expr_with_context`.

    Parameters
    ----------
    expr : LazyExpr or object
        Expression to reduce. Non-:class:`~triqs.gfs.lazy_expressions.LazyExpr` 
        values are returned unchanged.

    Returns
    -------
    object
        Reduced value of the expression (or ``expr`` itself if it was
        not a :class:`~triqs.gfs.lazy_expressions.LazyExpr`).

    Raises
    ------
    ValueError
        If no terminal can build an evaluation context, or if multiple
        terminals build incompatible contexts (e.g. Green's functions
        on different meshes).
    """
    if not isinstance (expr, LazyExpr): return expr # do nothing
    # first take all terminals
    C = [ t.__lazy_expr_eval_context__() for t in all_terminals(expr) if hasattr(t, "__lazy_expr_eval_context__") ]
    if C == []: raise ValueError("Evaluation impossible: expression is purely abstract")
    all_equal = reduce (lambda x, y: x and y , [ C[0] == x for x in C ])
    if not all_equal: raise ValueError("Evaluation impossible: various terminals lead to incompatible evaluation contexts: their type are not compatible for binary ops")
    C = C[0]
    return eval_expr_with_context(C, expr)

