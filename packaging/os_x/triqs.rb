class Triqs < Formula
  desc "Toolbox for Research on Interacting Quantum Systems"
  homepage "https://triqs.github.io"
  url "https://github.com/TRIQS/triqs/archive/2.1-rc1.tar.gz"
  sha256 "461e9493616bb5b9c26227914ee292d6c3ffad8841238668449633724fb7d2e5"
  head "https://github.com/TRIQS/triqs.git"

  # doi "10.1016/j.cpc.2015.04.023"
  # tag "quantumphysics"

  depends_on "cmake" => :build
  depends_on "boost"
  depends_on "hdf5"
  depends_on "gmp"
  depends_on "fftw"
  depends_on "open-mpi"
  depends_on "python@2"
  depends_on "scipy"
  depends_on "numpy"
  depends_on "pkg-config" => :run

  def install
    system "pip", "install", "--user", "--upgrade", "pip"
    system "pip", "install", "--user", "--upgrade", "--force-reinstall", "--no-binary=h5py", "h5py"
    system "pip", "install", "--user", "--upgrade", "--force-reinstall", "--no-binary=mpi4py", "mpi4py"
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
    system "python -c 'import pytriqs'"
  end
end
