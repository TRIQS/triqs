class Triqs < Formula
  desc "Toolbox for Research on Interacting Quantum Systems"
  homepage "https://triqs.github.io"
  url "https://github.com/TRIQS/triqs/releases/download/3.0.2/triqs-3.0.2.tar.gz"
  sha256 "0420d214f66436b2652d0bdd17c41e96f72d7eacff636a88c1649f18a585711c"
  head "https://github.com/TRIQS/triqs.git"

  # doi "10.1016/j.cpc.2015.04.023"
  # tag "quantumphysics"

  depends_on "cmake" => :build
  depends_on "boost"
  depends_on "hdf5"
  depends_on "gmp"
  depends_on "fftw"
  depends_on "open-mpi"
  depends_on "python"
  depends_on "scipy"
  depends_on "numpy"
  depends_on "pkg-config" => :run

  def install
    system "pip", "install", "--user", "--upgrade", "pip"
    system "pip", "install", "--user", "--upgrade", "mpi4py"
    system "pip", "install", "--user", "--upgrade", "matplotlib"
    system "pip", "install", "--user", "--upgrade", "mako"
    system "pip", "install", "--user", "--upgrade", "jupyter"

    ENV.cxx11

    args = %W[
      ..
      -DCMAKE_BUILD_TYPE=Release
      -DCMAKE_INSTALL_PREFIX=#{prefix}
    ]

    mkdir "build" do
      system "cmake", *args
      system "make", "test"
      system "make", "install"
    end

    chmod 0555, bin/"clang_parser.py"
    chmod 0555, bin/"cpp2doc_tools.py"
  end

  test do
    system "python -c 'import triqs'"
  end
end
