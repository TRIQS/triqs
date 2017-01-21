################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2016 by I. Krivenko, M. Ferrero, O. Parcollet
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

from itertools import izip, product, chain
import operator
import numpy as np

class Block2Gf(object):
    """
    Generic Green's Function by two-index Block.
    """
    def __init__(self, name_list1, name_list2, block_list, **kwargs):
        """
    * Block2Gf(name_list1, name_list2, block_list, make_copies = False, name = 'G2')

        * ``name_list1``: list of the first names of the blocks, e.g. ["up","down"].
        * ``name_list2``: list of the second names of the blocks, e.g. ["up","down"].
        * ``block_list``: can be one of

            * list of lists of Green's function block objects; dimensions of the outer and inner lists
              must coincide with lengths of ``name_list1`` and ``name_list2`` respectively;
            * callable object taking two block names and returning a Green's function block object.

        * ``make_copies``: if True, it makes a copy of the blocks and build the Green's function from these copies.
        * ``note``: optional note describing this Green's Function object

        """
        # first extract the optional name argument
        self.name = kwargs.pop('name','')
        self.note = kwargs.pop('note','')

        rename_gf = kwargs.pop('rename_gf',True)
        make_copies = kwargs.get('make_copies', False)

        # First a few checks
        for name_list, var_name in ((name_list1,'name_list1'),(name_list2,'name_list2')):
            assert name_list, "%s: Empty list of block names" % var_name
            assert len(set(name_list)) == len(name_list), "%s: block names are not unique" % var_name
            for bn in name_list:
                try:
                    hash(bn)
                except:
                    raise RuntimeError, "%s: block name '%s' is not hashable" % (var_name,bn)
            for bn in name_list:
                assert not str(bn).startswith('__'), "%s: block names should not start with __" % var_name
            assert 'block_names' not in name_list, "%s: 'block_names' is a reserved keyword. It is not authorized as a block name" % var_name

        self.__indices1, self.__indices2 = tuple(name_list1), tuple(name_list2)
        self.__GFlist = []

        if callable(block_list):
            for bn1 in name_list1:
                self.__GFlist.append([])
                for bn2 in name_list2:
                    self.__GFlist[-1].append(block_list(bn1,bn2).copy() if make_copies else block_list(bn1,bn2))
        elif operator.isSequenceType(block_list):
            try:
                assert len(block_list) == len(name_list1), "incorrect outer size of block_list"
                for g_row in block_list:
                    assert len(g_row) == len(name_list2), "incorrect inner size of block_list"
                    self.__GFlist.append([])
                    for g in g_row:
                        self.__GFlist[-1].append(g.copy() if make_copies else g)
            except TypeError: pass
        else:
            raise RuntimeError, "block_list is not understood, see the documentation"

        # All blocks must have the same type
        if len(set([type(g) for g in chain.from_iterable(self.__GFlist)])) != 1:
            raise RuntimeError, "Block2Gf: All block must have the same type"

        # Add the name to the G
        if rename_gf:
            for i,g in self: g.name = "%s_%s"%(str(self.name),i) if self.name else '%s'%(i,)

    #------------ copy and construction -----------------------------------------------

    def copy(self,*args):
       """Returns a (deep) copy of self (i.e. new independent Blocks initialised with self) """
       return self.__class__(self.__indices1[:], self.__indices2[:],
                             [[g.copy(*args) for g in g_row] for g_row in self.__GFlist], make_copies=False)

    def view_selected_blocks(self, selected_blocks1, selected_blocks2):
        """Returns a VIEW of the selected blocks of self, in the same order as self."""
        for bn in selected_blocks1: assert bn in self.__indices1, "selected_blocks1: selected blocks must be existing blocks"
        for bn in selected_blocks2: assert bn in self.__indices2, "selected_blocks2: selected blocks must be existing blocks"
        return self.__class__ (selected_blocks1, selected_blocks2, lambda bn1, bn2: self[bn1,bn2], make_copies=False)

    def copy_selected_blocks(self, selected_blocks1, selected_blocks2):
        """Returns a COPY of the selected blocks of self, in the same order as self."""
        for bn in selected_blocks1: assert bn in self.__indices1, "selected_blocks1: selected blocks must be existing blocks"
        for bn in selected_blocks2: assert bn in self.__indices2, "selected_blocks2: selected blocks must be existing blocks"
        return self.__class__ (selected_blocks1, selected_blocks2, lambda bn1, bn2: self[bn1,bn2], make_copies=True)

    def copy_from(self, G2):
        """Copy the Green's function from G2: G2 MUST have the same structure!"""
        assert isinstance(G2, Block2Gf)
        for (i,g), (i2,g2) in izip(self,G2):
           assert g.data.shape == g2.data.shape, "Blocks %s and %s of the Green Function do have the same dimension"%(i1,i2)
        for (i,g),(i2,g2) in izip(self,G2): g.copy_from(g2)

     #--------------  Iterators -------------------------

    def __iter__(self):
        return izip(product(self.__indices1, self.__indices2), chain.from_iterable(self.__GFlist))

    #---------------------------------------------------------------------------------

    def _first(self):
        return self.__GFlist[0][0]

    def __len__(self):
        return len(self.__indices1) * len(self.__indices2)

    #---------------- Properties -------------------------------------------

    @property
    def indices(self):
        """A generator of the block indices"""
        for ind in product(self.__indices1, self.__indices2):
            yield ind

    @property
    def all_indices(self):
        """  An Iterator on Block2Gf indices and indices of the blocks of the form:
            block_index1,block_index2,n1,n2, where n1,n2 are indices of the block"""
        for sig, g in self:
            # A bit hacky...
            mesh_dim = len(g.mesh.components) if hasattr(g.mesh, 'components') else 1
            target_shape = g.data.shape[mesh_dim:]
            for i in product(*map(range, target_shape)):
                yield (sig[0],sig[1]) + i

    @property
    def n_blocks(self):
        """ Number of blocks"""
        return len(self)

    #----------------------   IO    -------------------------------------

    def __reduce__(self):
        return lambda cl,name,dic: cl.__factory_from_dict__(name, dic), (self.__class__,self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {"%s_%s"%bn : g for bn, g in self}
        d['block_names1'] = np.array(list(self.__indices1))
        d['block_names2'] = np.array(list(self.__indices2))
        d['note'] = self.note
        return d

    @classmethod
    def __factory_from_dict__(cls, name, d):
        block_names1, block_names2 = tuple(d.pop('block_names1')), tuple(d.pop('block_names2'))
        note = d.pop('note','')
        expected_keys = ["%s_%s" % bn for bn in product(block_names1, block_names2)]
        assert sorted(expected_keys) == sorted(d.keys()), "Reload mismatch: the indices and the group names do not corresponds"
        res = cls(block_names1, block_names2, lambda bn1, bn2: d["%s_%s"%(bn1,bn2)], make_copies=False, name=name, note=note)
        return res

    #--------------  Pretty print -------------------------

    def __repr__(self):
        s =  "Green's Function %s composed of %d 2-index blocks: \n"%(self.name,self.n_blocks)
        for i,g in self:
            s += " %s \n"%repr(g)
        if self.note: s += "NB: %s\n"%self.note
        return s

    def __str__ (self):
           return self.name if self.name else repr(self)

    #--------------  Bracket operator []  -------------------------

    def __getitem__(self,key):
        try:
            g = self.__GFlist[self.__indices1.index(key[0])][self.__indices2.index(key[1])]
        except KeyError:
            raise IndexError, "Block index '" + repr(key) + "' incorrect. Possible indices are: " + repr(list(product(self.__indices1,self.__indices2)))
        return g

    def __setitem__(self,key,val):
        try:
            g = self.__GFlist[self.__indices1.index(key[0])][self.__indices2.index(key[1])]
        except KeyError:
            raise IndexError, "Block index '" + repr(key) + "' incorrect. Possible indices are: "+ repr(list(product(self.__indices1,self.__indices2)))
        g << val

    # -------------- Various operations -------------------------------------

    def __lshift__(self, A):
        """ A can be 2 things:
          * G << any_init will init all the Block2Gf with the initializer
          * G << g2 where g2 is a Block2Gf will copy g2 into self
          """
        if isinstance(A, self.__class__):
           for bn, g in self: g.copy_from(A[bn])
        else:
           for bn, g in self:  self[bn] << A[bn]
        return self

    def __iadd__(self,arg):
        if isinstance(arg, self.__class__):
            for bn, g in self: self[bn] += arg[bn]
        elif operator.isSequenceType(arg):
            assert len(arg) == len(self.__GFlist), "list of incorrect length"
            for l, (bn, g) in izip(arg,self.__GFlist): self[bn] += l
        else:
            for bn, g in self: self[bn] += arg
        return self

    def __add__(self,y):
        c = self.copy()
        c += y
        return c

    def __radd__(self,y): return self.__add__(y)

    def __isub__(self,arg):
        if isinstance(arg, self.__class__):
           for bn, g in self: self[bn] -= arg[bn]
        elif operator.isSequenceType(arg):
            assert len(arg) == len(self.__GFlist) , "list of incorrect length"
            for l, (bn, g) in izip(arg,self.__GFlist): self[bn] -= l
        else:
            for bn, g in self: self[bn] -= arg
        return self

    def __sub__(self,y):
        c = self.copy()
        c -= y
        return c

    def __rsub__(self,y):
        c = (-1)*self.copy()
        c += y
        return c

    def __imul__(self,arg):
        if isinstance(arg, self.__class__):
            for bn, g in self: self[bn] *= arg[bn]
        elif operator.isSequenceType(arg):
            assert len(arg) == len(self.__GFlist) , "list of incorrect length"
            for l, (bn, g) in izip(arg,self.__GFlist): self[bn] *= l
        else:
            for bn, g in self: self[bn] *= arg
        return self

    def __mul__(self,y):
        c = self.copy()
        c *= y
        return c

    def __rmul__(self,x): return self.__mul__(x)

    def __idiv__(self,arg):
        if operator.isSequenceType(arg):
            assert len(arg) == len(self.__GFlist) , "list of incorrect length"
            for l, (bn, g) in izip(arg,self.__GFlist): self[bn] /= l
        else:
            for bn, g in self: self[bn] /= arg
        return self

    def __div__(self,y):
        c = self.copy()
        c /= y
        return c

    def __neg__(self):
        c = self.copy()
        c *= -1
        return c
   #-----------------------------plot protocol -----------------------------------

    def _plot_(self, opt_dict):
        """ Implement the plot protocol"""
        r = []
        for bn, g in self:
            initial_dict = opt_dict.copy()
            r += g._plot_(initial_dict)
        self.name, name_kept = self.name, opt_dict.pop('name', self.name)
        first_g_name = self._first().name
        ylabel = r[0]['ylabel'].replace(first_g_name, self.name) if first_g_name else self.name
        for dic in r:
           dic['ylabel'] = ylabel # replace the ylabel of the elements to a single ylabel
        self.name = name_kept
        return r

   #--------------------------------------------------------------------------

    def zero(self):
        for i,g in self: g.zero()

    def __check_attr(self,attr):
        if not hasattr(self._first(), attr):
           raise RuntimeError, "The blocks of this Green's Function do not possess the %s method" % attr

#---------------------------------------------------------

from pytriqs.archive.hdf_archive_schemes import register_class
register_class(Block2Gf)

