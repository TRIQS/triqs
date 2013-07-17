from tools import py_deserialize
import descriptors

cdef class TailGf:
    cdef tail _c
    def __init__(self, **d):
        """
        TailGf ( shape, size, order_min )
        TailGf ( data, order_min )
        """
        c_obj = d.pop('encapsulated_c_object', None)
        if c_obj :
            assert d == {}
            self._c = extractor [tail] (c_obj) ()
            return

        bss = d.pop('boost_serialization_string', None)
        if bss :
            assert d == {}, "Internal error : boost_serialization_string must be the only argument"
            boost_unserialize_into(<std_string>bss,self._c) 
            return 

        omin = d.pop('order_min')
        a = d.pop('data',None)
        if a==None :
            (N1, N2), s = d.pop('shape'), d.pop('size')
            a = numpy.zeros((s,N1,N2), numpy.complex)
        m = d.pop('mask',None)
        if m==None :
            m = numpy.zeros(a.shape[1:3], int)
            m.fill(omin+a.shape[0]-1)
        assert len(d) == 0, "Unknown parameters in TailGf constructions %s"%d.keys()
        self._c = tail(array_view[dcomplex,THREE](a), omin, array_view[long,TWO](m))
    
    #-------------- Reduction -------------------------------

    def __reduce__(self):
        return py_deserialize, (self.__class__,boost_serialize(self._c),)

    #-------------- Properties  -------------------------------

    property data : 
        """Access to the data array"""
        def __get__(self) :
            return self._c.data().to_python()

    property mask:
        """Access to the mask"""
        def __get__(self) :
            return self._c.mask_view().to_python()

    property shape : 
        def __get__(self) : return self.data.shape[1:3]

    property order_min : 
        """Min order of the expansion"""
        def __get__(self) : return self._c.order_min()

    property order_max : 
        """Max order of the expansion"""
        def __get__(self) : return self._c.order_max()

    property N1 : 
        def __get__(self): return self.shape[0]

    property N2 : 
        def __get__(self): return self.shape[1]

    property size : 
        """Length of the expansion"""
        def __get__(self) : return self._c.size()
    
    def copy(self) : 
        return self.__class__(data = self.data.copy(), order_min = self.order_min, mask = self.mask.copy())

    def copy_from(self, TailGf T) :
        assert self.order_min <= T.order_min, "Copy_from error " 
        self._c << T._c

    def _make_slice(self, sl1, sl2):
        return self.__class__(data = self.data[:,sl1,sl2], order_min = self.order_min, mask = self.mask[sl1,sl2])

    def __repr__ (self) :
        return string.join([ "%s"%self[r]+ (" /" if r>0 else "") + " Om^%s"%(abs(r)) for r in range(self.order_min, self.order_max+1) ] , " + ")

    def __getitem__(self,i) :
        """Returns the i-th coefficient of the expansion, or order Om^i"""
        if not self.has_coef(i) : raise IndexError, "Index %s is out of range"%i
        return self.data[i-self.order_min,:,:]

    def __setitem__(self,i, val) :
        """Sets the i-th coefficient of the expansion, or order Om^i"""
        if not self.has_coef(i) : raise IndexError, "Index %s is out of range"%i
        self.data[i-self.order_min,:,:] = val

    def has_coef(self, i):
        return (i >= self.order_min) and (i <= self.order_max)

    def __call__(self, x) :
        val = 0.0
        for n in range(self.order_min, self.order_max+1):
          val += self[n] * x**(-n)
        return val

    def invert(self) :
        self._c << inverse_c (self._c)

    #########      arithmetic operations    #################

    def __iadd__(self, TailGf arg):
        self._c << self._c + arg._c
        return self

    def __add__(self, TailGf y):
        cdef tail t = (<TailGf?>self)._c +  y._c
        return make_TailGf(t)
        #c = self.copy()
        #c += y
        #return c

    def __isub__(self, TailGf arg):
        self._c << self._c - arg._c
        return self

    def __sub__(self,TailGf y):
        cdef tail t = (<TailGf?>self)._c -  y._c
        return make_TailGf(t)
        #c = self.copy()
        #c -= y
        #return c

    def __imul__(self,arg):
        n = type(arg).__name__
        if n == 'TailGf' :
            self._c << self._c * (<TailGf?>arg)._c
        elif descriptors.is_scalar(arg):
            self._c << as_dcomplex(arg)* self._c
        else : 
            raise RuntimeError, " argument type not recognized in imul for %s"%arg
        return self

    def __mul__(self,arg):
        c = self.copy()
        c *= arg
        return c

    def __idiv__(self,arg):
        if descriptors.is_scalar(arg):
          self._c << self._c / as_dcomplex(arg)
        else:
          raise RuntimeError, "rhs must be a scalar"
        return self

    def __div__(self,arg):
        c = self.copy()
        c /= arg
        return c
    
    #---- other operations ----

    def zero(self) : 
        """Sets the expansion to 0"""
        self._c << as_dcomplex(0.0)

    def transpose (self) : 
        """Transpose the array : new view as in numpy"""
        return TailGf(data=self.data.transpose(), order_min=self.order_min, mask=self.mask.transpose())

    def conjugate(self) : 
        """Transpose the array : new view as in numpy"""
        return TailGf(data=self.data.conjugate(), order_min=self.order_min, mask=self.mask)
        
    def __write_hdf5__ (self, gr , char * key) :
        h5_write (make_h5_group(gr), key, self._c)

#----------------  Reading from h5 ---------------------------------------

def h5_read_TailGf( gr, std_string key) : 
    return make_TailGf( h5_extractor[tail]()(make_h5_group(gr),key))

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (TailGf, read_fun = h5_read_TailGf)

#-----------------------------------------------------
# C -> Python 
#-----------------------------------------------------

cdef inline make_TailGf ( tail x) : 
    return TailGf(encapsulated_c_object = encapsulate (&x))

