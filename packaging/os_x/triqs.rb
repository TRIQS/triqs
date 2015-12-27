require 'formula'

class Triqs < Formula
  homepage 'ipht.cea.fr/triqs/'
  version '1.0'
  url 'file:///Users/parcolle/triqs/src/libs', :using => :git
  #head 'https://github.com/tomaok/TRIQS.git' ##Thomas'temporary fork where on-site compilation NOT required
  #url 'https://github.com/TRIQS/TRIQS/archive/master.zip' ##once the on-site compilation requirement has been removed
  #sha1 ''

  depends_on 'cmake' => :build
  depends_on 'gfortran'
  #depends_on 'hdf5'
  #depends_on 'open-mpi'
  depends_on 'fftw'
  depends_on 'gmp'
  depends_on 'gsl'
  #depends_on 'python'
  #depends_on 'zmq'
  depends_on 'boost'

  # python package needed
  #depends_on :python => 'numpy'
  #depends_on :python => 'scipy'
  #depends_on :python => 'h5py'
  #depends_on :python => 'cython'
  #depends_on :python => 'matplotlib'

  def install
    args=["-DPYTHON_INTERPRETER=/usr/local/bin/python", "-DALLOW_COMPILATION_IN_SOURCES=ON" , "-DCMAKE_INSTALL_PREFIX=/usr/local"]
    system "cmake", ".", *args
    system "make -j8 " # if this fails, try separate make/make install steps
    system "make test"
    system "make -j8 install" # if this fails, try separate make/make install steps
  end


  def caveats
   s = <<-EOS.undent
    **********************************************************************
    Uninstalling requires to clean manually as : 
    find /usr/local/ -name "*triqs*" |grep -v Library |xargs rm -rf
    **********************************************************************
   EOS
   return s
  end 

  #def test
  #  system "pytriqs"
  #end
end
