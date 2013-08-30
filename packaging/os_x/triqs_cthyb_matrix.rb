require 'formula'

class Triqs_cthyb_matrix < Formula
  homepage 'ipht.cea.fr/triqs/XXX'
  head 'https://github.com/TRIQS/cthyb_matrix.git'
  url 'https://github.com/TRIQS/cthyb_matrix/XXXXX.tar.gz' 
  #sha1 ''

  depends_on 'triqs'

  def install
    args=["-DTRIQS_PATH=/usr/local/"]
    system "cmake", ".", *args
    system "make -j8 " # if this fails, try separate make/make install steps
    system "make test"
    system "make -j8 install" # if this fails, try separate make/make install steps
  end

end
