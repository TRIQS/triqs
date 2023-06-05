# Copyright (c) 2022 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

from spack import *

class Triqs(CMakePackage):
    """TRIQS: a Toolbox for Research on Interacting Quantum Systems"""

    homepage = "https://triqs.github.io"
    url      = "https://github.com/TRIQS/triqs/archive/refs/tags/3.0.2.tar.gz"

    version('3.0.2', sha256='0420d214f66436b2652d0bdd17c41e96f72d7eacff636a88c1649f18a585711c')
    version('3.0.x', git='https://github.com/TRIQS/triqs.git', branch='3.0.x')

    variant('libclang', default=True, description='Build against libclang to enable c++2py support. ')

    # TRIQS Dependencies
    depends_on('cmake', type='build')
    depends_on('mpi', type=('build', 'link'))
    depends_on('blas', type=('build', 'link'))
    depends_on('lapack', type=('build', 'link'))
    depends_on('fftw@3:', type=('build', 'link'))
    depends_on('boost', type=('build', 'link'))
    depends_on('gmp', type=('build', 'link'))
    depends_on('hdf5', type=('build', 'link'))
    depends_on('llvm', type=('build', 'link'), when='+libclang')
    depends_on('python@3.9:', type=('build', 'link', 'run'))
    depends_on('py-scipy', type=('run'))
    depends_on('py-numpy', type=('run'))
    depends_on('py-h5py', type=('run'))
    depends_on('py-mpi4py', type=('run'))
    depends_on('py-matplotlib', type=('run'))
    depends_on('py-mako', type=('run'))
    depends_on('py-sphinx', type=('run'))

    extends('python')
