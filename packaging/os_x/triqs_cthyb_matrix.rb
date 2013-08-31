require 'formula'

class TriqsCthybMatrix < Formula
  homepage 'ipht.cea.fr/triqs/XXX'
  #head 'https://github.com/TRIQS/cthyb_matrix.git'
  version '1.0'
  url 'file:///Users/parcolle/triqs/src/cthyb_matrix', :using => :git
  #url 'https://github.com/TRIQS/cthyb_matrix/XXXXX.tar.gz' 
  #sha1 ''

  depends_on 'cmake' => :build
  depends_on 'triqs'

  def install
    args=["-DTRIQS_PATH=/usr/local/"]
    system "cmake", ".", *args
    system "make -j8 " # if this fails, try separate make/make install steps
    system "make test"
    system "make -j8 install" # if this fails, try separate make/make install steps
  end

  def uninstall
    system "rm /usr/local/bin/pytriqs"
  end

end
