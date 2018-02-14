properties([
  disableConcurrentBuilds(),
  buildDiscarder(logRotator(numToKeepStr: '10', daysToKeepStr: '30'))
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
  ["clang", ['CC=/usr/local/opt/llvm/bin/clang', 'CXX=/usr/local/opt/llvm/bin/clang++', 'CXXFLAGS=-I/usr/local/opt/llvm/include', 'LDFLAGS=-L/usr/local/opt/llvm/lib']]
]
for (int i = 0; i < osxPlatforms.size(); i++) {
  def platformEnv = osxPlatforms[i]
  def platform = platformEnv[0]
  platforms["osx-$platform"] = { ->
    stage("osx-$platform") {
      timeout(time: 1, unit: 'HOURS') {
	node('osx && triqs') {
	  def workDir = pwd()
	  def tmpDir = pwd(tmp:true)
	  def cpp2pyDir = "$tmpDir/cpp2py"
	  def venvDir = "$tmpDir/venv"
	  def buildDir = "$tmpDir/build"
	  def installDir = "$tmpDir/install"

	  checkout scm
	  dir(cpp2pyDir) {
	    /* should we make this a proper submodule? */
	    git(url: 'https://github.com/TRIQS/cpp2py')
	    // sh '[[ -d cpp2py ]] || git clone y && git -C cpp2py pull && git -C cpp2py describe --always'
	  }

	  dir(buildDir) { withEnv(platformEnv[1]+[
	      "PATH=$installDir/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin",
	      "CPATH=$installDir/include",
	      "LIBRARY_PATH=$installDir/lib",
	      "PYTHONPATH=$installDir/lib/python2.7/site-packages",
	      "CMAKE_PREFIX_PATH=$installDir/share/cmake"]) {
	    deleteDir()
	    dir(installDir) {
	      deleteDir()
	    }
	    sh """#!/bin/bash -ex
	      virtualenv $venvDir
	      source $venvDir/bin/activate
	      pip install --no-binary=h5py,mpi4py -r $workDir/packaging/requirements.txt

	      cmake $cpp2pyDir -DCMAKE_INSTALL_PREFIX=$installDir
	      make
	      make install
	      rm -rf *

	      cmake $workDir -DCMAKE_INSTALL_PREFIX=$installDir
	      make -j2
	      make test
	      make install
	    """
	  } }
	  zip(zipFile: "osx-${platform}.zip", archive: true, dir: installDir)
	}
      }
    }
  }
}

parallel platforms
