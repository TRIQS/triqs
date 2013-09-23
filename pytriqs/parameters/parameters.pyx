from dcomplex cimport * 
from shared_ptr cimport *
from arrays cimport *   
from inserter_in_map cimport *   
from libcpp.pair cimport pair
from libcpp.vector cimport vector
from libcpp.string cimport string as std_string
from cython.operator cimport dereference as deref, preincrement as inc #dereference and increment operator
import numpy as np

cdef extern from "triqs/parameters/parameters.hpp" namespace "triqs::parameters" : 
    
    cdef cppclass extract_strict "triqs::utility::extract_strict" [T]: 
        extract_strict()
        bint is_possible(_object &)
        T invoke(_object &)

cdef class Parameters:
    """
    """
    #cdef parameters _c
    def __init__(self) : 
        """  """
        self._c = parameters()

    def __getitem__(self, key) :
        cdef _object _o = self._c[key]
        if extract_strict[long]().is_possible(_o) : return extract_strict[long]().invoke(_o)
        if extract_strict[double]().is_possible(_o) : return extract_strict[double]().invoke(_o)
        if extract_strict[std_string]().is_possible(_o) : return extract_strict[std_string]().invoke(_o)
        if extract_strict[int]().is_possible(_o) : return extract_strict[int]().invoke(_o)

        if extract_strict[parameters]().is_possible(_o) : 
            pp = Parameters()
            pp._c = extract_strict[parameters]().invoke(_o)
            return pp

        if extract_strict[array_view[long, ONE]]().is_possible(_o)  : return extract_strict[array_view[long,ONE]]().invoke(_o).to_python()
        if extract_strict[array_view[long, TWO]]().is_possible(_o)  : return extract_strict[array_view[long,TWO]]().invoke(_o).to_python()
        if extract_strict[array_view[long, THREE]]().is_possible(_o): return extract_strict[array_view[long,THREE]]().invoke(_o).to_python()
        if extract_strict[array_view[double,ONE]]().is_possible(_o) : return extract_strict[array_view[double,ONE]]().invoke(_o).to_python()
        if extract_strict[array_view[double,TWO]]().is_possible(_o) : return extract_strict[array_view[double,TWO]]().invoke(_o).to_python()
        if extract_strict[array_view[double,THREE]]().is_possible(_o) : return extract_strict[array_view[double,THREE]]().invoke(_o).to_python()
       
        if extract_strict[vector[std_string]]().is_possible(_o) : return extract_strict[vector[std_string]]().invoke(_o)
        if extract_strict[vector[long]]().is_possible(_o) : return extract_strict[vector[long]]().invoke(_o)
        
        raise ValueError, "Can not extract the key %s"%key

    def __setitem__(self, key, rhs) :
        if isinstance(rhs, int) : inserter_in_map[parameters,long](self._c)(key, rhs)
        elif isinstance(rhs, float) : inserter_in_map[parameters,double](self._c)(key, rhs)
        elif isinstance(rhs, str) : inserter_in_map[parameters,std_string](self._c)(key, rhs)
        elif isinstance(rhs, Parameters) : inserter_in_map[parameters,parameters](self._c)(key, (<Parameters>rhs)._c)
        elif isinstance(rhs, dict) : self[key] = Parameters().update(rhs)
        elif isinstance(rhs, list) or isinstance(rhs,tuple) :
            if set([type(x) for x in rhs]) == set([type('')]) : # list or tuple of string
                inserter_in_map[parameters,vector[std_string]](self._c)(key, rhs)
            elif set([type(x) for x in rhs]) == set([type(1)]) : # list or tuple of int
                inserter_in_map[parameters,vector[long]](self._c)(key, rhs)
            else : 
                raise TypeError, "List and tuple are not supported by Parameters. Please use numpy arrays"
        elif isinstance(rhs, np.ndarray) :
            try : inserter_in_map[parameters,array_view[long,ONE]](self._c)(key, array_view[long,ONE](rhs))
            except : pass
            try : inserter_in_map[parameters,array_view[long,TWO]](self._c)(key, array_view[long,TWO](rhs))
            except : pass
            try : inserter_in_map[parameters,array_view[long,THREE]](self._c)(key, array_view[long,THREE](rhs))
            except : pass
            try : inserter_in_map[parameters,array_view[double,ONE]](self._c)(key, array_view[double,ONE](rhs))
            except : pass
            try : inserter_in_map[parameters,array_view[double,TWO]](self._c)(key, array_view[double,TWO](rhs))
            except : pass
            try : inserter_in_map[parameters,array_view[double,THREE]](self._c)(key, array_view[double,THREE](rhs))
            except : pass
        else : 
            raise TypeError, "Unknown object : key = %s, rhs = %s"%(key, rhs)

    def __repr__(self) : 
        return printer[parameters]()(self._c)

    def update(self, d) : 
        for k,v in d.items(): self[k]=v
        return self
    
    def update2(self, **d) : return self.update(d)
   


