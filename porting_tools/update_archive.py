import h5py
import sys
import numpy

if len(sys.argv) < 3:
  print "Usage: python update_archive.py old_archive new_archive"
  sys.exit()

print """
This script is an attempt to update your archive to TRIQS 1.0.
Please keep a copy of your old archive as this script is
** not guaranteed ** to work for your archive.
If you encounter any problem please report it on github!
"""

# The dictionary of modifications to make
modifications = {
  'GF': {'SSS': 'BlockGf', '__Name': 'name', '__Note': 'note', '__BlockIndicesList': 'indices'},
  'GFBloc_ImFreq': {'SSS': 'GfImFreq'},
  'GFBloc_ReFreq': {'SSS': 'GfReFreq'},
  'GFBloc_ImTime': {'SSS': 'GfImTime'},
  'GFBloc_ReTime': {'SSS': 'GfReTime'},
  'GFBloc_ImLegendre': {'SSS': 'GfLegendre'},
  'MeshGF': {'SSS': 'MeshGf'},
  'TailGF': {'SSS': 'TailGf'},
  'DOS': {'Name': 'name'},
  'BlockGf': {'__Name': 'name', '__Note': 'note', '__BlockIndicesList': 'indices'}
}


def sanitize_imfreq(origin, dest):

    dest.create_dataset(name="data", data=origin["Data"].value.transpose((2,0,1,3)))
    dest["data"].attrs.create('__complex__', "1")

    dest.create_group(name="indices")
    exec("indL = %s"%origin["IndicesL"].value)
    exec("indR = %s"%origin["IndicesR"].value)
    indL = [ str(i) for i in indL ]
    indR = [ str(i) for i in indR ]
    dest["indices"].create_dataset(name="left", data=indL)
    dest["indices"].create_dataset(name="right", data=indR)

    dest.create_group(name="singularity")
    dest["singularity"].create_dataset(name="data", data=origin["Tail"]["array"].value)
    dest["singularity"]["data"].attrs.create('__complex__', "1")
    dest["singularity"].create_dataset(name="omin", data=origin["Tail"]["OrderMinMIN"].value)
    mask = numpy.zeros( dest["singularity"]["data"].shape[0:2], numpy.integer )
    mask.fill(origin["Tail"]["OrderMax"].value)
    dest["singularity"].create_dataset(name="mask", data=mask)

    dest.create_group(name="mesh")
    beta = origin["Mesh"]["Beta"].value
    pi = numpy.arccos(-1)
    size = numpy.uint(len(origin["Mesh"]["array"].value))
    dest["mesh"].create_dataset(name="kind", data=2)
    dest["mesh"].create_dataset(name="min", data=pi/beta)
    dest["mesh"].create_dataset(name="max", data=(2*size+1)*pi/beta)
    dest["mesh"].create_dataset(name="size", data=size)
    dest["mesh"].create_group(name="domain")
    dest["mesh"]["domain"].create_dataset(name="beta", data=beta)
    dest["mesh"]["domain"].create_dataset(name="statistic", data={"Fermion":"F", "Boson":"B"}[origin["Mesh"]["Statistic"].value] )

    return ['Data', 'IndicesL', 'IndicesR', 'Mesh', 'Name', 'Note', 'Tail']


def sanitize_imtime(origin, dest):

    dest.create_dataset(name="data", data=origin["Data"].value.transpose((2,0,1)))

    dest.create_group(name="indices")
    exec("indL = %s"%origin["IndicesL"].value)
    exec("indR = %s"%origin["IndicesR"].value)
    indL = [ str(i) for i in indL ]
    indR = [ str(i) for i in indR ]
    dest["indices"].create_dataset(name="left", data=indL)
    dest["indices"].create_dataset(name="right", data=indR)

    dest.create_group(name="singularity")
    dest["singularity"].create_dataset(name="data", data=origin["Tail"]["array"].value)
    dest["singularity"]["data"].attrs.create('__complex__', "1")
    dest["singularity"].create_dataset(name="omin", data=origin["Tail"]["OrderMinMIN"].value)
    mask = numpy.zeros( dest["singularity"]["data"].shape[0:2], numpy.integer )
    mask.fill(origin["Tail"]["OrderMax"].value)
    dest["singularity"].create_dataset(name="mask", data=mask)

    dest.create_group(name="mesh")
    beta = origin["Mesh"]["Beta"].value
    size = numpy.uint(len(origin["Mesh"]["array"].value))
    min_t = origin["Mesh"]["array"].value[0]
    if min_t > 1e-10:
      kind = 0
      assert(abs(min_t - 0.5*beta/size) < 1e-10)
    else:
      kind = 2
    dest["mesh"].create_dataset(name="kind", data=kind)
    dest["mesh"].create_dataset(name="min", data=0.0)
    dest["mesh"].create_dataset(name="max", data=beta)
    dest["mesh"].create_dataset(name="size", data=size)
    dest["mesh"].create_group(name="domain")
    dest["mesh"]["domain"].create_dataset(name="beta", data=beta)
    dest["mesh"]["domain"].create_dataset(name="statistic", data={"Fermion":"F", "Boson":"B"}[origin["Mesh"]["Statistic"].value] )

    return ['Data', 'IndicesL', 'IndicesR', 'Mesh', 'Name', 'Note', 'Tail']

