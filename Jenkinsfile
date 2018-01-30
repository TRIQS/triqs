properties([
  disableConcurrentBuilds()
])

/* map of all builds to run, populated below */
def platforms = [:]

def dockerPlatforms = ["ubuntu-clang", "ubuntu-gcc", "centos-gcc"]
/* .each is currently broken in jenkins */
for (int i = 0; i < dockerPlatforms.size(); i++) {
  def platform = dockerPlatforms[i]
  platforms[platform] = { ->
    stage(platform) {
      timeout(time: 1, unit: 'HOURS') {
	node('docker') {
	  checkout scm
	  /* construct a Dockerfile for this base */
	  sh '''
	    ( cat packaging/Dockerfile.$STAGE_NAME ; sed '0,/^FROM /d' Dockerfile.build ) > Dockerfile
	  '''
	  /* build and tag */
	  def img = docker.build("flatironinstitute/triqs:${env.BRANCH_NAME}-${env.STAGE_NAME}")
	}
      }
    }
  }
}

def osxPlatforms = [
  ["gcc", ['CC=gcc-7', 'CXX=g++-7']],
  ["clang", ['LLVM=/usr/local/opt/llvm', 'CC=/usr/local/opt/llvm/bin/clang', 'CXX=/usr/local/opt/llvm/bin/clang++']]
]
for (int i = 0; i < osxPlatforms.size(); i++) {
  def platformEnv = osxPlatforms[i]
  def platform = platformEnv[0]
  platforms["osx-$platform"] = { ->
    stage("osx-$platform") {
      timeout(time: 1, unit: 'HOURS') {
	node('osx && triqs') {
	  withEnv(['PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin']+platformEnv[1]) {
	    checkout scm
	    /* should we make this a proper submodule? */
	    sh '[[ -d cpp2py ]] || git clone https://github.com/TRIQS/cpp2py && git -C cpp2py pull && git -C cpp2py describe --always'
	    sh 'mkdir -p cpp2py.build build install'
	    sh 'virtualenv pyvenv'
	    sh '''#!/bin/bash -ex
	      INSTALL=$PWD/install
	      source pyvenv/bin/activate
	      pip install --no-binary=h5py,mpi4py -r packaging/requirements.txt

	      cd cpp2py.build
	      cmake ../cpp2py -DCMAKE_INSTALL_PREFIX=$INSTALL
	      make
	      make install
	      cd ..
	      source $INSTALL/share/cpp2pyvars.sh

	      cd build
	      # Is there a better way to force brew's clang to find its own libc++ library?
	      cmake .. -DCMAKE_INSTALL_PREFIX=$INSTALL ${LLVM:+-DCMAKE_CXX_FLAGS=-I$LLVM/include -DCMAKE_EXE_LINKER_FLAGS=-L$LLVM/lib -DCMAKE_SHARED_LINKER_FLAGS=-L$LLVM/lib}
	      make -j2
	      make test
	      make install
	    '''
	  }
	}
      }
    }
  }
}

parallel platforms
