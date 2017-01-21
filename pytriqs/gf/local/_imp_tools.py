from pytriqs.gf.singularities import TailGf
from pytriqs.gf.gf import Gf

def make_gf(clsname, mesh, target_shape, name, n_order_tail, indices = None, **kwargs):
    """
    """
    indices_pack = get_indices_in_dict(kwargs)
    if target_shape is None: 
      assert indices_pack, "No shape, no indices !"
      indices = indices_pack
      target_shape = [len(x) for x in indices]

    data = kwargs.pop('data', None) 
    tail = kwargs.pop('tail', None)
    if not tail : tail = kwargs.pop('singularity', None)
    if not tail : tail = TailGf(target_shape = target_shape, n_order=n_order_tail, order_min = -2)

    if kwargs: raise ValueError, clsname + ": Unused parameters %s were passed."%kwargs.keys()

    return Gf(mesh = mesh, data = data, target_shape = None if data is not None else target_shape,
              singularity = tail, indices = indices_pack, name = name) 

    
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
    indices_list=[list ( d['indices'][i]) for i in range(len(d['indices']))]
    d.pop('indices') #pop key from dict 

    # Now check the indices
    ty = set()
    for indices in indices_list:
     for x in indices:
      ty.add(type(x))
    assert len(ty) !=0, "No indices found !"
    assert len(ty)==1, " All indices must have the same type %s"%ty

    # If the indices are not string, make them string anyway
    indices_list = [[ str(x) for x in indices ] for indices in indices_list]

    return indices_list