def sanitize_legendre(origin, dest):

    dest.create_dataset(name="data", data=origin["Data"].value.transpose((2,0,1,3)))

    dest.create_group(name="indices")
    exec("indL = %s"%origin["IndicesL"].value)
    exec("indR = %s"%origin["IndicesR"].value)
    indL = [ str(i) for i in indL ]
    indR = [ str(i) for i in indR ]
    dest["indices"].create_dataset(name="left", data=indL)
    dest["indices"].create_dataset(name="right", data=indR)

    dest.create_group(name="mesh")
    beta = origin["Mesh"]["Beta"].value
    size = numpy.uint(len(origin["Mesh"]["array"].value))
    dest["mesh"].create_group(name="domain")
    dest["mesh"]["domain"].create_dataset(name="beta", data=beta)
    dest["mesh"]["domain"].create_dataset(name="n_max", data=size)
    dest["mesh"]["domain"].create_dataset(name="statistic", data={"Fermion":"F", "Boson":"B"}[origin["Mesh"]["Statistic"].value] )

    return ['Data', 'IndicesL', 'IndicesR', 'Mesh', 'Name', 'Note', 'Tail']

def sanitize_refreq(origin, dest):

    dest.create_dataset(name="data", data=origin["Data"].value.transpose((2,0,1,3)))
    dest["data"].attrs.create('__complex__', "1")

    dest.create_group(name="indices")
    exec("indL = %s"%origin["IndicesL"].value)
    exec("indR = %s"%origin["IndicesR"].value)
    indL = [ str(i) for i in indL ]
    indR = [ str(i) for i in indR ]
    dest["indices"].create_dataset(name="left", data=indL)
    dest["indices"].create_dataset(name="right", data=indR)

    dest.create_group(name="singularity")
    dest["singularity"].create_dataset(name="data", data=origin["Tail"]["array"].value)
    dest["singularity"]["data"].attrs.create('__complex__', "1")
    dest["singularity"].create_dataset(name="omin", data=origin["Tail"]["OrderMinMIN"].value)
    mask = numpy.zeros( dest["singularity"]["data"].shape[0:2], numpy.integer )
    mask.fill(origin["Tail"]["OrderMax"].value)
    dest["singularity"].create_dataset(name="mask", data=mask)

    dest.create_group(name="mesh")
    size = numpy.uint(len(origin["Mesh"]["array"].value))
    min_w = origin["Mesh"]["array"].value[0]
    max_w = origin["Mesh"]["array"].value[-1]
    dest["mesh"].create_dataset(name="kind", data=1)
    dest["mesh"].create_dataset(name="min", data=min_w)
    dest["mesh"].create_dataset(name="max", data=max_w)
    dest["mesh"].create_dataset(name="size", data=size)

    return ['Data', 'IndicesL', 'IndicesR', 'Mesh', 'Name', 'Note', 'Tail']



def copy_and_correct(origin, dest):

  scheme = None
  exclude = []

  # copy attributes in destination archive
  for at in origin.attrs.keys():

    # determine if the group is a TRIQS scheme
    if at == 'TRIQS_HDF5_data_scheme':

      scheme = origin.attrs['TRIQS_HDF5_data_scheme']
      try:
        new_name = modifications[scheme]['SSS']
      except:
        new_name = scheme
      dest.attrs.create('TRIQS_HDF5_data_scheme', new_name)

    else:

      dest.attrs.create(at, origin.attrs[at])

  # some special cases here
  if scheme == "GFBloc_ImFreq" or scheme == "GfImFreq": exclude = sanitize_imfreq(origin, dest)
  if scheme == "GFBloc_ImTime" or scheme == "GfImTime": exclude = sanitize_imtime(origin, dest)
  if scheme == "GFBloc_ReFreq" or scheme == "GfReFreq": exclude = sanitize_refreq(origin, dest)
  if scheme == "GFBloc_ImLegendre" or scheme == "GfLegendre": exclude = sanitize_legendre(origin, dest)

  # copy the rest
  for key in [ o for o in origin if o not in exclude ]:

    # key is a dataset
    if hasattr(origin[key],'value'):

      # check if dataset needs renaming
      try:
        new_key = modifications[scheme][key]
      except:
        new_key = key

      # create dataset and copy data as well as attributes in destination archive
      dest.create_dataset(name=new_key, data=origin[key].value)
      for at in origin[key].attrs.keys():
        dest[key].attrs.create(at, origin[key].attrs[at])

    # key is a group
    else:

      dest.create_group(name=key)
      copy_and_correct(origin[key], dest[key])


# Let's do it
A = h5py.File(sys.argv[1],'r')
B = h5py.File(sys.argv[2],'w')

print "Starting update..."
copy_and_correct(A,B)
print "Update done and written in", sys.argv[2]


