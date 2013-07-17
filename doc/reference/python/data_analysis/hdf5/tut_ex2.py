from pytriqs.archive import HDFArchive
from pytriqs.gf.local import GfImFreq

# Define a Green function 
G = GfImFreq ( indices = [1], beta = 10, n_points = 1000)
      
# Opens the file myfile.h5, in read/write mode
R = HDFArchive('myfile.h5', 'w')
# Store the object G under the name 'g1' and mu
R['g1'] = G
R['mu'] = 1.29
del R # closing the files (optional : file is closed when the R reference is deleted)




