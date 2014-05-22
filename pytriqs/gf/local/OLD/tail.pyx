from tools import py_deserialize
import descriptors

cdef class TailGf:
    cdef tail _c
    def __init__(self, **d):
        """
        TailGf ( shape )
        TailGf ( data, mask, order_min )
        """
            # default values
        omin = -1
        omax =  8

        a = d.pop('data',None)
        if a==None :
            (N1, N2) = d.pop('shape')
            a = numpy.zeros((omax-omin+1,N1,N2), numpy.complex)
        m = d.pop('mask',None)
        if m==None :
            m = numpy.zeros(a.shape[1:3], int)
            m.fill(omax)
        o = d.pop('order_min',None)
        if o==None: o = omin

        assert len(d) == 0, "Unknown parameters in TailGf constructions %s"%d.keys()

        self._c = tail(array_view[dcomplex,THREE](a), array_view[long,TWO](m), o)
    
    property N1 : 
        def __get__(self): return self.shape[0]

    property N2 : 
        def __get__(self): return self.shape[1]

    def _make_slice(self, sl1, sl2):
        return self.__class__(data = self.data[:,sl1,sl2], mask = self.mask[sl1,sl2])

    def __repr__ (self) :
        omin = self.order_min
        while ((omin <= self.order_max) and (numpy.max(numpy.abs(self.data[omin-self.order_min,:,:])) < 1e-8)):
          omin = omin+1
        if omin == self.order_max+1:
          return "%s"%numpy.zeros(self.shape)
        else:
          return string.join([ "%s"%self[r]+ (" /" if r>0 else "") + " Om^%s"%(abs(r)) for r in range(omin, self.order_max+1) ] , " + ")

    def __call__(self, x) :
        val = 0.0
        for n in range(self.order_min, self.order_max+1):
          val += self[n] * x**(-n)
        return val
    
    #---- other operations ----
    def transpose (self) : 
        """Transpose the array : new view as in numpy"""
        return TailGf(data=self.data.transpose(), mask=self.mask.transpose())

    def conjugate(self) : 
        """Transpose the array : new view as in numpy"""
        return TailGf(data=self.data.conjugate(), mask=self.mask)
        


