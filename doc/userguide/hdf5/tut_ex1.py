from h5 import *
import numpy
R = HDFArchive('myfile.h5', 'w') # Opens the file myfile.h5, in read/write mode
R['mu'] = 1.29
R.create_group('S')
S= R['S']
S['a'] = "a string"
S['b'] = numpy.array([1,2,3])
del R,S # closing the files (optional: 
#file is closed when the references to R and subgroup are deleted)
