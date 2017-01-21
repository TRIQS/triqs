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
import re

class HDF5Scheme : 
    def __init__(self, classname, modulename, doc, read_fun= None) : 
        self.classname, self.modulename, self.doc, self.read_fun = classname, modulename,doc, read_fun
    def __str__(self) : 
        return """
        Name of the class : %s
        Name of the module : %s
        Documentation : %s"""(self.classname,self.modulename,self.doc)
        
_hdf5_schemes_dict= {}

def register_class (cls, doc = None, read_fun = None) :
    """
     For each class, register it with::

         from HDFArchive_Schemes_dict import register_class 
         register_class (GfImFreq, doc= doc_if_different_from cls._hdf5_data_scheme_doc_ )

    """
    SchemeName = cls._hdf5_data_scheme_ if hasattr(cls,"_hdf5_data_scheme_") else cls.__name__ 
    assert SchemeName not in _hdf5_schemes_dict, "class %s is already registered"%SchemeName
    doc = doc if doc else (cls._hdf5_data_scheme_doc_ if hasattr(cls,"_hdf5_data_scheme_doc_") else {})
    _hdf5_schemes_dict [SchemeName] = HDF5Scheme (cls.__name__, cls.__module__,doc, read_fun)

def hdf_scheme_access (SchemeName): 
    # We need to find the key in _hdf5_schemes_dict which AS A REGEX will MATCH SchemeName
    l = [x for x in _hdf5_schemes_dict.keys() if re.match(x, SchemeName)]
    if len(l) ==0 : 
        raise KeyError, "HDF5 Data Scheme %s is not registered"%SchemeName
    if len(l) >1 : # must be perfectly ordered
        l = sorted(l)
        if not all( l[i] in l[i+1] for i in range(len(l)-1)): # they are not ordered like Gf, GfImFreq, GfImFreq_x_ImFreq ...
            raise KeyError, "HDF5 Data Scheme %s is ambiguous. Possible schemes are %s"%(SchemeName,l)
    return _hdf5_schemes_dict[l[-1]] # Take the most refined

#def classname (SchemeName) : 
#   return access(SchemeName).classname 

#def modulename (SchemeName) : 
#   return access(SchemeName).modulename 


