
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
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

# h5py
import numpy,string,h5py
class HDFArchiveGroupBasicLayer :
    _class_version = 1

    def __init__(self, parent, subpath ):
        """  """
        self.options = parent.options
        self._group = parent._group[subpath] if subpath else parent._group
        assert type(self._group) in [h5py.Group,h5py.File], "Internal error"
        self.ignored_keys = [] 
        self.cached_keys = self._group.keys()

    def _init_root(self, LocalFileName, open_flag) :
        try :
            fich = h5py.File(LocalFileName, open_flag)
        except :
            print "Cannot open the HDF file %s"%LocalFileName
            raise
        # checking the version
        if open_flag not in ['r','r+','a'] :
            self._version = self._class_version
        else :
            try :
                self._version = int(fich.attrs['HDFArchive_Version'])
            except :
                self._version = 1
            if self._version > self._class_version :
                raise IOError, "File %s is too recent for this version of HDFArchive module"%Filename
        self._group = fich

    def is_group(self,p) :
        """Is p a subgroup ?"""
        assert len(p)>0 and p[0]!='/'
        return p in self.cached_keys and type(self._group[p]) == h5py.Group

    def is_data(self,p) :
        """Is p a leaf ?"""
        assert len(p)>0 and p[0]!='/'
        return p in self.cached_keys and type(self._group[p]) == h5py.Dataset

    def write_attr (self, key, val) :
        self._group.attrs.create(key, val)
        #self._group.attrs[key] =  val

    def read_attr(self,attribute_name) :
        return self._group.attrs[attribute_name]

    def _read (self, key) :
        A = self._group[key]
        val =  numpy.array(A) if A.shape!=() else A.value
        if self.options["UseAlpsNotationForComplex"] and '__complex__' in self._group[key].attrs :
            assert type(val) == numpy.ndarray, 'complex tag is set, but I have not an array'
            assert not numpy.iscomplexobj(val), 'complex tag is set, but I have a complex !'
            if len(val.shape) == 1 :
                val = val[0] + 1j * val[1]
            else : 
                val = val[...,0] + 1j*val[...,1]

        def _numpy_scalar_to_python_scalar (v) :
            n= numpy
            if type(v) in [n.int, n.int8, n.int16, n.int32, n.int64] : return int(v)
            if type(v) in [n.uint8, n.uint16, n.uint32, n.uint64] : return int(v)
            if type(v) in [n.float,  n.float32, n.float64] : return float(v)
            if type(v) in [n.complex, n.complex64, n.complex128] : return complex(v)
            if type(v) in [n.str_, n.string_] : return str(v)
            # converts the array of numpy string into a list of normal python strings
            if type(v) is n.ndarray and v.dtype.type is n.string_ : return map(str,v)
            return v 

        return _numpy_scalar_to_python_scalar ( val)

    def _write_array(self, key, A) :
        c =  self.options["UseAlpsNotationForComplex"] and numpy.iscomplexobj(A)  
        if c:
            val  = numpy.zeros( A.shape + (2,) )
            val[...,0], val[...,1] = A.real, A.imag
        else : 
            val = A
        self._group[key] = numpy.array(val,copy=1,order='C')
        if c : self._group[key].attrs["__complex__"] = 1
        self.cached_keys.append(key)

    def _write_scalar(self, key, A) :
        c =  self.options["UseAlpsNotationForComplex"] and type(A) ==type (1j)
        val = numpy.array([A.real, A.imag]) if c else A
        self._group[key] =val
        if c : self._group[key].attrs["__complex__"]= 1
        self.cached_keys.append(key)

    def _flush(self):
        if bool(self._group): self._group.file.flush()

    def _close(self):
        if bool(self._group): self._group.file.close()

    def create_group (self,key):
        self._group.create_group(key)
        self.cached_keys.append(key)

    def keys(self) :
        return self.cached_keys

    def _clean_key(self,key, report_error=False) :
        if report_error and key not in self.cached_keys :
             raise KeyError, "Key %s is not in archive !!"%key
        if key in self.cached_keys :
          del self._group[key]
          self.cached_keys.remove(key)
        else: raise KeyError, "Key %s is not in archive !!"%key

