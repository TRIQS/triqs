from types import SliceType
import descriptor_base

class LazyCTX:
    def __init__ (self, G): 
        self.G = G
    def _is_compatible_for_ops(self, g): 
        m1,m2  = self.G.mesh, g.mesh
        return m1 is m2 or m1 == m2
    def __eq__ (self, y):
        return isinstance(y, self.__class__) and self._is_compatible_for_ops(y.G)
    def __call__ (self, x): 
        if not isinstance(x, descriptor_base.Base): return x
        tmp = self.G.copy()
        x(tmp)
        return tmp

def get_indices_in_dict(d): 
    """
    :param d: dictionary with at least keys 'indices' or ('indicesL' and 'indicesR')
    works for indices of matrix_valued and tensor_valued<3> functions
    """
    if not ( ('indicesL' in d and 'indicesR' in d) or 'indices' in d): return ()
    
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

def get_indices_in_dict_tv(d): 
    """
    :param d: dictionary with at least keys 'indices'
    works for indices of tensor_valued<3> functions
    """
    if not ( 'indices' in d): return ()
    
    indicesR1 = list ( d['indices'][0])
    indicesR2 = list ( d['indices'][1])
    indicesR3 = list ( d['indices'][2])
    d.pop('indices') #pop key from dict 

    # Now check the indices
    ty = set([type(x) for x in indicesR1]+[type(x) for x in indicesR2]+ [type(x) for x in indicesR3])
    assert len(ty) !=0, "No indices found !"
    assert len(ty)==1, " All indices must have the same type %s"%ty

    # If the indices are not string, make them string anyway
    indicesR1 = [ str(x) for x in indicesR1 ]     
    indicesR2 = [ str(x) for x in indicesR2 ]     
    indicesR3 = [ str(x) for x in indicesR3 ]     

    return indicesR1, indicesR2, indicesR3
