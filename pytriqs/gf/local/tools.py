from types import SliceType
import descriptors

class LazyCTX:
    def __init__ (self, G): 
        self.G = G
    def _is_compatible_for_ops(self, g): 
        m1,m2  = self.G.mesh, g.mesh
        return m1 is m2 or m1 == m2
    def __eq__ (self, y):
        return isinstance(y, self.__class__) and self._is_compatible_for_ops(y.G)
    def __call__ (self, x): 
        if not isinstance(x, descriptors.Base): return x
        tmp = self.G.copy()
        x(tmp)
        return tmp

class IndicesConverter: 
    def __init__(self, indices): 
        if indices == None: # none is a trivial converter
            self.indices=None
            return

        try: 
            self.indices = list(indices)[:] # make a copy
        except:
            raise RuntimeError, "Indices must be a list or transformable into a list %s"(indices,)
        assert len(set(repr(x) for x in self.indices)) == len(self.indices), "Error: indices are not unique !!!"
        assert self.indices != [], "Error: indices are empty!!!"
        try:
            # a continuous list of ordered integers
            self.__indices_are_sliceable = (self.indices== range(min(self.indices),max(self.indices)+1)) 
            self.__indexmin = min(self.indices)
            self.__indexmax = max(self.indices)
            #self.__indexlen = self.__indexmax - self.__indexmin +1
        except:
            self.__indices_are_sliceable =False
        self.Length = len(self.indices)
    
    def convertToNumpyIndex(self,a,noslice=False):
        """
        Transcription of one Python index/slice into the index/slice of the numpy
        """
        if self.indices==None: 
            if type(a)==SliceType: return a
            return a if noslice else slice(a,a+1,1)

        if type(a)==SliceType:
            if not self.__indices_are_sliceable: raise IndexError, "Indices %s can't be sliced"%self.indices
            # put the slice to start at 0, and clip it
            sta,sto,ste = slice(a.start, a.stop , a.step).indices(self.__indexmax+1)
            return slice( max(0,sta-self.__indexmin), sto - self.__indexmin, ste)
                    
        try:
            s1 = self.indices.index(a) 
        except:
            raise IndexError, "Index %s out of range %s"%(a,self.indices)
        return s1 if noslice else slice(s1,s1+1,1)

def get_indices_in_dict( d): 
    # exclusive: size = (n1,n2) or IndicesL/R
    if 'indices_pack' in d: 
        indicesL, indicesR = d.pop('indices_pack')
    else:
        indicesL = list ( d.pop('indicesL',()) or d.pop('indices',()) )
        indicesR = list ( d.pop('indicesR',()) or indicesL  )

    # Now check the indices
    ty = set([type(x) for x in indicesL]+[type(x) for x in indicesR])
    assert len(ty) !=0, "No indices found !"
    assert len(ty)==1, " All indices must have the same type %s"%ty

    # If the indices are not string, make them string anyway
    indicesL = [ str(x) for x in indicesL ]     
    indicesR = [ str(x) for x in indicesR ]  

    return indicesL, indicesR

def py_deserialize( cls, s): 
    return cls(boost_serialization_string = s)


